#pragma once
#include <cassert>
#include <numeric>
#include <tuple>
#include <iterator>
#include <functional>

namespace iter {

    template<typename T>
    class Enumerator {
        T* t;
    public:
        Enumerator(T* t) : t(t) { }
        ~Enumerator() = default;

        auto begin() {
            return EIter(t->begin(), 0);
        }

        auto end() {
            return EIter(t->end(), t->size());
        }

    private:
        struct EIter {
            typename T::iterator iter;
            std::size_t index;


            using value_type = std::pair<typename T::value_type, std::size_t>;
            using pointer = std::pair<typename T::value_type, std::size_t>*;
            using reference = std::pair<std::reference_wrapper<typename T::value_type>, std::size_t>;
            using deref_type = std::pair<typename T::value_type, std::size_t>;
            using iterator_category = std::forward_iterator_tag;

            EIter(typename T::iterator current, std::size_t index) : iter(current), index(index) {}

             EIter& operator++() {
                ++index;
                ++iter;
                return *this;
            }

            EIter operator++(int) {
                EIter e{iter, index};
                this->iter++;
                this->index++;
                return e;
            }

            reference operator*() {
                return std::make_pair(*iter, index);
            }

            bool operator==(const EIter& rhs) const { return iter == rhs.iter && index == rhs.index; }
            bool operator!=(const EIter& rhs) const { return !(*this == rhs); }

        };
    };

    template <typename T>
    Enumerator<T> enumerate(T& t) {
        return Enumerator(&t);
    }
}

namespace iter::win {
    using WSize = std::size_t;

    /**
     * Helper function. To keep myself from the insanity of typing out the static_cast calls everywhere.
     */
    template <typename TValue, typename SpanValue>
    inline double avg(TValue val, SpanValue sp) {
        return static_cast<double>(val) / static_cast<double>(sp);
    }

    template<typename Container, typename OutputIt, typename RangeFn, typename T = typename OutputIt::container_type::value_type>
    void container_window_acc(const Container& c, std::size_t window_size, OutputIt out_iterator, RangeFn fn) {
        assert(window_size <= c.size());
        auto begin = c.cbegin();
        auto container_end = c.cend();
        auto window_end = c.cbegin() + window_size;
        for(; window_end <= container_end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, fn);
            *out_iterator = std::move(v);
        }
    }

    /*
     * Function that takes two iterators, begin and end, and a size, the sliding range between them, that accumulate will be
     * ran on. Useful for calculating averages over a span. out_iterator must be an output iterator that can output the accumulated
     * value of the window_sized range. RangeFn is the function that is ran on each individual element in the sub ranges/sliding window element,
     * and therefore RangeFn must be a lambda/function with a parameter of T, here defined as the value_type of the output iterators container's value_type,
     * and the element type.
     */
    template<typename Iterator, typename OutputIterator, typename AccumulatorFn, typename T = typename OutputIterator::container_type::value_type>
    void accumulate_windows(Iterator begin, Iterator end, std::size_t window_size, OutputIterator out_iterator, AccumulatorFn acc_fn) {
        assert(static_cast<unsigned int>(window_size) <= (end - begin));
        auto window_end = begin + window_size;
        for(; window_end <= end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, acc_fn);
            *out_iterator = std::move(v);
        }
    }

    template<typename Iterator, typename OutputIt, typename AccumulatorFn, typename MapFn, typename T = typename OutputIt::container_type::value_type>
    void accumulate_window_transform(Iterator begin, Iterator end, WSize window_size, OutputIt out_iterator, AccumulatorFn r_fn, MapFn map_to) {
        assert(static_cast<unsigned int>(window_size) <= (end - begin));
        auto window_end = begin + window_size;
        for(; window_end <= end; window_end++, begin++) {
            auto v = std::accumulate(begin, window_end, T{}, r_fn);
            auto tv = map_to(v);
            *out_iterator = std::move(tv);
        }
    }

}

