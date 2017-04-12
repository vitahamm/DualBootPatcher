/*
 * Copyright (C) 2016-2017  Andrew Gunnerson <andrewgunnerson@gmail.com>
 *
 * This file is part of MultiBootPatcher
 *
 * MultiBootPatcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MultiBootPatcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MultiBootPatcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cerrno>
#include <cinttypes>
#include <cstring>

#include <getopt.h>

#include <json/json.h>
#include <json/writer.h>
#include <yaml-cpp/yaml.h>

#include "mbdevice/json.h"


static Json::Value yaml_node_to_json_node(const YAML::Node &yaml_node)
{
    switch (yaml_node.Type()) {
    case YAML::NodeType::Null:
        return Json::nullValue;

    case YAML::NodeType::Scalar:
        try {
            return yaml_node.as<Json::LargestInt>();
        } catch (const YAML::BadConversion &e) {}
        try {
            return yaml_node.as<double>();
        } catch (const YAML::BadConversion &e) {}
        try {
            return yaml_node.as<bool>();
        } catch (const YAML::BadConversion &e) {}
        try {
            return yaml_node.as<std::string>();
        } catch (const YAML::BadConversion &e) {}
        throw std::runtime_error("Cannot convert scalar value to known type");

    case YAML::NodeType::Sequence: {
        Json::Value array;

        for (auto const &item : yaml_node) {
            array.append(yaml_node_to_json_node(item));
        }

        return array;
    }

    case YAML::NodeType::Map: {
        Json::Value object;

        for (auto const &item : yaml_node) {
            object[item.first.as<std::string>()] =
                    yaml_node_to_json_node(item.second);
        }

        return object;
    }

    case YAML::NodeType::Undefined:
    default:
        throw std::runtime_error("Unknown YAML node type");
    }
}

static void print_json_error(const std::string &path,
                             const mb::device::JsonError &error)
{
    fprintf(stderr, "%s: Error: ", path.c_str());

    switch (error.type) {
    case mb::device::JsonErrorType::PARSE_ERROR:
        fprintf(stderr, "Failed to parse generated JSON: %s\n",
                error.parser_msg.c_str());
        break;
    case mb::device::JsonErrorType::MISMATCHED_TYPE:
        fprintf(stderr, "Expected %s, but found %s at %s\n",
                error.expected_type.c_str(), error.actual_type.c_str(),
                error.context.c_str());
        break;
    case mb::device::JsonErrorType::UNKNOWN_KEY:
        fprintf(stderr, "Unknown key at %s\n", error.context.c_str());
        break;
    case mb::device::JsonErrorType::UNKNOWN_VALUE:
        fprintf(stderr, "Unknown value at %s\n", error.context.c_str());
        break;
    default:
        fprintf(stderr, "Unknown error\n");
        break;
    }
}

static void print_validation_error(const std::string &path,
                                   const std::string &id,
                                   uint64_t flags)
{
    fprintf(stderr, "%s: [%s] Error during validation (0x%" PRIx64 "):\n",
            path.c_str(), id.empty() ? "unknown" : id.c_str(), flags);

    struct mapping {
        uint64_t flag;
        const char *msg;
    } mappings[] = {
        { mb::device::VALIDATE_MISSING_ID,                        "Missing device ID" },
        { mb::device::VALIDATE_MISSING_CODENAMES,                 "Missing device codenames" },
        { mb::device::VALIDATE_MISSING_NAME,                      "Missing device name" },
        { mb::device::VALIDATE_MISSING_ARCHITECTURE,              "Missing device architecture" },
        { mb::device::VALIDATE_MISSING_SYSTEM_BLOCK_DEVS,         "Missing system block device paths" },
        { mb::device::VALIDATE_MISSING_CACHE_BLOCK_DEVS,          "Missing cache block device paths" },
        { mb::device::VALIDATE_MISSING_DATA_BLOCK_DEVS,           "Missing data block device paths" },
        { mb::device::VALIDATE_MISSING_BOOT_BLOCK_DEVS,           "Missing boot block device paths" },
        { mb::device::VALIDATE_MISSING_RECOVERY_BLOCK_DEVS,       "Missing recovery block device paths" },
        { mb::device::VALIDATE_MISSING_BOOT_UI_THEME,             "Missing Boot UI theme" },
        { mb::device::VALIDATE_MISSING_BOOT_UI_GRAPHICS_BACKENDS, "Missing Boot UI graphics backends" },
        { mb::device::VALIDATE_INVALID_ARCHITECTURE,              "Invalid device architecture" },
        { mb::device::VALIDATE_INVALID_FLAGS,                     "Invalid device flags" },
        { mb::device::VALIDATE_INVALID_BOOT_UI_FLAGS,             "Invalid Boot UI flags" },
        { 0, nullptr }
    };

    for (auto it = mappings; it->flag; ++it) {
        if (flags & it->flag) {
            fprintf(stderr, "- %s\n", it->msg);
            flags &= ~it->flag;
        }
    }

    if (flags) {
        fprintf(stderr, "- Unknown remaining flags (0x%" PRIx64 ")", flags);
    }
}

static bool validate(const std::string &path, const std::string &json,
                     bool is_array)
{
    mb::device::JsonError error;

    if (is_array) {
        std::vector<mb::device::Device> devices;

        if (!mb::device::device_list_from_json(json, devices, error)) {
            print_json_error(path, error);
            return false;
        }

        bool failed = false;

        for (auto const &device : devices) {
            uint64_t flags = device.validate();
            if (flags) {
                print_validation_error(path, device.id(), flags);
                failed = true;
            }
        }

        if (failed) {
            return false;
        }
    } else {
        mb::device::Device device;

        if (!mb::device::device_from_json(json, device, error)) {
            print_json_error(path, error);
            return false;
        }

        uint64_t flags = device.validate();
        if (flags) {
            print_validation_error(path, device.id(), flags);
            return false;
        }
    }

    return true;
}

static void usage(FILE *stream)
{
    fprintf(stream,
            "Usage: devicesgen [OPTION]...\n"
            "\n"
            "Options:\n"
            "  -o, --output <file>\n"
            "                   Output file (outputs to stdout if omitted)\n"
            "  -h, --help       Display this help message\n"
            "  --styled         Output in human-readable format\n");
}

int main(int argc, char *argv[])
{
    int opt;

    enum Options {
        OPT_STYLED             = 1000,
    };

    static const char short_options[] = "o:h";

    static struct option long_options[] = {
        {"styled", no_argument, 0, OPT_STYLED},
        {"output", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int long_index = 0;

    const char *output_file = nullptr;
    bool styled = false;

    while ((opt = getopt_long(argc, argv, short_options,
                              long_options, &long_index)) != -1) {
        switch (opt) {
        case OPT_STYLED:
            styled = true;
            break;

        case 'o':
            output_file = optarg;
            break;

        case 'h':
            usage(stdout);
            return EXIT_SUCCESS;

        default:
            usage(stderr);
            return EXIT_FAILURE;
        }
    }

    Json::Value json_root;
    Json::FastWriter json_fast_writer;
    Json::StyledWriter json_styled_writer;

    for (int i = optind; i < argc; ++i) {
        try {
            YAML::Node root = YAML::LoadFile(argv[i]);
            Json::Value node = yaml_node_to_json_node(root);
            std::string output = json_fast_writer.write(node);

            if (!validate(argv[i], output, node.isArray())) {
                return EXIT_FAILURE;
            }

            if (node.isArray()) {
                for (auto const &item : node) {
                    json_root.append(item);
                }
            } else {
                json_root.append(node);
            }
        } catch (const std::exception &e) {
            fprintf(stderr, "%s: Failed to convert file: %s\n",
                    argv[i], e.what());
            return EXIT_FAILURE;
        }
    }

    FILE *fp = stdout;

    if (output_file) {
        fp = fopen(output_file, "w");
        if (!fp) {
            fprintf(stderr, "%s: Failed to open file: %s\n",
                    output_file, strerror(errno));
            return EXIT_FAILURE;
        }
    }

    std::string output;
    if (styled) {
        output = json_styled_writer.write(json_root);
    } else {
        output = json_fast_writer.write(json_root);
    }

    if (fwrite(output.data(), 1, output.size(), fp) != output.size()) {
        fprintf(stderr, "Failed to write JSON: %s\n", strerror(errno));
    }

    if (output_file) {
        if (fclose(fp) != 0) {
            fprintf(stderr, "%s: Failed to close file: %s\n",
                    output_file, strerror(errno));
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
