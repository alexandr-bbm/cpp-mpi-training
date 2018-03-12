#include <cstdlib>
#include <vector>
#include <functional>
#include "types.h"

using namespace std;

_v_d sum_vectors(_v_d a, _v_d b) {
    _v_d result = a;
    transform (result.begin(), result.end(), b.begin(), result.begin(), plus<double>());
    return result;
}

_v_d multiply_vector(_v_d a, double b) {
    _v_d result = a;
    transform(result.begin(), result.end(), result.begin(),
              bind1st(multiplies<double>(), b));
    return result;
}

_v_d substract_vectors(_v_d a, _v_d b) {
    _v_d result = a;
    transform (result.begin(), result.end(), b.begin(), result.begin(), minus<double>());
    return result;
}
