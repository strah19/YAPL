#ifndef VEC_H
#define VEC_H

#include <string.h>

#include "err.h"

using Lint = long long;

template <typename T>
class Vec {
public:
    Vec(Lint init_cap = 100);
    ~Vec();

    Lint push(const T& data);
    T pop();
    void clear();
    T& back();

    void factor(Lint f);
    Lint size() const;
    T& operator[](Lint i);
    inline T* array() { return arr; }
private:
    T* arr;
    Lint cap;
    Lint len;
    Lint f = 2;    //factor of the cap when full
};

template <typename T>
Vec<T>::Vec(Lint init_cap) : cap(init_cap), arr(new T[init_cap]), len(0) { }

template <typename T>
Vec<T>::~Vec() {
    delete [] arr;
}

template <typename T>
Lint Vec<T>::push(const T& data) {
    if (len == cap) {
        T* old = arr;
        arr = new T[cap = cap * f]; //this will double the size of the array when it is full
        memcpy(arr, old, sizeof(T) * len);
        delete[] old;
    }
    arr[len++] = data;

    return len;
}

template <typename T>
T Vec<T>::pop() {
    return arr[len-- - 1];
}

template <typename T>
Lint Vec<T>::size() const {
    return len;
}

template <typename T> 
void Vec<T>::factor(Lint f) {
    if (f > 1) {
        this->f = f;
        return;
    }
    report_error("factor of Vec must be greater than 1.\n");
}

template <typename T>
T& Vec<T>::operator[](Lint i) {
    if (i >= len) {
        fatal_error("vec index out of bound.\n");
    }
    return *(arr + i);
}

template <typename T>
void Vec<T>::clear() {
    len = 0;
}

template <typename T>
T& Vec<T>::back() {
    return arr[len - 1];
}

using String = Vec<char>;

#endif // !VEC_H