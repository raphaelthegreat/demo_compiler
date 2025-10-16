#pragma once

#include <cstdint>
#include <type_traits>

namespace Mix {

template <int num_bytes>
class Item {
    static constexpr int BITS_PER_BYTE = 6;
    static constexpr int VALUE_BITS = num_bytes * BITS_PER_BYTE;
    static constexpr int VALUE_MASK = (1U << VALUE_BITS) - 1;

public:
    constexpr Item() = default;
    constexpr Item(uint32_t raw_) : raw{raw_} {}

    template <typename... Args> requires(sizeof...(Args) == num_bytes)
    constexpr Item(Args... b) {
        int bit_pos = VALUE_BITS - BITS_PER_BYTE;
        ([&] {
            raw |= (b << bit_pos);
            bit_pos -= BITS_PER_BYTE;
        }(), ...);
    }

    constexpr uint32_t Raw() const {
        return raw;
    }

    constexpr bool Sign() const {
        return (raw >> VALUE_BITS) & 1;
    }

    constexpr int32_t Value() const {
        const int32_t value = raw & VALUE_MASK;
        return Sign() ? -value : value;
    }

private:
    uint32_t raw{};
};

using Word = Item<5>;
using Short = Item<2>;

} // namespace Mix