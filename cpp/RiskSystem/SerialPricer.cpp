#include "SerialPricer.h"
#include <stdexcept>

#include "Models/BondTrade.h"
#include "Models/FxTrade.h"
#include "Pricers/CorpBondPricingEngine.h"
#include "Pricers/FxPricingEngine.h"
#include "Pricers/GovBondPricingEngine.h"

SerialPricer::~SerialPricer() {

}

void SerialPricer::loadPricers() {
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    for (const auto& configItem : pricerConfig)
    {
        const auto& tradeType = configItem.getTradeType();

        auto add_and_validate = [&](auto pricer) {
            auto& slot = pricers_[tradeType];
            slot = std::move(pricer);

            if (!slot->isTradeTypeSupported(tradeType)) {
                throw std::runtime_error(
                    "Pricing engine does not support trade type: " + tradeType
                );
            }
        };

        // Have chosen this way for readability
        // Yes, this has the major drawback you have to remember to keep this in sync when you add supporting a
        // trade type with a new pricing engine
        if (tradeType == FxTrade::FxForwardTradeType ||
            tradeType == FxTrade::FxSpotTradeType)
        {
            add_and_validate(std::make_unique<FxPricingEngine>());
        }
        else if (tradeType == BondTrade::GovBondTradeType)
        {
            add_and_validate(std::make_unique<GovBondPricingEngine>());
        }
        else if (tradeType == BondTrade::CorpBondTradeType)
        {
            add_and_validate(std::make_unique<CorpBondPricingEngine>());
        }
        else
        {
            throw std::runtime_error("Trade type not implemented: " + tradeType);
        }
    }
}

void SerialPricer::price(const std::vector<std::vector<std::unique_ptr<ITrade>>>& tradeContainers,
                         IScalarResultReceiver* resultReceiver) {
    loadPricers();

    for (const auto& tradeContainer : tradeContainers) {
        for (const auto& trade : tradeContainer) {
            std::string tradeType = trade->getTradeType();
            if (pricers_.find(tradeType) == pricers_.end()) {
                resultReceiver->addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
                continue;
            }

            auto&& pricer = pricers_[tradeType];
            pricer->price(*trade, resultReceiver);
        }
    }
}
