#include "PricingConfigLoader.h"

#include <cassert>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

std::string PricingConfigLoader::getConfigFile() const {
    return configFile_;
}

void PricingConfigLoader::setConfigFile(const std::string& file) {
    configFile_ = file;
}

static std::string read_file_to_string(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static std::vector<std::string> split_lines(const std::string& s) {
    std::vector<std::string> lines;
    std::string line;
    std::istringstream iss(s);
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); // handle CRLF
        }
        lines.push_back(std::move(line));
    }
    return lines;
}

// Used chatgpt to generate this
// Told it to use simple regex for the scope of this task. Really you'd be insane not to use an xml library
static PricingEngineConfig
parse_pricing_engine_config_xml(const std::string& filepath)
{
    std::string xml = read_file_to_string(filepath);
    auto lines = split_lines(xml);

    if (lines.size() < 3) {
        throw std::runtime_error("Invalid XML: too few lines");
    }

    // Line 1: XML declaration
    {
        // Has BOM at start so ignore this
        size_t pos = 0;
        if (lines[0].size() >= 3 &&
            static_cast<unsigned char>(lines[0][0]) == 0xEF &&
            static_cast<unsigned char>(lines[0][1]) == 0xBB &&
            static_cast<unsigned char>(lines[0][2]) == 0xBF)
        {
            pos = 3;
        }

        if (lines[0].compare(pos, 5, "<?xml") != 0)
        {
            throw std::runtime_error("Invalid XML declaration");
        }
    }

    // Line 2: root open tag
    {
        const std::regex rootOpenRe("^\\s*<PricingEngines>\\s*$");
        if (!std::regex_match(lines[1], rootOpenRe)) {
            throw std::runtime_error("Invalid root tag: " + lines[1]);
        }
    }

    // Engine line (attribute order exactly as in file)
    const std::regex engineRe(
        "^\\s*<Engine\\s+tradeType=\"([^\"]+)\"\\s+assembly=\"([^\"]+)\"\\s+pricingEngine=\"([^\"]+)\"\\s*/>\\s*$"
    );

    const std::regex rootCloseRe("^\\s*</PricingEngines>\\s*$");

    PricingEngineConfig result;

    for (size_t i = 2; i < lines.size(); ++i) {
        const std::string& line = lines[i];

        if (line.find_first_not_of(" \t") == std::string::npos) {
            continue; // skip blank lines
        }

        if (std::regex_match(line, rootCloseRe)) {
            return result;
        }

        std::smatch m;
        if (!std::regex_match(line, m, engineRe)) {
            throw std::runtime_error("Invalid Engine line: " + line);
        }

        PricingEngineConfigItem item;
        item.setTradeType(m[1].str());
        item.setAssembly(m[2].str());
        item.setTypeName(m[3].str());

        result.push_back(std::move(item));
    }

    throw std::runtime_error("Missing </PricingEngines> closing tag");
}

PricingEngineConfig PricingConfigLoader::loadConfig()
{
    return parse_pricing_engine_config_xml(configFile_);
}
