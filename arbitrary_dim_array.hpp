#ifndef MS_ARBITRARY_DIM_ARRAY
#define MS_ARBITRARY_DIM_ARRAY

#include <iostream>

namespace ms {

    /*
     * Custom Exception Class -> invoked when array index is out of range
     */
    class Out_Of_Range_Exception : public std::exception {
    public:
        const char *what() const throw() {
            return "\nOut_Of_Range_Exception\n";
        }
    };

    // Basic declaration for template multidimensional Array class
    template<typename T, std::size_t... Dims>
    class Array;

    // Recursion Case for template multidimensional Array class
    template<typename T, std::size_t Dim, std::size_t... Dims>
    class Array<T, Dim, Dims...> {
    public:
        // Compile-time check for dimension of the array (should always be greater than 0).
        static_assert(Dim > 0, "Array cannot be created with less than zero dimension.");

        // Default constructor must be defined, either explicitly or implicitly.
        Array() : _array_size{Dim} {}

        // Copy constructor. The dimensionality of the source array must be the same.
        Array(const Array &array) : _array_size{Dim} {
            // Copy the elements from array to this->_array
            for (int index = 0; index < _array_size; ++index) {
                _array[index] = array[index];
            }
        }

        // Template copy constructor. The dimensionality of the source array must be the same.
        template<typename U>
        Array(const Array<U, Dim, Dims...> &array) : _array_size{Dim} {
            // Copy the elements from array to this->_array
            for (int index = 0; index < _array_size; ++index) {
                _array[index] = array[index];
            }
        }

        // Copy assigmsent operator. The dimensionality of the source array must be the same.
        // Self-assigmsent must be a no-op.
        Array &operator=(const Array &array) {

            // Throw exception if array dimensions are not same
            if (this->_array_size != array._array_size) {
                throw Out_Of_Range_Exception();
            }

            // Self-assigmsent check
            if (this != &array) {
                // Copy the elements from array to this->_array
                for (std::size_t index = 0; index < _array_size; ++index) {
                    _array[index] = array[index];
                }
            }
            return *this;
        }

        // Template copy assigmsent operator. The dimensionality of the source array must be the same. Self-assigmsent must be a no-op.
        template<typename U>
        Array &operator=(const Array<U, Dim, Dims...> &array) {

            // Throw exception if array dimensions are not same
            if (this->_array_size != array._array_size) {
                throw Out_Of_Range_Exception();
            }

            // Self-assigmsent check using C-style casting
            if (this != (Array<T, Dim, Dims...> *) &array) {
                // Copy the elements from array to this->_array
                for (std::size_t index = 0; index < _array_size; ++index) {
                    _array[index] = array[index];
                }
            }
            return *this;
        }

        // Overloaded operator [] to access array elements
        Array<T, Dims...> &operator[](std::size_t index) {

            // Throw exception if index is greater than the size of the array
            if (index >= _array_size) {
                throw Out_Of_Range_Exception();
            }

            return _array[index];
        }

        // Const overloaded operator [] to access array elements
        const Array<T, Dims...> &operator[](std::size_t index) const {

            // Throw exception if index is greater than the size of the array
            if (index >= _array_size) {
                throw Out_Of_Range_Exception();
            }

            return _array[index];
        }

        /*
         * Nested class used to iterate through the array in row-major order.
         * This iterator can be be used to read or write from the array.
         */
        class FirstDimensionIterator {
        public:
            // Default constructor
            FirstDimensionIterator() : _arr_ptr{nullptr}, _arr_index{0}, _arr_iter_size{Dim}, _is_iter_end{false} {}

            // Copy constructor
            FirstDimensionIterator(const FirstDimensionIterator &first_iter) : _arr_ptr{first_iter._arr_ptr},
                                                                                         _arr_index{first_iter._arr_index},
                                                                                         _arr_iter_size{Dim},
                                                                                         _is_iter_end{first_iter._is_iter_end},
                                                                                         _arr_rec_iter{first_iter._arr_rec_iter} {}

