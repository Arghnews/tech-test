#ifndef SCALARRESULTS_H
#define SCALARRESULTS_H

#include "IScalarResultReceiver.h"
#include "ScalarResult.h"
#include <map>
#include <vector>
#include <optional>
#include <string>
#include <iterator>
#include <type_traits>

class ScalarResults : public IScalarResultReceiver {
public:
    ScalarResults() = default;

    // So we can ensure we don't have to worry about iterator invalidation concerns at least now
    // Not strictly necessary, but it's the case anyway
    ScalarResults(const ScalarResults&) = delete;
    ScalarResults& operator=(const ScalarResults&) = delete;
    ScalarResults(ScalarResults&&) = default;
    ScalarResults& operator=(ScalarResults&&) = default;

    virtual ~ScalarResults();
    std::optional<ScalarResult> operator[](const std::string& tradeId) const;

    bool containsTrade(const std::string& tradeId) const;

    virtual void addResult(const std::string& tradeId, double result) override;

    virtual void addError(const std::string& tradeId, const std::string& error) override;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const ScalarResult;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator() = default;
        explicit Iterator(const ScalarResults* results);

        // Iterator must be constructable from ScalarResults parent
        Iterator& operator++();
        ScalarResult operator*() const;
        bool operator==(const Iterator& other) const;
    private:
        using ResultsIter = std::map<std::string, double>::const_iterator;
        using ErrorsIter = std::map<std::string, std::string>::const_iterator;

        const ScalarResults* results_ = nullptr;
        ResultsIter resultsIter_{};
        ErrorsIter errorsIter_{};
    };

    Iterator begin() const;
    Iterator end() const;

private:
    std::map<std::string, double> results_;
    std::map<std::string, std::string> errors_;
};

#endif // SCALARRESULTS_H
