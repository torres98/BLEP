#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

template <typename Element>
class VectorDS;

template <typename Element>
class MatrixDS;

template <typename Element>
void fill_matrix_randomly(MatrixDS<Element> &A, uint8_t min = 0, uint8_t max = 255, uint32_t seed = 0);

/**
 * Container implementing matrices and the associated mathematical operations.
 * 
 * @tparam Element Type of the values to be stored.
 */
template <typename Element>
class MatrixDS {
    protected:
        Element* M;
        uint16_t rows, columns;

        bool is_memory_static;
    
    public:
        /**
         * Create a MatrixDS object with the specified size.
         *
         * @param rows Number of rows.
         * @param columns Number of columns.
         * @param init If true the new MatrixDS object is filled with the default value (by
         *             default false).
         */
        MatrixDS(uint16_t rows, uint16_t columns, bool init = false) {
            this -> rows = rows;
            this -> columns = columns;

            if (init)
                this -> M = new Element[rows * columns]();
            else
                this -> M = new Element[rows * columns];

            this -> is_memory_static = false;
        }

        /**
         * Create a MatrixDS object using a static array as its content. 
         *
         * @param A Pointer to a static array.
         */
        MatrixDS(Element* A, uint16_t rows, uint16_t columns) {
            this -> rows = rows;
            this -> columns = columns;
            this -> M = A;

            this -> is_memory_static = true;
        }

        /**
         * Create a MatrixDS object copying the content of another one. 
         *
         * @param A MatrixDS object to be copied.
         */
        MatrixDS(const MatrixDS& A) {
            this -> rows = A.rows;
            this -> columns = A.columns;
            this -> M = new Element[rows * columns];

            this -> is_memory_static = false;

            std::copy(A.M, A.M + rows * columns, this -> M);
        }

        /**
         * Create an empty MatrixDS object. 
         */
        MatrixDS() {
            this -> rows = 0;
            this -> columns = 0;
            this -> M = nullptr;

            this -> is_memory_static = false;
        }

        /**
         * Return the number of rows of the MatrixDS object.
         * 
         * @return Number of rows of the MatrixDS object.
         */
        uint16_t nrows() const {
            return this -> rows;
        }

        /**
         * Return the number of columns of the MatrixDS object.
         * 
         * @return Number of columns of the MatrixDS object.
         */
        uint16_t ncolumns() const {
            return this -> columns;
        }

        /**
         * Return the number of elements of the MatrixDS object.
         * 
         * @return Number of elements of the MatrixDS object.
         */
        uint32_t size() const {
            return this -> rows * this -> columns;
        }

        /**
         * Set the value at the specified position of the MatrixDS object. 
         *
         * @param i Row index.
         * @param j Column index
         * @param elem Value to be inserted.
         * 
         * @throw Throws a std::invalid_argument exception if one of the following conditions is met:
         *            - the row index is greater or equal than the number of rows of the MatrixDS
         *              object
         *            - the column index is greater or equal than the number of columns of the MatrixDS
         *              object
         */
        void set(uint16_t i, uint16_t j, const Element& elem) {
            if (i >= this -> rows) {
                std::ostringstream error_log;
                error_log << "Row index " << i << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            } else if (j >= this -> columns) {
                std::ostringstream error_log;
                error_log << "Column index " << j << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            }

            (this -> M)[i * this -> columns + j] = elem;
        }

        /**
         * Check if the MatrixDS object has full row rank. 
         * 
         * @return true if the MatrixDS object has full row rank, false otherwise.
         */
        bool has_full_row_rank() const {
            MatrixDS A_ref = MatrixDS(*this);
            uint16_t n = this -> rows, m = this -> columns;

            for (uint16_t k = 0; k < n; k++) {
                // Initialize pivot row index and pivot value
                Element pivot = A_ref.M[m * k + k];
                uint16_t pivot_row = k;
                
                // look for maximum (absolute) pivot value on column k (after row k)
                for (uint16_t i = k+1; i < n; i++)
                    if (A_ref.M[m * i + k] > pivot) //should be abs
                        pivot = A_ref.M[m * i + k], pivot_row = i;
        
                // if a principal diagonal element is zero, then the
                // matrix is singular, so it can't have full rank
                if (!A_ref.M[m * k + pivot_row])
                    return false;
        
                // Swap the pivot row with current row
                if (pivot_row != k) {
                    std::swap_ranges(
                        A_ref.M + pivot_row * m,
                        A_ref.M + (pivot_row + 1) * m,
                        A_ref.M + k * m
                    );
                }

                for (uint16_t i = k+1; i < n; i++) {
                    // current_row = pivot * current_row - current_row[0] * pivot_row
                    for (uint16_t j = k+1; j < m; j++)
                        A_ref.M[m * i + j] = pivot * A_ref.M[m * i + j] - A_ref.M[m * i + k] * A_ref.M[m * k + j];

                    // set to zero the current column value
                    A_ref.M[m * i + k] = Element();
                }
            }
            
            return true;
        }
        