            // Value constructor to initialize iterator member variables
            FirstDimensionIterator(Array<T, Dim, Dims...> *arr_ptr, std::size_t arr_index,
                                        std::size_t arr_size, bool is_end) : _arr_ptr{arr_ptr},
                                                                             _arr_index{arr_index},
                                                                             _arr_iter_size{arr_size},
                                                                             _is_iter_end{is_end} {}

            // Copy assigmsent operator
            FirstDimensionIterator &operator=(const FirstDimensionIterator &first_iter) {
                // Self-assigmsent check
                if (this != &first_iter) {
                    _arr_ptr = first_iter._arr_ptr;
                    _arr_index = first_iter._arr_index;
                    _arr_iter_size = first_iter._arr_iter_size;
                    _arr_rec_iter = first_iter._arr_rec_iter;
                    _is_iter_end = first_iter._is_iter_end;
                }
                return *this;
            }

            // Function to modify end flag of iterator (recursively)
            void modify_end_flag() {
                _arr_rec_iter = _arr_ptr->_array[_arr_index].fmbegin();
                _arr_rec_iter._is_iter_end = false;
                _arr_rec_iter.modify_end_flag();
            }

            // Increments the iterator one element in row-major order and returns the incremented iterator (preincrement).
            FirstDimensionIterator &operator++() {
                // Recursive call for nested iterator objects
                ++_arr_rec_iter;
                if (_arr_rec_iter._is_iter_end) {
                    ++_arr_index;
                    if (_arr_iter_size == _arr_index) {
                        _arr_index = 0;
                        _is_iter_end = true;
                    } else {
                        _is_iter_end = false;
                    }
                    modify_end_flag();
                }
                return *this;
            }

            // Increments the iterator one element in row-major and returns an iterator pointing to element prior to incrementing (postincrement).
            FirstDimensionIterator operator++(int) {
                FirstDimensionIterator iter_ret(*this);
                ++(*this); // Using above preincrement operator
                return iter_ret;
            }

            // Returns a reference to the T at this position in the array.
            T &operator*() const {
                // Recursive call to dereference operator of nested iterators
                *_arr_rec_iter;
            }

        public:
            /*
             * Nested class member variables
             */
            Array<T, Dim, Dims...> *_arr_ptr;  // Array pointer to point respective array
            std::size_t _arr_index;       // Array current index
            std::size_t _arr_iter_size;   // Size of array
            bool _is_iter_end;            // Flag to check end of the iteration
            typename Array<T, Dims...>::FirstDimensionIterator _arr_rec_iter; // Iterator object to be used for recursive operations
        };

        /*
         * Nested class used to iterate through the array in column-major order.
         * This iterator can be be used to read or write from the array.
         */
        class LastDimensionIterator {
        public:
            // Default constructor
            LastDimensionIterator() : _arr_ptr{nullptr}, _arr_index{0}, _arr_iter_size{Dim}, _is_iter_end{false} {}

            // Copy constructor
            LastDimensionIterator(const LastDimensionIterator &last_iter) : _arr_ptr{last_iter._arr_ptr},
                                                                                      _arr_index{last_iter._arr_index},
                                                                                      _arr_iter_size{Dim},
                                                                                      _is_iter_end{last_iter._is_iter_end},
                                                                                      _arr_rec_iter{last_iter._arr_rec_iter} {}

            // Value constructor to initialize iterator member variables
            LastDimensionIterator(Array<T, Dim, Dims...> *arr_ptr, std::size_t arr_index,
                                       std::size_t arr_size, bool is_end) : _arr_ptr{arr_ptr},
                                                                            _arr_index{arr_index},
                                                                            _arr_iter_size{Dim},
                                                                            _is_iter_end{is_end} {}

            // Copy assigmsent operator
            LastDimensionIterator &operator=(const LastDimensionIterator &last_iter) {
                // Self-assigmsent check
                if (this != &last_iter) {
                    _arr_ptr = last_iter._arr_ptr;
                    _arr_index = last_iter._arr_index;
                    _arr_iter_size = last_iter._arr_iter_size;
                    _arr_rec_iter = last_iter._arr_rec_iter;
                    _is_iter_end = last_iter._is_iter_end;
                }
                return *this;
            }

