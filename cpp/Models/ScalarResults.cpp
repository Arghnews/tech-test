#include "ScalarResults.h"
#include <stdexcept>

ScalarResults::~ScalarResults() = default;

std::optional<ScalarResult> ScalarResults::operator[](const std::string& tradeId) const {
    if (!containsTrade(tradeId)) {
        return std::nullopt;
    }

    std::optional<double> priceResult = std::nullopt;
    std::optional<std::string> error = std::nullopt;

    auto resultIt = results_.find(tradeId);
    if (resultIt != results_.end()) {
        priceResult = resultIt->second;
    }

    auto errorIt = errors_.find(tradeId);
    if (errorIt != errors_.end()) {
        error = errorIt->second;
    }

    return ScalarResult(tradeId, priceResult, error);
}

bool ScalarResults::containsTrade(const std::string& tradeId) const {
    return results_.find(tradeId) != results_.end() || errors_.find(tradeId) != errors_.end();
}

void ScalarResults::addResult(const std::string& tradeId, double result) {
    results_[tradeId] = result;
}

void ScalarResults::addError(const std::string& tradeId, const std::string& error) {
    errors_[tradeId] = error;
}

ScalarResults::Iterator::Iterator(const ScalarResults* results)
    : results_(results), resultsIter_(results->results_.begin()), errorsIter_(results->errors_.begin())
{
}

ScalarResults::Iterator& ScalarResults::Iterator::operator++() {
    if (resultsIter_ != results_->results_.end())
    {
        ++resultsIter_;
    }
    else if (errorsIter_ != results_->errors_.end())
    {
        ++errorsIter_;
    }
    else
    {
        throw std::runtime_error("Iterator out of bounds");
    }
    return *this;
}

ScalarResult ScalarResults::Iterator::operator*() const {
    if (resultsIter_ != results_->results_.end())
        return ScalarResult(resultsIter_->first, resultsIter_->second, std::nullopt);
    else if (errorsIter_ != results_->errors_.end())
        return ScalarResult(errorsIter_->first, std::nullopt, errorsIter_->second);
    else
        throw std::runtime_error("Iterator out of bounds");
}

bool ScalarResults::Iterator::operator==(const Iterator& other) const {
    return resultsIter_ == other.resultsIter_ && errorsIter_ == other.errorsIter_;
}

ScalarResults::Iterator ScalarResults::begin() const {
    return ScalarResults::Iterator{this};
}

ScalarResults::Iterator ScalarResults::end() const {
    return ScalarResults::Iterator{this};
}
