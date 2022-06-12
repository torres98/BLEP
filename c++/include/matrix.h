#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

template <typename Element>
class Matrix {
    protected:
        Element* M;
        uint16_t rows, columns;

    public:
        
        Matrix(uint16_t rows, uint16_t columns, bool init = false) {
            this -> rows = rows;
            this -> columns = columns;

            if (init)
                this -> M = new Element[rows * columns]();
            else
                this -> M = new Element[rows * columns];
        }

        //Constructor from static 2D array
        template <uint16_t rows, uint16_t columns>
        Matrix(const Element (&array)[rows][columns]) {
            this -> rows = rows;
            this -> columns = columns;
            this -> M = new Element[rows * columns];

            std::copy(*array, *array + rows * columns, this -> M);
        }

        // Copy constructor
        Matrix(const Matrix& A) {
            this -> rows = A.rows;
            this -> columns = A.columns;
            this -> M = new Element[rows * columns];

            std::copy(A.M, A.M + rows * columns, this -> M);
        }

        // Slice constructor
        Matrix(const Matrix& A, uint16_t row_i, uint16_t row_f, uint16_t col_i, uint16_t col_f) {

            if (row_i >= A.rows || row_f > A.rows) {
                std::stringstream error_log;
                error_log << "Row indices [" << row_i << ":" << row_f << "] out of bounds for matrix of size " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            } else if (col_i >= A.columns || col_f > A.columns) {
                std::ostringstream error_log;
                error_log << "Column indices [" << col_i << ":" << col_f << "] out of bounds for matrix of size " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            this -> rows = row_f - row_i;
            this -> columns = col_f - col_i;
            this -> M = new Element[rows * columns];

            if (this -> columns == A.columns) {
                std::copy(A.M + row_i * A.columns, A.M + row_f * A.columns, this -> M);
            } else {
                for (uint16_t i = row_i; i < row_f; i++)
                    std::copy(A.M + i * A.columns + col_i, A.M + i * A.columns + col_f, this -> M + (i-row_i) * this -> columns);
            }

        }

        // Default constructor
        Matrix() {
            this -> rows = 0;
            this -> columns = 0;
            this -> M = nullptr;
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

        void swap_row(uint16_t row_1, uint16_t row_2) {
            std::swap_ranges(
                this -> M + row_1 * this -> columns,
                this -> M + (row_1+1) * this -> columns,
                this -> M + row_2 * this -> columns
            );
        }

        bool has_full_rank() {
            Matrix M_ref = Matrix(*this);

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
                if (pivot_row != k)
                    M_ref.swap_row(k, pivot_row);

                for (uint16_t i = k+1; i < M_ref.rows; i++) {
                    // current_row = pivot * current_row - current_row[0] * pivot_row
                    for (uint16_t j = k+1; j < M_ref.columns; j++)
                        M_ref(i, j) = pivot * M_ref(i, j) - M_ref(i, k) * M_ref(k, j);
        
                    // set to zero the current column value
                    M_ref(i, k) = Element();
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

            return (this -> M)[i*columns + j];
        }

        //setter (tofix)
        Element& operator() (uint16_t i, uint16_t j) {
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

            return (this -> M)[i*columns + j];
        }

        Matrix operator*(const Matrix &A) const {
            
            if (this -> columns != A.rows) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for product between matrices of size " << this -> rows << "x" << this -> columns << " and " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            Matrix O = Matrix(this -> rows, A.columns, true);
            
            for (uint16_t i = 0; i < this -> rows; i++)
                for (uint16_t j = 0; j < A.columns; j++)
                    for (uint16_t h = 0; h < this -> columns; h++)
                        O.M[i*O.columns + j] += (this -> M)[i*this -> columns + h] * A.M[h*A.columns + j];

            return O;
        }

        Matrix& operator=(const Matrix &A) {

            if (this -> rows != A.rows || this -> columns != A.columns) {
                delete[] this -> M;

                this -> rows = A.rows;
                this -> columns = A.columns;
                this -> M = new Element[this -> rows * this -> columns];
            }

            std::copy(A.M, A.M + rows * columns, this -> M);

            return *this;

        }

        bool operator==(const Matrix &A) const {
            if (this -> rows != A.rows || this -> columns != A.columns)
                return false;

            return std::equal(this -> M, this -> M + rows * columns, A.M);
        }

        ~Matrix() {
            delete[] this -> M;
        }

        friend std::ostream& operator<<(std::ostream &s, const Matrix<Element> &A) {
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

#endif
