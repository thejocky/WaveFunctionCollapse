#pragma once

template <typename T> class Array2D {

    size_t ySize_;
    size_t xSize_;
    T* data_;

    public:

    Array2D(size_t y, size_t x) 
        : ySize_(y), xSize_(x),
            data_(new T[y*x]{NULL}) {}

    size_t yLen() {return ySize_;}
    size_t xLen() {return xSize_;}

    T index(size_t y, size_t x) {
        return data_[y*xSize_ + x];
    }
    
    T set(size_t y, size_t x, T data) {
        T tmp = data_[y*xSize_ + x];
        data_[y*xSize_ + x] = data;
        return tmp;
    }

    T* operator[](size_t index) {
        return data_ + (index*xSize_);
    }

};

