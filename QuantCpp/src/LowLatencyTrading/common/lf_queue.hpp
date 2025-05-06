#pragma once

#include <iostream>
#include <vector>
#include <atomic>

#include "macros.hpp"

namespace QuantCpp::Common
{

    template <typename T>
    class LFQueue final
    {
    public:
        LFQueue(size_t num_element)
            : store_(num_element, T()) // 이니셜라이저 리스트로 수정
        {
        }

        auto getNextWriteTo() noexcept
        {
            return &store_[next_write_index_];
        }
        auto updateWriteIndex() noexcept
        {
            next_write_index_ = (next_write_index_ + 1) % store_.size();
            num_elements_++;
        }
        auto getNextToRead() const noexcept -> const T *
        {
            return (size() ? &store_[next_read_index_] : nullptr);
        }

        auto updateReadIndex() noexcept
        {
            next_read_index_ = (next_read_index_ + 1) % store_.size();
            ASSERT(num_elements_ != 0, "Read an invalid element in: " +
                                           std::to_string(pthread_self()));
            num_elements_--;
        }

        auto size() const noexcept
        {
            return num_elements_.load();
        }

        LFQueue() = delete;
        LFQueue(const LFQueue &) = delete;
        LFQueue(const LFQueue &&) = delete;
        LFQueue &operator=(const LFQueue &) = delete;
        LFQueue &operator=(const LFQueue &&) = delete;

    private:
        std::vector<T> store_;
        std::atomic<size_t> next_write_index_ = {0};
        std::atomic<size_t> next_read_index_ = {0};
        std::atomic<size_t> num_elements_ = {0};
    };
}
