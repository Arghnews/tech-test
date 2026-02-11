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

ScalarResults::Iterator::Iterator(const ScalarResults* results, ResultsIter resultsIter, ErrorsIter errorsIter)
    : results_(results), resultsIter_(std::move(resultsIter)), errorsIter_(std::move(errorsIter))
{
}

ScalarResults::Iterator& ScalarResults::Iterator::operator++() {
    if (resultsIter_ == results_->results_.end() && errorsIter_ == results_->errors_.end()) {
        throw std::runtime_error("Iterator out of bounds");
    }

    if (resultsIter_ == results_->results_.end())
    {
        ++errorsIter_;
    }
    else if (errorsIter_ == results_->errors_.end())
    {
        ++resultsIter_;
    }

    // We need to whichever iterator is lagging behind. Except in the case where both point at the same key
    if (resultsIter_->first == errorsIter_->first) {
        ++resultsIter_;
        ++errorsIter_;
    }
    else if (resultsIter_->first < errorsIter_->first)
    {
        ++resultsIter_;
    }
    else
    {
        ++errorsIter_;
    }

    return *this;
}

ScalarResult ScalarResults::Iterator::operator*() const {
    if (resultsIter_ == results_->results_.end() && errorsIter_ == results_->errors_.end()) {
        throw std::runtime_error("Iterator out of bounds");
    }

    if (resultsIter_ != results_->results_.end())
        return {resultsIter_->first, resultsIter_->second, std::nullopt};
    if (errorsIter_ != results_->errors_.end())
        return {errorsIter_->first, std::nullopt, errorsIter_->second};

    if (resultsIter_->first < errorsIter_->first) {
        return {resultsIter_->first, resultsIter_->second, std::nullopt};
    }
    if (errorsIter_->first < resultsIter_->first) {
        return {errorsIter_->first, std::nullopt, errorsIter_->second};
    }

    return {errorsIter_->first, resultsIter_->second, errorsIter_->second};
}

bool ScalarResults::Iterator::operator==(const Iterator& other) const {
    return resultsIter_ == other.resultsIter_ && errorsIter_ == other.errorsIter_;
}

ScalarResults::Iterator ScalarResults::begin() const {
    auto i1 = results_.begin()->first;
    auto i2 = errors_.begin()->first;
    return ScalarResults::Iterator{this, results_.begin(), errors_.begin()};
}

ScalarResults::Iterator ScalarResults::end() const {
    return ScalarResults::Iterator{this, results_.end(), errors_.end()};
}
