#ifndef BONDTRADE_H
#define BONDTRADE_H

#include "BaseTrade.h"
#include <cctype>
#include <stdexcept>
#include <string>

// Gone for simple function here to strip \r from back of string
// Don't know if trailing spaces matter on corp bonds or FX, could check tests
// If knew that, could put this in base class and make sure tradeId is set via setter to always run this
inline void rstrip(std::string& s)
{
    while (!s.empty() &&
        std::isspace(static_cast<unsigned char>(s.back())))
    {
        s.pop_back();
    }
}

class BondTrade : public BaseTrade {
public:
    static constexpr const char* GovBondTradeType = "GovBond";
    static constexpr const char* CorpBondTradeType = "CorpBond";
    
    BondTrade(const std::string& tradeId, const std::string& tradeType)
        : tradeType_(tradeType) {
        if (tradeId.empty()) {
            throw std::invalid_argument("A valid non null, non empty trade ID must be provided");
        }
        tradeId_ = tradeId;
        rstrip(tradeId_);
    }
    
    std::string getTradeType() const override { return tradeType_; }
    
private:
    std::string tradeType_;
};

#endif // BONDTRADE_H

