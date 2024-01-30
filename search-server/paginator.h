#pragma once

#include <vector>

template <typename Iterator>
class IteratorRange {
public:

    IteratorRange() = default;
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {}

    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }

    size_t size() const {
        return end_ - begin_;
    }

private:
    Iterator begin_;
    Iterator end_;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& output, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        output << *it;
    }

    return output;
}

template <typename Iterator>
class Paginator {
public:

    explicit Paginator(Iterator range_begin, Iterator range_end, size_t page_size) {
        while (range_begin != range_end) {
            auto it = range_end;
            if (range_end - range_begin > page_size) it = range_begin + page_size;
            pages_.push_back({ range_begin, it });
            range_begin = it;
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    size_t size() const {
        return pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};