        /**
         * Return the result of the inner product between the specified row of the MatrixDS object
         * and the VectorDS object. 
         * 
         * @param i Row index.
         * @param v VectorDS object.
         * 
         * @return Result of the inner product.
         * 
         * @throw Throws a std::invalid_argument exception if one of the following conditions is met: 
         *            - the row index is greater or equal than the number of rows of the MatrixDS
         *              object
         *            - the number of columns of the MatrixDS object is different from the size of
         *              the VectorDS object
         */
        Element row_vector_product(uint16_t i, const VectorDS<Element> &v) const {
            uint16_t vector_size = v.size();

            if (i >= this -> rows) {
                std::ostringstream error_log;
                error_log << "Row index " << i << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            } else if (this -> columns != vector_size) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for matrix row-vector product between matrix of size " << this -> rows << "x" << this -> columns << " and result vector of size " << vector_size << ")";
                throw std::invalid_argument(error_log.str());
            } 

            Element result = Element();
            uint32_t row_offset = i * this -> columns;
            
            for (uint16_t j = 0; j < vector_size; j++)            
                result += this -> M[row_offset + j] * v.M[j];

            return result;
        }

        /**
         * Return the element at the specified position in the MatrixDS object. 
         * 
         * @param i Row index.
         * @param j Column index.
         * 
         * @return Value of the requested element.
         * 
         * @throw Throws a std::invalid_argument exception if one of the following conditions is met:
         *            - the row index is greater or equal than the number of rows of the MatrixDS
         *              object
         *            - the column index is greater or equal than the number of columns of the MatrixDS
         *              object
         */
        Element operator() (uint16_t i, uint16_t j) const { 
            if (i >= this -> rows) {
                std::ostringstream error_log;
                error_log << "Row index " << i << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            } else if (j >= this -> columns) {
                std::ostringstream error_log;
                error_log << "Column index " << j << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            }

            return (this -> M)[i * this -> columns + j];
        }

        /**
         * Return the sum of the given MatrixDS objects. 
         * 
         * @param A MatrixDS object.
         * 
         * @return Result of the sum between the MatrixDS objects.
         * 
         * @throw Throws a std::invalid_argument exception if the MatrixDS objects have different
         *        sizes.
         */
        MatrixDS operator+(const MatrixDS &A) const {
            if (this -> rows != A.rows || this -> columns != A.columns) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for sum between matrices of size " << this -> rows << "x" << this -> columns << " and " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            MatrixDS O = MatrixDS(this -> rows, this -> columns, true);
            
            for (uint32_t i = 0; i < this -> size(); i++)
                O.M[i] = (this -> M)[i] + A.M[i];

            return O;
        }

        /**
         * Return the subtraction of the given MatrixDS objects.
         * 
         * @param A MatrixDS object.
         * 
         * @return Result of the subtraction between the MatrixDS objects.
         * 
         * @throw Throws a std::invalid_argument exception if the MatrixDS objects have different
         *        sizes.
         */
        MatrixDS operator-(const MatrixDS &A) const {
            if (this -> rows != A.rows || this -> columns != A.columns) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for subtraction between matrices of size " << this -> rows << "x" << this -> columns << " and " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            MatrixDS O = MatrixDS(this -> rows, this -> columns, true);
            
            for (uint32_t i = 0; i < this -> size(); i++)
                O.M[i] = (this -> M)[i] - A.M[i];

            return O;
        }

        /**
         * Return the result of the matrix product between two MatrixDS objects.
         * 
         * @param A MatrixDS object.
         * 
         * @return Result of the product between the MatrixDS objects.
         * 
         * @throw Throws a std::invalid_argument exception if the number of columns of the first
         *        MatrixDS object is different from the number of rows of the second one.
         */
        MatrixDS operator*(const MatrixDS &A) const {
            if (this -> columns != A.rows) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for product between matrices of size " << this -> rows << "x" << this -> columns << " and " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            uint16_t n = this -> rows, m = A.columns, p = this -> columns;
            MatrixDS O = MatrixDS(n, m, true);
            
            for (uint16_t i = 0; i < n; i++)
                for (uint16_t j = 0; j < m; j++)
                    for (uint16_t h = 0; h < p; h++)
                        O.M[i*m + j] += (this -> M)[i*p + h] * A.M[h*m + j];

            return O;
        }

        /**
         * Copy the content of the given MatrixDS object in the current one.
         * 
         * @param A MatrixDS object to be copied.
         * 
         * @return Reference to the updated MatrixDS object.
         */
        MatrixDS& operator=(const MatrixDS &A) {
            if (this -> rows != A.rows || this -> columns != A.columns) {
                if (!this -> is_memory_static)
                    delete[] this -> M;
                else
                    this -> is_memory_static = false;

                this -> rows = A.rows;
                this -> columns = A.columns;
                this -> M = new Element[A.size()];
            }

            std::copy(A.M, A.M + rows * columns, this -> M);

            return *this;
        }

        /**
         * Check if any element of the MatrixDS object evaluates to true. 
         * 
         * @return true if each element of the MatrixDS object evaluates to false, false otherwise.
         */
        operator bool() const {
            return std::any_of(this -> M, this -> M + this -> size(), [](Element x) { return bool(x); });
        }

        /**
         * Check that two MatrixDS object have the same size and content.
         * 
         * @param A MatrixDS object.
         * 
         * @return true if the two MatrixDS objects have the same size and content, false otherwise.
         */
        bool operator==(const MatrixDS &A) const {
            if (this -> rows != A.rows || this -> columns != A.columns)
                return false;

            return std::equal(this -> M, this -> M + this -> size(), A.M);
        }

        ~MatrixDS() {
            if (!this -> is_memory_static)
                delete[] this -> M;
        }

        /**
         * Fill with random elements the MatrixDS object.
         * 
         * @param A MatrixDS object. #fix
         */
        template <typename Elem>
        friend void fill_matrix_randomly(MatrixDS<Elem> &A, uint8_t min, uint8_t max, uint32_t seed);

        friend std::ostream& operator<<(std::ostream &s, const MatrixDS &A) {
            uint16_t n = A.rows, m = A.columns;

            if (n == 0 || m == 0) {
                s << "[]" << std::endl;
            } else {
                for (uint16_t i = 0; i < n; i++) {
                    s << "[";

                    for (uint16_t j = 0; j + 1 < m; j++)
                        s << A.M[i*m + j] << ", ";

                    s << A.M[(i+1)*m - 1] << "]" << std::endl;
                }
            }

            return s;
        }
};

