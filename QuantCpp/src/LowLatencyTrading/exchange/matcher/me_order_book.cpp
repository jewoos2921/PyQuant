#include "me_order_book.hpp"
#include "matching_engine.hpp" // Include the definition of MatchingEngine

namespace QuantCpp::Exchange
{
    explicit MEOrderBook::MEOrderBook(TickerId ticker_id,
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
    }
    auto MEOrderBook::match(TickerId ticker_id,
                            ClientId client_id, Side side,
                            OrderId client_order_id,
                            OrderId new_market_order_id,
                            MEOrder *bid_itr, Qty *leaves_qty) noexcept -> void
    {
    }
    auto MEOrderBook::checkForMatch(ClientId client_id,
                                    OrderId client_order_id,
                                    TickerId ticker_id,
                                    Side side,
                                    Price price,
                                    Qty qty,
                                    Qty new_market_order_id) noexcept -> void
    {
    }
    auto MEOrderBook::removeOrder(MEOrder *order) noexcept
    {
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