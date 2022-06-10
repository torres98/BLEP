#ifndef MATRIX_H
#define MATRIX_H

#include <cstring>
#include <iostream>
#include <sstream>

template <typename Element>
class Matrix {
    protected:
        Element* M;
        unsigned int rows, columns;

    public:
        
        Matrix(unsigned int rows, unsigned int columns, bool init = false) {

            this -> rows = rows;
            this -> columns = columns;

            if (init)
                M = new Element[rows * columns]();
            else
                M = new Element[rows * columns];

        }

        //Constructor from static 2D array
        template <unsigned int rows, unsigned int columns>
        Matrix(const Element (&A)[rows][columns]) {

            this -> rows = rows;
            this -> columns = columns;
            M = new Element[rows * columns];

            memcpy(this -> M, A, rows * columns * sizeof(Element)); //FIX: could raise problems with objects

        }

        // Copy constructor
        Matrix(const Matrix& A) {

            rows = A.rows;
            columns = A.columns;
            M = new Element[rows * columns];

            memcpy(this -> M, A.M, rows * columns * sizeof(Element)); //FIX: could raise problems with objects

        }

        // Slice constructor
        Matrix(const Matrix& A, unsigned int row_i, unsigned int row_f, unsigned int col_i, unsigned int col_f) {

            if (row_i >= A.rows || row_f > A.rows) {
                std::stringstream error_log;
                error_log << "Row indices [" << row_i << ":" << row_f << "] out of bounds for matrix of size " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            } else if (col_i >= A.columns || col_f > A.columns) {
                std::ostringstream error_log;
                error_log << "Column indices [" << col_i << ":" << col_f << "] out of bounds for matrix of size " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            rows = row_f - row_i;
            columns = col_f - col_i;
            M = new Element[rows * columns];

            if (columns == A.columns) {
                memcpy(this -> M, A[row_i], rows * columns * sizeof(Element)); //FIX: could raise problems with objects
            } else {
                for (unsigned int i = row_i; i < row_f; i++)
                    memcpy((*this)[i-row_i], &(A[i][col_i]), columns * sizeof(Element)); //FIX: could raise problems with objects
            }

        }

        // Default constructor
        Matrix() {
            this -> rows = 0;
            this -> columns = 0;
            M = nullptr;
        }

        unsigned int nrows() const {
            return rows;
        }

        unsigned int ncolumns() const {
            return columns;
        }

        static void swap_row(Matrix &M, unsigned int row_1, unsigned int row_2) {
            Element tmp;

            for (unsigned int j = 0; j < M.columns; j++) {
                tmp = M[row_1][j];

                M[row_1][j] = M[row_2][j];
                M[row_2][j] = tmp;
            }
        }

        bool has_full_rank() {

            Matrix M_ref = Matrix(*this);

            Element pivot;
            unsigned int pivot_row;

            for (unsigned int k = 0; k < rows; k++) {
                // Initialize pivot row index and pivot value
                pivot = M_ref[k][k];
                pivot_row = k;
                
                // look for maximum (absolute) pivot value on column k (after row k)
                for (unsigned int i = k+1; i < rows; i++)
                    if (M_ref[i][k] > pivot) //should be abs
                        pivot = M_ref[i][k], pivot_row = i;
        
                // if a principal diagonal element is zero, then the
                // matrix is singular, so it can't have full rank
                if (!M_ref[k][pivot_row])
                    return false;
        
                // Swap the pivot row with current row
                if (pivot_row != k)
                    swap_row(M_ref, k, pivot_row);

                for (unsigned int i = k+1; i < rows; i++) {
                    // current_row = pivot * current_row - current_row[0] * pivot_row
                    for (unsigned int j = k+1; j < columns; j++)
                        M_ref[i][j] = pivot * M_ref[i][j] - M_ref[i][k] * M_ref[k][j];
        
                    // set to zero the current column value
                    M_ref[i][k] = Element();
                }

            }

            std::cout << M_ref;
            return true;
        }

        /*bool has_full_rank_v2() {

            Matrix M_ref = Matrix(*this);

            Element pivot, f;
            unsigned int pivot_row;

            unsigned int row_indices = new unsigned int[M_ref.rows];
            
            for (unsigned int i = 0; i < M_ref.rows; i++)
                row_indices[i] = i;

            for (unsigned int k = 0; k < rows; k++) {
                // Initialize maximum value and index for pivot
                pivot_row = row_indices[k];
                pivot = M_ref[pivot_row][pivot_row];

                // look for maximum (absolute) pivot value on column k (after row k)
                for (unsigned int i = k+1; i < rows; i++)
                    if (M_ref[i][k] > pivot) //should be abs
                        pivot = M_ref[i][k], pivot_row = i;
        
                // if a principal diagonal element is zero, then the matrix is
                // singular, so it can't have full rank
                if (!M_ref[k][pivot_row])
                    return false;
        
                // Swap the pivot row with current row
                if (pivot_row != k)
                    swap_row(M_ref, k, pivot_row);

                for (unsigned int i = k+1; i < rows; i++) {
                    // factor f to set current row kth element to 0,
                    // and subsequently remaining kth column to 0
                    f = M_ref[i][k] / pivot; //works only if division doesn't produce a different type
        
                    // subtract fth multiple of corresponding kth row element
                    for (unsigned int j = k+1; j < columns; j++)
                        M_ref[i][j] = M_ref[i][j] - M_ref[k][j] * f;
        
                    // filling lower triangular matrix with zeros
                    M_ref[i][k] = Element();
                }

            }

            std::cout << M_ref;
            return true;
        }*/

        //Seems like i won't be needing it
        /*Element* operator[](unsigned int i) {
            return M + i*columns;
        }*/

        Element* operator[](unsigned int i) const {
            return M + i*columns;
        }

        Matrix operator*(const Matrix &A) const {
            
            if (columns != A.rows) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for product between matrices of size " << rows << "x" << columns << " and " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            Matrix O = Matrix(rows, A.columns, true);
            
            for (unsigned int i = 0; i < rows; i++)
                for (unsigned int j = 0; j < A.columns; j++)
                    for (unsigned int h = 0; h < columns; h++)
                        O[i][j] += (*this)[i][h] * A[h][j];

            return O;
        }

        Matrix& operator=(const Matrix &A) {

            if (rows != A.rows || columns != A.columns) {
                delete[] M;

                rows = A.rows;
                columns = A.columns;
                M = new Element[rows * columns];
            }

            memcpy(this -> M, A.M, rows * columns * sizeof(Element)); //FIX: could raise problems with objects

            return *this;

        }

        bool operator==(const Matrix &A) const {
            if (rows != A.rows || columns != A.columns)
                return false;

            return memcmp(this -> M, A.M, rows * columns * sizeof(Element)) == 0; //FIX: could raise problems with objects
        }

        ~Matrix() {
            delete[] M;
        }

};

template <typename Element>
std::ostream& operator<<(std::ostream &s, const Matrix<Element> &A) {

    unsigned int n = A.nrows(), m = A.ncolumns();

    if (n == 0 || m == 0) {
        s << "[]";
    } else {

        for (unsigned int i = 0; i < n; i++) {
            s << "[";

            for (unsigned int j = 0; j < m-1; j++)
                s << A[i][j] << ", ";

            s << A[i][m-1] << "]" << std::endl;
        }

    }

    return s;
}

#endif