            // Function to modify end flag of iterator (recursively)
            void modify_end_flag() {
                _arr_rec_iter._arr_ptr = &(_arr_ptr->_array[_arr_index]);
                _arr_rec_iter.modify_end_flag();
                _is_iter_end = _arr_rec_iter._is_iter_end;
            }

            // Increments the iterator one element in row-major order and returns the incremented iterator (preincrement).
            LastDimensionIterator &operator++() {
                _arr_index = (_arr_index + 1) % _arr_iter_size;
                if (_arr_index == 0) {
                    _is_iter_end = true;
                    ++_arr_rec_iter;
                }
                modify_end_flag();
                return *this;
            }

            // Increments the iterator one element in row-major and returns an iterator pointing to element prior to incrementing (postincrement).
            LastDimensionIterator operator++(int) {
                LastDimensionIterator iter_ret(*this);
                ++(*this); // Using above preincrement operator
                return iter_ret;
            }

            // Returns a reference to the T at this position in the array.
            T &operator*() const {
                // Recursive call to dereference operator of nested iterators
                *_arr_rec_iter;
            }

        public:
            /*
             * Nested class member variables
             */
            Array<T, Dim, Dims...> *_arr_ptr;  // Array pointer to point respective array
            std::size_t _arr_index;       // Array current index
            std::size_t _arr_iter_size;   // Size of array
            bool _is_iter_end;            // Flag to check end of the iteration
            typename Array<T, Dims...>::LastDimensionIterator _arr_rec_iter; // Iterator object to be used for recursive operations
        };

        // Returns a FirstDimensionIterator (nested classes) object pointing to the first element.
        FirstDimensionIterator fmbegin() {
            FirstDimensionIterator start_iter(this, 0, _array_size, false);
            start_iter._arr_rec_iter = _array[0].fmbegin(); // Recursive call to initialize recursive iterator
            return start_iter;
        }

        // Returns a FirstDimensionIterator (nested classes) object pointing one past the last element.
        FirstDimensionIterator fmend() {
            FirstDimensionIterator end_iter(this, 0, _array_size, true);
            end_iter._arr_rec_iter = _array[0].fmend(); // Recursive call to initialize recursive iterator
            return end_iter;
        }

        // Returns a LastDimensionIterator pointing to the first element.
        LastDimensionIterator lmbegin() {
            LastDimensionIterator start_iter(this, 0, _array_size, false);
            start_iter._arr_rec_iter = _array[0].lmbegin(); // Recursive call to initialize recursive iterator
            return start_iter;
        }

        // Returns a LastDimensionIterator pointing one past the last element.
        LastDimensionIterator lmend() {
            LastDimensionIterator end_iter(this, 0, _array_size, true);
            end_iter._arr_rec_iter = _array[0].lmend(); // Recursive call to initialize recursive iterator
            return end_iter;
        }

        // Friend functions to compare Iterators
        friend bool operator==(const FirstDimensionIterator &f_iter_1, const FirstDimensionIterator &f_iter_2) {
            // Checking basic parameters of iterators
            if (f_iter_1._arr_ptr != f_iter_2._arr_ptr || f_iter_1._arr_index != f_iter_2._arr_index
                || f_iter_1._arr_iter_size != f_iter_2._arr_iter_size || f_iter_1._is_iter_end != f_iter_2._is_iter_end) {
                return false;
            }
            // Checking the condition when iteration through array is completed and both begin and end flags are true
            if (f_iter_1._is_iter_end) return true;
            // Recursive check for nested iterator objects
            return f_iter_1._arr_rec_iter == f_iter_2._arr_rec_iter;
        }

        friend bool operator!=(const FirstDimensionIterator &f_iter_1, const FirstDimensionIterator &f_iter_2) {
            // Reusing above implementation of == overloaded operator
            return !(f_iter_1 == f_iter_2);
        }

