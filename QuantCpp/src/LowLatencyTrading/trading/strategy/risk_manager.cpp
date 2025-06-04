#include "risk_manager.hpp"
#include "order_manager.hpp"
namespace QuantCpp::Trading
{
    RiskManager::RiskManager(Logger *logger, const PositionKeeper *position_keeper,
                             const TradeEngineCfgHashMap &ticker_cfg) : logger_(logger)
    {
        for (TickerId i = 0; i < ME_MAX_TICKERS; ++i)
        {
            ticker_risk_.at(i).position_info_ = position_keeper->getPositionInfo(i);
            ticker_risk_.at(i).rick_cfg_ = ticker_cfg.at(i).risk_cfg_;
            
        }
    }

    auto RiskManager::checkPreTradeRisk(TickerId ticker_id, Side side, Qty qty) const noexcept
    {
        return ticker_risk_.at(ticker_id).checkPreTradeRisk(side, qty);
    }
}