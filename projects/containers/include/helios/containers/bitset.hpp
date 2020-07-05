#pragma once

#include <helios/macros.hpp>

#include <cassert>
#include <cstddef>

namespace helios
{
    static constexpr size_t bits_to_words(const size_t count, const size_t word_size)
    {
        const size_t bytes = (count + 1) / 8;
        return bytes;
    }

    static constexpr size_t log2(const size_t sz)
    {
        return sz < 2 ? 1 : (1 + log2(sz / 2));
    }

    template <size_t Count>
    class bitset
    {
        using word_type_t = u32;

        static constexpr size_t Wprds = bits_to_words(Count, sizeof(word_type_t));
        static constexpr size_t LogBytes = log2(Wprds);

    public:
        class reference
        {
            friend class bitset;

            constexpr reference(bitset& ref, size_t idx) : _bits(ref), _index(idx)
            {
            }

        public:
            constexpr reference(const reference&) = default;
            constexpr reference(reference&&) noexcept = default;
            ~reference() = default;

            constexpr operator bool() const noexcept
            {
                return _bits.test(_index);
            }

            constexpr reference& operator=(bool value) noexcept
            {
                _bits.set(_index, value);
                return *this;
            }

            constexpr reference& operator=(const reference& other) noexcept
            {
                _bits = other._bits;
                _index = other._index;
                return *this;
            }

            constexpr reference& operator=(reference&&) noexcept = default;

            constexpr reference& flip() noexcept
            {
                _bits.flip(_index);
                return *this;
            }

            constexpr bool operator~() const noexcept
            {
                return ~_bits.test(_index);
            }

        private:
            bitset& _bits;
            size_t _index;
        };

        constexpr bitset() : _payload(), _set(0)
        {
            for (size_t i = 0; i < Wprds; i++)
            {
                _payload[i] = 0;
            }
        }

        constexpr bitset(const bitset& other);
        constexpr bitset(bitset&& other) noexcept;
        ~bitset() = default;

        constexpr bool operator[](const size_t idx) const noexcept
        {
            assert(idx < Count);
            return test(idx);
        }

        constexpr reference operator[](const size_t idx) noexcept
        {
            assert(idx < Count);
            return reference(*this, idx);
        }

        constexpr bool test(const size_t idx) const noexcept
        {
            assert(idx < Count);
            const size_t byte = idx >> LogBytes;
            const size_t bit = idx & (8 * sizeof(word_type_t) - 1);

            return 1 & (_payload[byte] >> bit);
        }

        constexpr bitset& set(size_t pos, bool value = true) noexcept
        {
            assert(pos < Count);
            const size_t byte = pos >> LogBytes;
            const size_t bit = pos & (8 * sizeof(word_type_t) - 1);

            if (value)
            {
                _payload[byte] |= (1 << (bit - 1));
                ++_set;
            }
            else
            {
                _payload[byte] &= ~(1 << (bit - 1));
                --_set;
            }

            return *this;
        }

        constexpr bitset& set() noexcept
        {
            for (size_t i = 0; i < Count; i++)
            {
                set(i, true);
            }
            return *this;
        }

        constexpr bitset& reset(size_t pos) noexcept
        {
            assert(pos < Count);
            return set(pos, false);
        }

        constexpr bitset& reset() noexcept
        {
            for (size_t i = 0; i < Count; i++)
            {
                set(i, false);
            }

            return *this;
        }

        constexpr bitset& flip(size_t pos) noexcept
        {
            assert(pos < Count);
            const size_t byte = pos >> LogBytes;
            const size_t bit = pos & (8 * sizeof(word_type_t) - 1);

            _payload[byte] ^= (1 << (bit - 1));
            if (test(pos))
            {
                ++_set;
            }
            else
            {
                --_set;
            }

            return *this;
        }

        constexpr bitset& flip() noexcept
        {
            for (size_t i = 0; i < Count; i++)
            {
                flip(i);
            }
            return *this;
        }

        constexpr bool all() const noexcept
        {
            return _set == Count;
        }