        friend bool operator==(const LastDimensionIterator &l_iter_1, const LastDimensionIterator &l_iter_2) {
            // Checking basic parameters of iterators
            if (l_iter_1._arr_ptr != l_iter_2._arr_ptr || l_iter_1._arr_index != l_iter_2._arr_index
                || l_iter_1._arr_iter_size != l_iter_2._arr_iter_size || l_iter_1._is_iter_end != l_iter_2._is_iter_end) {
                return false;
            }
            // Checking the condition when iteration through array is completed and both begin and end flags are true
            if (l_iter_1._is_iter_end) return true;
            // Recursive check for nested iterator objects
            return l_iter_1._arr_rec_iter == l_iter_2._arr_rec_iter;
        }

        friend bool operator!=(const LastDimensionIterator &l_iter_1, const LastDimensionIterator &l_iter_2) {
            // Reusing above implementation of == overloaded operator
            return !(l_iter_1 == l_iter_2);
        }

    public:
        /*
         * Class member variables
         */
        static T ValueType;             // Member to indicate the data type of the array
        Array<T, Dims...> _array[Dim];  // Member to represent array of Array<T, Dims...> type
        std::size_t _array_size;        // Member to hold size of the member array
    };

    // Base Case for template multidimensional Array class
    template<typename T, std::size_t Dim>
    class Array<T, Dim> {
    public:
        // Compile-time check for dimension of the array (should always be greater than 0)
        static_assert(Dim > 0, "Array cannot be created with less than zero dimension.");

        // Default constructor must be defined, either explicitly or implicitly.
        Array() : _array_size{Dim} {}

        // Copy constructor. The dimensionality of the source array must be the same.
        Array(const Array &array) : _array_size{Dim} {
            // Copy the elements from array to this->_array
            for (std::size_t index = 0; index < _array_size; ++index) {
                _array[index] = array[index];
            }
        }

        // Template copy constructor. The dimensionality of the source array must be the same.
        template<typename U>
        Array(const Array<U, Dim> &array) : _array_size{Dim} {
            // Copy the elements from array to this->_array
            for (std::size_t index = 0; index < _array_size; ++index) {
                _array[index] = array[index];
            }
        }

        // Copy assigmsent operator. The dimensionality of the source array must be the same. Self-assigmsent must be a no-op.
        Array &operator=(const Array &array) {

            // Throw exception if array dimensions are not same
            if (this->_array_size != array._array_size) {
                throw Out_Of_Range_Exception();
            }

            // Self-assigmsent check
            if (this != &array) {
                // Copy the elements from array to this->_array
                for (std::size_t index = 0; index < _array_size; ++index) {
                    _array[index] = array[index];
                }
            }
            return *this;
        }

        // Template copy assigmsent operator. The dimensionality of the source array must be the same. Self-assigmsent must be a no-op.
        template<typename U>
        Array &operator=(const Array<U, Dim> &array) {

            // Throw exception if array dimensions are not same
            if (this->_array_size != array._array_size) {
                throw Out_Of_Range_Exception();
            }

            // Self-assigmsent check using C-style casting
            if (this != (Array<T, Dim> *) &array) {
                // Copy the elements from array to this->_array
                for (std::size_t index = 0; index < _array_size; ++index) {
                    _array[index] = array[index];
                }
            }
            return *this;
        }

        // Overloaded operator [] to access array elements
        T &operator[](std::size_t index) {

            // Throw exception if index is greater than the size of the array
            if (index >= _array_size) {
                throw Out_Of_Range_Exception();
            }

            return _array[index];
        }

        // Const overloaded operator [] to access array elements
        const T &operator[](std::size_t index) const {

            // Throw exception if index is greater than the size of the array
            if (index >= _array_size) {
                throw Out_Of_Range_Exception();
            }

            return _array[index];
        }

        /*
         * Nested class used to iterate through the array in row-major order.
         * This iterator can be be used to read or write from the array.
         */
        class FirstDimensionIterator {
        public:
            // Default constructor
            FirstDimensionIterator() : _arr_ptr{nullptr}, _arr_index{0}, _arr_iter_size{Dim}, _is_iter_end{false}, _arr{nullptr} {}

