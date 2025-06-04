#pragma once

#include "common/macros.hpp"
#include "common/types.hpp"
#include "common/logging.hpp"

#include "exchange/order_server/client_response.hpp"

#include "market_order_book.hpp"

namespace QuantCpp::Trading
{
    struct PositionInfo
    {
        int32_t position_ = 0;
        double real_pnl_ = 0.0;
        double unreal_pnl_ = 0.0;
        double total_pnl_ = 0.0;

        std::array<double, sideToIndex(Side::MAX) + 1> open_vwap_;
        Qty volume_ = 0;
        const BBO *bbo_ = nullptr;
        
        auto toString() const -> std::string
        {
            std::stringstream ss;
            ss << "PositionInfo["
               << "position:" << position_
               << ", real_pnl:" << real_pnl_
               << ", unreal_pnl:" << unreal_pnl_
               << ", total_pnl:" << total_pnl_
               << ", volume:" << qtyToString(volume_)
               << " vwaps:[" << (position_ ? open_vwap_.at(sideToIndex(Side::BUY)) / std::abs(position_) : 0)
               << "X" << (position_ ? open_vwap_.at(sideToIndex(Side::SELL)) / std::abs(position_) : 0)
               << "] "
               << (bbo_ ? bbo_->toString() : "") << "}";
            return ss.str();
        }

        auto addFill(const Exchange::MEClientResponse *client_response,
                     Logger logger) noexcept
        {
            const auto old_position = position_;
            const auto side_index = sideToIndex(client_response->side_);
            const auto opp_side_index = sideToIndex(client_response->side_ == Side::BUY ? Side::SELL : Side::BUY);
            const auto side_value = sideToValue(client_response->side_);
            position_ += side_value * client_response->exec_qty_;
            volume_ += client_response->exec_qty_;
        }
    };
    class PositionKeeper final
    {
    public:
        PositionKeeper(Logger *logger)
            : logger_(logger) {}

        PositionKeeper() = delete;
        // Disable copy and move semantics
        PositionKeeper(const PositionKeeper &) = delete;
        PositionKeeper(const PositionKeeper &&) = delete;
        PositionKeeper &operator=(const PositionKeeper &) = delete;
        PositionKeeper &operator=(const PositionKeeper &&) = delete;

    private:
        std::string time_str_;
        Logger *logger_;
    };
} // namespace QuantCpp::Trading