        constexpr bool any() const noexcept
        {
            return _set > 0;
        }

        constexpr bool none() const noexcept
        {
            return _set == 0;
        }

        constexpr size_t count() const noexcept
        {
            return _set;
        }

        constexpr size_t size() const noexcept
        {
            return _set;
        }

        constexpr bitset& operator&=(const bitset right) noexcept
        {
            for (size_t i = 0; i < Wprds; i++)
            {
                _payload[i] &= right._payload[i];
            }
            return *this;
        }

        constexpr bitset& operator|=(const bitset right) noexcept
        {
            for (size_t i = 0; i < Wprds; i++)
            {
                _payload[i] |= right._payload[i];
            }
            return *this;
        }

        constexpr bitset& operator^=(const bitset right) noexcept
        {
            for (size_t i = 0; i < Wprds; i++)
            {
                _payload[i] ^= right._payload[i];
            }
            return *this;
        }

        constexpr bitset& operator<<=(const size_t shift) noexcept
        {
            if (__builtin_expect(shift != 0, 1))
            {
                const size_t byte = shift >> LogBytes;
                const size_t bit = shift & (8 * sizeof(word_type_t) - 1);

                if (bit == 0)
                {
                    for (size_t i = Wprds; i >= byte; --i)
                    {
                        _payload[i] = _payload[i - byte];
                    }
                }
                else
                {
                    const size_t offset = 8 * sizeof(word_type_t) - bit;
                    for (size_t i = Wprds - 1; i > byte; --i)
                    {
                        _payload[i] = (_payload[i - byte] << offset) | (_payload[i - byte - 1] >> offset);
                    }
                    _payload[byte] = _payload[0] << offset;
                }

                for (size_t i = 0; i < byte; ++i)
                {
                    _payload[i] = static_cast<word_type_t>(0);
                }
            }
            return *this;
        }

        constexpr bitset& operator>>=(const size_t shift) noexcept
        {
            if (__builtin_expect(shift != 0, 1))
            {
                const size_t byte = shift >> LogBytes;
                const size_t bit = shift & (8 * sizeof(word_type_t) - 1);
                const size_t limit = Wprds - byte - 1;

                if (bit == 0)
                {
                    for (size_t i = 0; i <= limit; ++i)
                    {
                        _payload[i] = _payload[i + byte];
                    }
                }
                else
                {
                    const size_t offset = 8 * sizeof(word_type_t) - bit;
                    for (size_t i = Wprds - 1; i > byte; --i)
                    {
                        _payload[i] = (_payload[i + byte] >> offset) | (_payload[i + byte + 1] << offset);
                    }
                    _payload[limit] = _payload[Wprds - 1] >> offset;
                }

                for (size_t i = limit + 1; i < Wprds; ++i)
                {
                    _payload[i] = static_cast<word_type_t>(0);
                }
            }
            return *this;
        }

        constexpr bitset operator~() const noexcept
        {
            bitset cpy = *this;
            cpy.flip();
            return cpy;
        }

    private:
        word_type_t _payload[Wprds];
        size_t _set = 0;
    };

    template <size_t Count>
    inline constexpr bitset<Count> operator&(bitset<Count> left, const bitset<Count>& right) noexcept
    {
        return left &= right;
    }

    template <size_t Count>
    inline constexpr bitset<Count> operator|(bitset<Count> left, const bitset<Count>& right) noexcept
    {
        return left |= right;
    }

    template <size_t Count>
    inline constexpr bitset<Count> operator^(bitset<Count> left, const bitset<Count>& right) noexcept
    {
        return left ^= right;
    }

    template <size_t Count>
    inline constexpr bitset<Count> operator<<(bitset<Count> left, const bitset<Count>& right) noexcept
    {
        return left <<= right;
    }

    template <size_t Count>
    inline constexpr bitset<Count> operator>>(bitset<Count> left, const bitset<Count>& right) noexcept
    {
        return left >>= right;
    }

} // namespace helios