            // Copy constructor
            FirstDimensionIterator(const FirstDimensionIterator &first_iter) : _arr_ptr{first_iter._arr_ptr},
                                                                                         _arr_index{first_iter._arr_index},
                                                                                         _arr_iter_size{Dim},
                                                                                         _is_iter_end{first_iter._is_iter_end},
                                                                                         _arr{first_iter._arr} {}

            // Value constructor to initialize iterator member variables
            FirstDimensionIterator(T *arr_ptr, std::size_t arr_index,
                                        std::size_t arr_size, bool is_end) : _arr_ptr{arr_ptr},
                                                                             _arr_index{arr_index},
                                                                             _arr_iter_size{arr_size},
                                                                             _is_iter_end{is_end} {}

            // Copy assigmsent operator
            FirstDimensionIterator &operator=(const FirstDimensionIterator &first_iter) {
                // Self-assigmsent check
                if (this != &first_iter) {
                    _arr_ptr = first_iter._arr_ptr;
                    _arr_index = first_iter._arr_index;
                    _arr_iter_size = first_iter._arr_iter_size;
                    _is_iter_end = first_iter._is_iter_end;
                    _arr = first_iter._arr;
                }
                return *this;
            }

            // Function to modify end flag of iterator
            void modify_end_flag() {
                _arr_ptr = &(_arr->_array[_arr_index]);
            }

            // Increments the iterator one element in row-major order and returns the incremented iterator (preincrement).
            FirstDimensionIterator &operator++() {
                ++_arr_index;
                if (_arr_iter_size == _arr_index) {
                    _arr_index = 0;
                    _is_iter_end = true;
                } else {
                    _is_iter_end = false;
                }
                //modify_end_flag();
                return *this;
            }

            // Increments the iterator one element in row-major and returns an iterator pointing to element prior to incrementing (postincrement).
            FirstDimensionIterator operator++(int) {
                FirstDimensionIterator iter_ret(*this);
                ++(*this); // Using above preincrement operator
                return iter_ret;
            }

            // Returns a reference to the T at this position in the array.
            T &operator*() const {
                // Return the value from the array
                return _arr_ptr[_arr_index];
            }

        public:
            /*
             * Nested class member variables
             */
            T *_arr_ptr;                  // Array pointer to point base array
            Array<T, Dim> *_arr;          // Pointer to hold the state of the object
            std::size_t _arr_index;       // Array current index
            std::size_t _arr_iter_size;   // Size of array
            bool _is_iter_end;            // Flag to check end of the iteration
        };

        /*
         * Nested class used to iterate through the array in column-major order.
         * This iterator can be be used to read or write from the array.
         */
        class LastDimensionIterator {
        public:
            // Default constructor
            LastDimensionIterator() : _arr_index{0}, _arr_iter_size{Dim}, _is_iter_end{false}, _arr_ptr{nullptr} {}

            // Copy constructor
            LastDimensionIterator(const LastDimensionIterator &last_iter) : _arr_ptr{last_iter._arr_ptr},
                                                                                      _arr_index{last_iter._arr_index},
                                                                                      _arr_iter_size{Dim},
                                                                                      _is_iter_end{last_iter._is_iter_end} {}

            // Value constructor to initialize iterator member variables
            LastDimensionIterator(T *arr_ptr, std::size_t arr_index,
                                       std::size_t arr_size, bool is_end) : _arr_index{arr_index},
                                                                            _arr_iter_size{Dim},
                                                                            _is_iter_end{is_end} {}

            // Copy assigmsent operator
            LastDimensionIterator &operator=(const LastDimensionIterator &last_iter) {
                // Self-assigmsent check
                if (this != &last_iter) {
                    _arr_ptr = last_iter._arr_ptr;
                    _arr_index = last_iter._arr_index;
                    _arr_iter_size = last_iter._arr_iter_size;
                    _is_iter_end = last_iter._is_iter_end;
                }
                return *this;
            }

            // Function to modify end flag of iterator
            void modify_end_flag() {
                //_arr = &(_arr->_array[_arr_index]);
            }

            // Increments the iterator one element in row-major order and returns the incremented iterator (preincrement).
            LastDimensionIterator &operator++() {
                ++_arr_index;
                if (_arr_iter_size == _arr_index) {
                    _arr_index = 0;//_arr_index % _arr_iter_size;
                    _is_iter_end = true;
                } /*else {
                    _is_iter_end = false;
                }*/
                //modify_end_flag();
                return *this;
            }

