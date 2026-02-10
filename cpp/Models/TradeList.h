#ifndef TRADELIST_H
#define TRADELIST_H

#include "ITrade.h"
#include "ITradeReceiver.h"
#include <vector>

class TradeList : public ITradeReceiver {
public:
    TradeList() = default;
    
    void add(std::unique_ptr<ITrade> trade) override {
        trades_.push_back(std::move(trade));
    }
    
    size_t size() const { return trades_.size(); }
    std::unique_ptr<ITrade>& operator[](size_t index) { return trades_[index]; }
    const std::unique_ptr<ITrade>& operator[](size_t index) const { return trades_[index]; }

    using iterator = std::vector<std::unique_ptr<ITrade>>::iterator;
    using const_iterator = std::vector<std::unique_ptr<ITrade>>::const_iterator;
    
    iterator begin() { return trades_.begin(); }
    iterator end() { return trades_.end(); }
    const_iterator begin() const { return trades_.begin(); }
    const_iterator end() const { return trades_.end(); }
    
private:
    std::vector<std::unique_ptr<ITrade>> trades_;
};

#endif // TRADELIST_H
