#ifndef BASETRADE_H
#define BASETRADE_H

#include "ITrade.h"
#include <cctype>
#include <chrono>
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

class BaseTrade : public ITrade {
public:
    BaseTrade() = default;
    virtual ~BaseTrade() = default;
    
    std::chrono::system_clock::time_point getTradeDate() const override { return tradeDate_; }
    void setTradeDate(const std::chrono::system_clock::time_point& date) override { tradeDate_ = date; }
    
    std::string getInstrument() const override { return instrument_; }
    void setInstrument(const std::string& instrument) override { instrument_ = instrument; }
    
    std::string getCounterparty() const override { return counterparty_; }
    void setCounterparty(const std::string& counterparty) override { counterparty_ = counterparty; }
    
    double getNotional() const override { return notional_; }
    void setNotional(double notional) override { notional_ = notional; }
    
    double getRate() const override { return rate_; }
    void setRate(double rate) override { rate_ = rate; }
    
    std::string getTradeType() const override = 0;
    std::string getTradeId() const override { return tradeId_; }
    
protected:
    std::string tradeId_;
    
private:
    std::chrono::system_clock::time_point tradeDate_;
    std::string instrument_;
    std::string counterparty_;
    double notional_ = 0.0;
    double rate_ = 0.0;
};

#endif // BASETRADE_H

