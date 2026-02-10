#include "FxTradeLoader.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>


// Need a multi char split, since the file is in utf8 and the delim ¬ is 2 bytes
// getline won't do it
// From chatgpt
static std::vector<std::string>
split_line(std::string_view input, std::string_view delim)
{
    std::vector<std::string> tokens;

    while (true) {
        auto pos = input.find(delim);
        if (pos == std::string_view::npos) {
            tokens.emplace_back(input);
            break;
        }

        tokens.emplace_back(input.substr(0, pos));
        input.remove_prefix(pos + delim.size());
    }

    return tokens;
}

static std::unique_ptr<FxTrade> createTradeFromLine(const std::string& line) {
    std::vector<std::string> items = split_line(line, "¬");

    if (items.size() == 2 && items[0] == "END") {
        return nullptr;
    }
    if (items.size() < 9) {
        throw std::runtime_error("Invalid line format");
    }

    auto trade = std::make_unique<FxTrade>(items[8], items[0]);

    // Lambda to parse YYYY-MM-DD into system_clock::time_point
    auto parseDate = [](const std::string& s) {
        std::tm tm{};
        std::istringstream ss(s);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        if (ss.fail()) {
            throw std::runtime_error("Invalid date: " + s);
        }
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    };

    trade->setTradeDate(parseDate(items[1]));
    trade->setValueDate(parseDate(items[6]));

    trade->setInstrument(items[2] + items[3]);
    trade->setCounterparty(items[7]);
    trade->setNotional(std::stod(items[4]));
    trade->setRate(std::stod(items[5]));

    return trade;
}

static void loadTradesFromFile(const std::string& filename, FxTradeList& tradeList) {
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be null");
    }

    std::ifstream stream(filename);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    int lineCount = 0;
    std::string line;
    while (std::getline(stream, line)) {
        // Has 2 header lines
        if (lineCount <= 1) {
        } else {
            // If trade is nullptr, we've hit end
            auto trade = createTradeFromLine(line);
            if (!trade)
            {
                break;
            }
            tradeList.add(std::move(trade));
        }
        lineCount++;
    }
}

std::vector<std::unique_ptr<ITrade>> FxTradeLoader::loadTrades() {
    FxTradeList tradeList;
    loadTradesFromFile(dataFile_, tradeList);

    std::vector<std::unique_ptr<ITrade>> result;
    for (size_t i = 0; i < tradeList.size(); ++i) {
        result.push_back(std::move(tradeList[i]));
    }
    return result;
}

std::string FxTradeLoader::getDataFile() const {
    return dataFile_;
}

void FxTradeLoader::setDataFile(const std::string& file) {
    dataFile_ = file;
}
