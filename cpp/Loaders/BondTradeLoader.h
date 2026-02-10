#ifndef BONDTRADELOADER_H
#define BONDTRADELOADER_H

#include "ITradeLoader.h"
#include "../Models/BondTrade.h"
#include "../Models/BondTradeList.h"
#include <string>
#include <vector>
#include <memory>

class BondTradeLoader : public ITradeLoader {
private:
    std::string dataFile_;
    
    void loadTradesFromFile(const std::string& filename, BondTradeList& tradeList);
    
public:
    static constexpr char separator = ',';
    std::vector<std::unique_ptr<ITrade>> loadTrades() override;
    std::string getDataFile() const override;
    void setDataFile(const std::string& file) override;
};

#endif // BONDTRADELOADER_H
