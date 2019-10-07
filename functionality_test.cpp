#include "arbitrary_dim_array.hpp"
#include <cassert>

// Program to test Arbitrary Dimension Array implementation
int main() {

    // Define a [2 X 3 X 4] array of integers
    nm::Array<int, 2, 3, 4> arr1, arr2;
    nm::Array<short, 2, 3, 4> arr3;

    // Initialize the arrays
    int value = 0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                arr1[i][j][k] = arr2[i][j][k] = value;
                arr3[i][j][k] = static_cast<short>(value);
                ++value;

                assert(arr1[i][j][k] == arr2[i][j][k]);
                assert(arr2[i][j][k] == arr3[i][j][k]);
            }
        }
    }

    // Range checked indexing
    arr1[0][0][0] = 1234;
    arr1[1][1][1] = arr1[0][0][0];
    arr1[0][2][3] = 5678;

    // Out of range, throws exception
    try {
        arr1[0][3][0] = 1;
        assert(false);
    } catch (nm::Out_Of_Range_Exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    // Assignment Operator
    arr1 = arr1;  // Self assignment is NOOP
    arr2 = arr1;  // Same dimensions and types
    arr1 = arr3;  // Member template constructor

    // Test the iterator addresses (Value semantics)
    assert(arr1.fmbegin() == arr1.fmbegin());
    assert(arr2.lmbegin() == arr2.lmbegin());

    // Iterator through array in Row Major Order
    std::cout << "Array elements in Row Major Order using First Dimension Iterator:- " << std::endl;
    for (nm::Array<int, 2, 3, 4>::FirstDimensionIterator it = arr1.fmbegin(); it != arr1.fmend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n" << std::endl;

    // Iterator through array in Column Major Order
    std::cout << "Array elements in Column Major Order using Last Dimension Iterator:- " << std::endl;
    for (nm::Array<int, 2, 3, 4>::LastDimensionIterator it = arr1.lmbegin(); it != arr1.lmend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // Test the type of Array object
    assert(typeid(nm::Array<double, 1>::ValueType) == typeid(double));
}
