#include "me_order_book.hpp"
#include "matching_engine.hpp" // Include the definition of MatchingEngine

namespace QuantCpp::Exchange
{
    MEOrderBook::MEOrderBook(TickerId ticker_id,
                             Logger *logger, MatchingEngine *matching_engine)
        : ticker_id_(ticker_id),
          matching_engine_(matching_engine),
          orders_at_price_pool_(ME_MAX_PRICE_LEVELS),
          order_pool_(ME_MAX_ORDER_IDS),
          logger_(logger)
    {
    }

    MEOrderBook::~MEOrderBook()
    {
        logger_->log("%:% %() % OrderBook\n%\n",
                     __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_), toString(false, true));

        matching_engine_ = nullptr;
        bids_by_price_ = asks_by_price_ = nullptr;
        for (auto &itr : cid_oid_to_order_)
        {
            itr.fill(nullptr);
        }
    }

    auto MEOrderBook::add(ClientId client_id, OrderId client_order_id,
                          TickerId ticker_id, Side side, Price price, Qty qty) noexcept -> void
    {
        const auto new_market_order_id = generateNewMarketOrderId();
        client_response_ = {ClientResponseType::ACCEPTED, client_id, ticker_id,
                            client_order_id, new_market_order_id, side, price, 0, qty};
        matching_engine_->sendClientResponse(&client_response_);

        const auto leaves_qty = checkForMatch(client_id, client_order_id, ticker_id, side, price, qty, new_market_order_id);

        if (LIKELY(leaves_qty))
        {
            const auto priority = getNextPriority(price);
            auto order = order_pool_.allocate(ticker_id, client_id,
                                              client_order_id, new_market_order_id, side, price,
                                              leaves_qty, priority, nullptr, nullptr);
            addOrder(order);

            market_update_ = {MarketUpdateType::ADD,
                              new_market_order_id,
                              ticker_id,
                              side, price, leaves_qty, priority};
            matching_engine_->sendMarketUpdate(&market_update_);
        }
    }

    auto MEOrderBook::cancel(ClientId client_id, OrderId order_id,
                             TickerId ticker_id) noexcept -> void
    {
    }

    auto MEOrderBook::toString(bool detailed, bool validity_check) const -> std::string
    {
    }

    auto MEOrderBook::addOrdersAtPrice(MEOrdersAtPrice *new_orders_at_price) noexcept
    {
    }

    auto MEOrderBook::removeOrdersAtPrice(Side side, Price price) noexcept
    {
    }

    auto MEOrderBook::getNextPriority(Price price) noexcept
    {
        const auto orders_at_price = getOrdersAtPrice(price);
        if (!orders_at_price)
        {
            return 1lu;
        }
        return orders_at_price->first_me_order_->prev_order_->priority_ + 1;
    }

    auto MEOrderBook::match(TickerId ticker_id,
                            ClientId client_id, Side side,
                            OrderId client_order_id,
                            OrderId new_market_order_id,
                            MEOrder *bid_itr, Qty *leaves_qty) noexcept
    {
        const auto order = bid_itr;
        const auto order_qty = order->qty_;
        const auto fill_qty = std::min(*leaves_qty, order_qty);

        *leaves_qty -= fill_qty;
        order->qty_ -= fill_qty;

        client_response_ = {ClientResponseType::FILLED,
                            client_id,
                            ticker_id,
                            client_order_id,
                            new_market_order_id,
                            side,
                            bid_itr->price_,
                            fill_qty,
                            *leaves_qty};
        matching_engine_->sendClientResponse(&client_response_);

        client_response_ = {ClientResponseType::FILLED,
                            order->client_id_,
                            ticker_id,
                            order->client_order_id_,
                            new_market_order_id,
                            order->side_,
                            bid_itr->price_,
                            fill_qty,
                            order->qty_};
        matching_engine_->sendClientResponse(&client_response_);

        market_update_ = {MarketUpdateType::TRADE,
                          OrderId_INVALID,
                          ticker_id,
                          side, bid_itr->price_, fill_qty, Priority_INVALID};
        matching_engine_->sendMarketUpdate(&market_update_);

        if (!order->qty_)
        {
            market_update_ = {MarketUpdateType::CANCEL,
                              order->market_order_id_,
                              ticker_id, order->side_, order->price_, order_qty, Priority_INVALID};
            matching_engine_->sendMarketUpdate(&market_update_);
            removeOrder(order);
        }
        else
        {
            market_update_ = {MarketUpdateType::MODIFY,
                              order->market_order_id_,
                              ticker_id, order->side_, order->price_, order->qty_, order->priority_};
            matching_engine_->sendMarketUpdate(&market_update_);
        }
    }

    auto MEOrderBook::checkForMatch(ClientId client_id,
                                    OrderId client_order_id,
                                    TickerId ticker_id,
                                    Side side,
                                    Price price,
                                    Qty qty,
                                    Qty new_market_order_id) noexcept
    {
        auto leaves_qty = qty;
        if (side == Side::BUY)
        {
            while (leaves_qty && asks_by_price_)
            {
                const auto ask_itr = asks_by_price_->first_me_order_;
                if (LIKELY(price < ask_itr->price_))
                {
                    break;
                }

                match(ticker_id, client_id, side, client_order_id, new_market_order_id, ask_itr, &leaves_qty);
            }
        }
        if (side == Side::SELL)
        {
            while (leaves_qty && bids_by_price_)
            {
                const auto bid_itr = bids_by_price_->first_me_order_;
                if (LIKELY(price > bid_itr->price_))
                {
                    break;
                }

                match(ticker_id, client_id,
                      side, client_order_id, new_market_order_id, bid_itr, &leaves_qty);
            }
        }
        return leaves_qty;
    }

    void MEOrderBook::removeOrder(MEOrder *order) noexcept
    {
        auto orders_at_price = getOrdersAtPrice(order->price_);

        if (order->prev_order_ == order)
        {
            removeOrdersAtPrice(order->side_, order->price_);
        }
        else
        {
            const auto order_before = order->prev_order_;
            const auto order_after = order->next_order_;
            order_before->next_order_ = order_after;
            order_after->prev_order_ = order_before;

            if (orders_at_price->first_me_order_ == order)
            {
                orders_at_price->first_me_order_ = order_after;
            }
            order->prev_order_ = order->next_order_ = nullptr;
        }

        cid_oid_to_order_.at(order->client_id_).at(order->client_order_id_) = nullptr;
        order_pool_.deallocate(order);
    }

    auto MEOrderBook::addOrder(MEOrder *order) noexcept
    {
        const auto orders_at_price = getOrdersAtPrice(order->price_);
        if (!orders_at_price)
        {
            order->next_order_ = order->prev_order_ = order;
            auto new_orders_at_price = orders_at_price_pool_.allocate(order->side_,
                                                                      order->price_, order, nullptr, nullptr);
            addOrdersAtPrice(new_orders_at_price);
        }
        else
        {
            auto first_order = (orders_at_price ? orders_at_price->first_me_order_ : nullptr);

            first_order->prev_order_->next_order_ = order;
            order->prev_order_ = first_order->prev_order_;
            order->next_order_ = first_order;
            first_order->prev_order_ = order;
        }
        cid_oid_to_order_.at(order->client_id_).at(order->client_order_id_) = order;
    }
}