/**
 * Container implementing vectors and the associated mathematical operations.
 * 
 * @tparam Element Type of the values to be stored.
 */
template <typename Element>
class VectorDS: public MatrixDS<Element> {

    public:
        /**
         * Create a VectorDS object with the specified size.
         *
         * @param size Number of elements.
         * @param row_vector If true the new VectorDS object is shaped as a row vector (by
         *                   default false).
         * @param init If true the new VectorDS object is filled with the default value (by
         *             default false).
         */
        VectorDS(uint16_t size, bool row_vector = false, bool init = false) :
            MatrixDS<Element>(row_vector ? 1: size, row_vector ? size: 1, init) {} //tip: fix zero size

        /**
         * Create a VectorDS object by casting the given MatrixDS object. 
         *
         * @param A MatrixDS object to be casted.
         * 
         * @throw Throws a std::invalid_argument exception if the MatrixDS object has more than
         *        one row and column.
         */
        VectorDS(const MatrixDS<Element> &A) : MatrixDS<Element>(A) {
            if (A.nrows() > 1 && A.ncolumns() > 1) {
                std::ostringstream error_log;
                error_log << "MatrixDS object of size " << A.nrows() << "x" << A.ncolumns() << " cannot be casted to VectorDS.";
                throw std::invalid_argument(error_log.str());
            }
        }

        /**
         * Create an empty VectorDS object. 
         */
        VectorDS() :
            MatrixDS<Element>() {}

        /**
         * Set the value at the specified position of the VectorDS object. 
         *
         * @param i Vector index.
         * @param elem Value to be inserted.
         * 
         * @throw Throws a std::invalid_argument exception if the vector index exceeds the
         *        size of the VectorDS object.
         */
        void set(uint16_t i, const Element& elem) {
            if (i >= this -> size()) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << this -> size();
                throw std::invalid_argument(error_log.str());
            }

            (this -> M)[i] = elem;
        }
        
        /**
         * Return the element at the specified position in the VectorDS object. 
         * 
         * @param i Vector index.
         * 
         * @return Value of the requested element.
         * 
         * @throw Throws a std::invalid_argument exception if the vector index exceeds the
         *        size of the VectorDS object.
         */
        Element operator() (uint16_t i) const {
            if (i >= this -> size()) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << this -> size();
                throw std::invalid_argument(error_log.str());
            }

            return (this -> M)[i];
        }

        /**
         * Return the result of the inner product between two VectorDS objects.
         * 
         * @param v VectorDS object.
         * 
         * @return Result of the inner product between the VectorDS objects.
         * 
         * @throw Throws a std::invalid_argument exception if the two VectorDS objects have
         *        different sizes.
         */
        Element operator*(const VectorDS &v) const {
            if (this -> size() != v.size()) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for inner product between vectors of size " << this -> size() << " and " << v.size();
                throw std::invalid_argument(error_log.str());
            }

            Element result = Element();
            
            for (uint16_t i = 0; i < this -> size(); i++)
                result += (this -> M)[i] * v.M[i];

            return result;
        }

        friend std::ostream& operator<<(std::ostream &s, const VectorDS &v) {
            uint16_t n = v.size();

            s << "[";

            if (n > 0) {
                for (uint16_t i = 0; i + 1 < n; i++)
                    s << v.M[i] << ", ";

                s << v.M[n-1];
            }

            s << "]" << std::endl;

            return s;
        }
};

#endif
