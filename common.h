#include <cstdlib>
#include <vector>
#include <functional>
#include <iostream>
#include "types.h"

#ifndef LAB2_COMMON_H
#define LAB2_COMMON_H

#endif //LAB2_COMMON_H

using namespace std;

_v_d sum_vectors(_v_d a, _v_d b);

_v_d multiply_vector(_v_d a, double b);

_v_d substract_vectors(_v_d a, _v_d b);

template<class T>
void cout_vector(_v<T> v) {
    for (auto col: v) {
        cout << col << ' ';
    }
    cout << endl;
}

template<class T>
void cout_2d_vector(_v<_v<T>> v) {
    for (auto row : v) {
        cout_vector(row);
    }
    cout << endl;
}
