#pragma once

#include <vector>
#include <functional>
#include <utility>

template <class T, class Container = std::vector<T>, class Compare = std::less<T>>
class PriorityQueue {
private:
    class Heap {
    private:
        Container data_;
        Compare cmp_;
        size_t size_;
        size_t size_data_;

        void Heapify(size_t curr) {
            if (curr >= size_data_) {
                return;
            }

            size_t left = 2 * curr;
            size_t right = 2 * curr + 1;

            size_t largest = curr;
            if (left < size_data_ && cmp_(data_[curr], data_[left])) {
                largest = left;
            }
            if (right < size_data_ && cmp_(data_[curr], data_[right])) {
                largest = right;
            }

            if (largest != curr) {
                std::swap(data_[largest], data_[curr]);
                Heapify(largest);
            }
        }

        void IncreaseKey(size_t curr, const T& value) {
            data_[curr] = value;
            while (curr > 1 && cmp_(data_[curr >> 1], data_[curr])) {
                std::swap(data_[curr], data_[curr >> 1]);
                curr >>= 1;
            }
        }

    public:
        Heap() : size_(0), size_data_(1) {
            data_.push_back(T());
        }

        explicit Heap(Compare cmp) : cmp_(cmp), size_(0), size_data_(1) {
            data_.push_back(T());
        }

        void Push(const T& new_el) {
            ++size_;
            data_.push_back(new_el);
            ++size_data_;
            IncreaseKey(size_data_ - 1, new_el);
        }

        void Pop() {
            --size_;
            std::swap(data_[1], data_[size_data_ - 1]);
            data_.pop_back();
            --size_data_;
            Heapify(1);
        }

        size_t Size() const {
            return size_;
        }

        T Top() const {
            return data_[1];
        }

        bool Empty() const {
            return size_ == 0;
        }
    };

    Heap heap_;

public:
    PriorityQueue() {
    }

    explicit PriorityQueue(Compare cmp) : heap_(cmp) {
    }

    size_t Size() const {
        return heap_.Size();
    }

    void Push(const T& value) {
        heap_.Push(value);
    }

    void Pop() {
        heap_.Pop();
    }

    T Top() const {
        return heap_.Top();
    }

    bool Empty() const {
        return heap_.empty();
    }
};