            // Increments the iterator one element in row-major and returns an iterator pointing to element prior to incrementing (postincrement).
            LastDimensionIterator operator++(int) {
                LastDimensionIterator iter_ret(*this);
                ++(*this); // Using above preincrement operator
                return iter_ret;
            }

            // Returns a reference to the T at this position in the array.
            T &operator*() const {
                // Return the value from the array
                //return _arr_ptr[_arr_index];
                return _arr_ptr->_array[_arr_index];
            }

        public:
            /*
             * Nested class member variables
             */
            Array<T, Dim> *_arr_ptr;          // Pointer to hold the state of the object
            std::size_t _arr_index;       // Array current index
            std::size_t _arr_iter_size;   // Size of array
            bool _is_iter_end;            // Flag to check end of the iteration
        };

        // Returns a FirstDimensionIterator (nested classes) object pointing to the first element.
        FirstDimensionIterator fmbegin() {
            FirstDimensionIterator start_iter(_array, 0, _array_size, false);
            start_iter._arr = this;
            return start_iter;
        }

        // Returns a FirstDimensionIterator (nested classes) object pointing one past the last element.
        FirstDimensionIterator fmend() {
            FirstDimensionIterator start_iter(_array, 0, _array_size, true);
            start_iter._arr = this;
            return start_iter;
        }

        // Returns a LastDimensionIterator pointing to the first element.
        LastDimensionIterator lmbegin() {
            LastDimensionIterator start_iter(_array, 0, _array_size, false);
            start_iter._arr_ptr = this;
            return start_iter;
        }

        // Returns a LastDimensionIterator pointing one past the last element.
        LastDimensionIterator lmend() {
            LastDimensionIterator start_iter(_array, 0, _array_size, true);
            start_iter._arr_ptr = this;
            return start_iter;
        }

        // Friend functions to compare Iterators
        friend bool operator==(const FirstDimensionIterator &f_iter_1, const FirstDimensionIterator &f_iter_2) {
            // Checking basic parameters of iterators
            if (f_iter_1._arr_ptr != f_iter_2._arr_ptr || f_iter_1._arr_index != f_iter_2._arr_index || f_iter_1._arr != f_iter_2._arr
                || f_iter_1._arr_iter_size != f_iter_2._arr_iter_size || f_iter_1._is_iter_end != f_iter_2._is_iter_end) {
                return false;
            }
            // Checking the condition when iteration through array is completed and both begin and end flags are true
            if (f_iter_1._is_iter_end) return true;
            // Recursive check for nested iterator objects
            return true;
        }

        friend bool operator!=(const FirstDimensionIterator &f_iter_1, const FirstDimensionIterator &f_iter_2) {
            // Reusing above implementation of == overloaded operator
            return !(f_iter_1 == f_iter_2);
        }

        friend bool operator==(const LastDimensionIterator &l_iter_1, const LastDimensionIterator &l_iter_2) {
            // Checking basic parameters of iterators
            if (l_iter_1._arr_ptr != l_iter_2._arr_ptr || l_iter_1._arr_index != l_iter_2._arr_index
                || l_iter_1._arr_iter_size != l_iter_2._arr_iter_size || l_iter_1._is_iter_end != l_iter_2._is_iter_end) {
                return false;
            }
            // Checking the condition when iteration through array is completed and both begin and end flags are true
            if (l_iter_1._is_iter_end) return true;
            // Recursive check for nested iterator objects
            return true;
        }

        friend bool operator!=(const LastDimensionIterator &l_iter_1, const LastDimensionIterator &l_iter_2) {
            // Reusing above implementation of == overloaded operator
            return !(l_iter_1 == l_iter_2);
        }

        /*
         * Class member variables
         */
        static T ValueType;         // Member to indicate the data type of the array
        T _array[Dim];              // Member to represent array of Array<T, Dims...> type
        std::size_t _array_size;    // Member to hold size of the member array
    };
}

#endif
