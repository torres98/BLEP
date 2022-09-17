#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

template <typename Element>
class MatrixDS {
    protected:
        Element* M;
        uint16_t rows, columns;

        bool is_memory_static;
    
    public:
        
        /**
         * Create a MatrixDS object with specified sizes .
         *
         * @param rows number of rows.
         * @param columns number of columns.
         * @param init tells the constructor wheter to fill the matrix with the default 
         *     element or not (false default).
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
         * Create a MatrixDS object using a pointer to a static array. 
         *
         * @param A pointer to static array.
         */
        MatrixDS(Element* A, unsigned int rows, unsigned int columns) {
            this -> rows = rows;
            this -> columns = columns;
            this -> M = A;

            this -> is_memory_static = true;
        }

        /**
         * Create a MatrixDS object from a 2D static array. 
         *
         * @param A 2D static array to be copied.
         */
        template <uint16_t rows, uint16_t columns>
        MatrixDS(const Element (&array)[rows][columns]) {
            this -> rows = rows;
            this -> columns = columns;
            this -> M = new Element[rows * columns];

            this -> is_memory_static = false;

            std::copy(*array, *array + rows * columns, this -> M);
        }

        /**
         * Create a MatrixDS object from another one. 
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

        uint16_t nrows() const {
            return this -> rows;
        }

        uint16_t ncolumns() const {
            return this -> columns;
        }

        uint16_t size() const {
            return this -> rows * this -> columns;
        }

        void set(uint16_t i, uint16_t j, const Element& v) {
            if (i >= this -> rows) {
                std::ostringstream error_log;
                error_log << "Row index " << i << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            }

            if (j >= this -> columns) {
                std::ostringstream error_log;
                error_log << "Column index " << j << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            }

            (this -> M)[i * this -> columns + j] = v;
        }

        bool has_full_rank() {
            MatrixDS M_ref = MatrixDS(*this);

            Element pivot;
            uint16_t pivot_row;

            for (uint16_t k = 0; k < M_ref.rows; k++) {
                // Initialize pivot row index and pivot value
                pivot = M_ref(k, k);
                pivot_row = k;
                
                // look for maximum (absolute) pivot value on column k (after row k)
                for (uint16_t i = k+1; i < M_ref.rows; i++)
                    if (M_ref(i, k) > pivot) //should be abs
                        pivot = M_ref(i, k), pivot_row = i;
        
                // if a principal diagonal element is zero, then the
                // matrix is singular, so it can't have full rank
                if (!M_ref(k, pivot_row))
                    return false;
        
                // Swap the pivot row with current row
                if (pivot_row != k) {
                    std::swap_ranges(
                        this -> M + pivot_row * this -> columns,
                        this -> M + (pivot_row + 1) * this -> columns,
                        this -> M + k * this -> columns
                    );
                }

                for (uint16_t i = k+1; i < M_ref.rows; i++) {
                    // current_row = pivot * current_row - current_row[0] * pivot_row
                    for (uint16_t j = k+1; j < M_ref.columns; j++)
                        M_ref.set(i, j, pivot * M_ref(i, j) - M_ref(i, k) * M_ref(k, j));
        
                    // set to zero the current column value
                    M_ref.set(i, k, Element());
                }

            }
            
            return true;
        }
        
        //getter
        Element operator() (uint16_t i, uint16_t j) const { 
            if (i >= this -> rows) {
                std::ostringstream error_log;
                error_log << "Row index " << i << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            }

            if (j >= this -> columns) {
                std::ostringstream error_log;
                error_log << "Column index " << j << " out of bounds for matrix of size " << this -> rows << "x" << this -> columns;
                throw std::invalid_argument(error_log.str());
            }

            return (this -> M)[i * this -> columns + j];
        }

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
         * Compute the product between two MatrixDS objects.
         * 
         * @param A MatrixDS object for the product.
         * 
         * @return A new MatrixDS object containing the result of the product.
         * 
         * @throws std::invalid_argument Thrown if the number of columns of the first
         *     matrix is different from the number of rows of the second one.
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
         * Copies the content of the given MatrixDS object in the current one.
         * 
         * @param A MatrixDS object to be copied.
         * 
         * @return Reference to the current MatrixDS object.
         */
        MatrixDS& operator=(const MatrixDS &A) {

            if (this -> rows != A.rows || this -> columns != A.columns) {
                if (!this -> is_memory_static)
                    delete[] this -> M;
                else
                    this -> is_memory_static = false;

                this -> rows = A.rows;
                this -> columns = A.columns;
                this -> M = new Element[this -> size()];
            }

            std::copy(A.M, A.M + rows * columns, this -> M);

            return *this;

        }

        operator bool() {
            return std::any_of(this -> M, this -> M + this -> size(), [](Element x) { return bool(x); });
        }

        /**
         * Check that the two MatrixDS object have the same size and content.
         * 
         * @param A MatrixDS object to compare 
         * 
         * @return true if the two MatrixDS objects have the same size and content, false
         *     otherwise.
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

template <typename Element>
class VectorDS: public MatrixDS<Element> {

    public:
        VectorDS(uint16_t size, bool row_vector = false, bool init = false) :
            MatrixDS<Element>(row_vector ? 1: size, row_vector ? size: 1, init) {} //tip: fix zero size

        VectorDS(const MatrixDS<Element> &A) : 
            MatrixDS<Element>(A) {}

        //Constructor from matrix row
        VectorDS(const MatrixDS<Element> &A, uint16_t row) : 
            MatrixDS<Element>(A, row, row + 1, 0, A.ncolumns()) {}
        
        VectorDS() :
            MatrixDS<Element>() {}

        void set(uint16_t i, const Element& v) {
            if (i >= this -> size()) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << this -> size();
                throw std::invalid_argument(error_log.str());
            }

            (this -> M)[i] = v;
        }

        static bool check_solution(const MatrixDS<Element> &A, const VectorDS &x, const VectorDS &b) {

            if (A.ncolumns() != x.size()) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes (matrix of size " << A.nrows() << "x" << A.ncolumns() << " and solution vector of size " << x.size() << ")";
                throw std::invalid_argument(error_log.str());
            }

            if (A.nrows() != b.size()) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes (matrix of size " << A.nrows() << "x" << A.ncolumns() << " and result vector of size " << b.size() << ")";
                throw std::invalid_argument(error_log.str());
            }

            for (uint16_t i = 0; i < A.nrows(); i++) {
                if (x.row_vector_product(A, i) != b(i))
                    return false;
            }

            return true;
        }

        Element row_vector_product(const MatrixDS<Element> &A, uint16_t row_index) const {
            uint16_t vector_size = this -> rows;

            if (A.ncolumns() != vector_size) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for matrix row-vector product between matrix of size " << A.rows << "x" << A.columns << " and result vector of size " << this -> rows << ")";
                throw std::invalid_argument(error_log.str());
            }

            Element result = Element();
            uint32_t row_offset = row_index * A.columns;
            
            for (uint16_t j = 0; j < vector_size; j++)            
                result += A.M[row_offset + j] * this -> M[j];

            return result;
        }

        Element operator() (uint16_t i) const {
            if (i >= this -> size()) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << this -> size();
                throw std::invalid_argument(error_log.str());
            }

            return (this -> M)[i];
        }

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
