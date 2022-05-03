#ifndef _MATRIX
#define _MATRIX

#include <iostream>
#include <sstream>
#include <cstring>

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

            memcpy(this -> M, A, rows * columns * sizeof(Element));

        }

        // Copy constructor
        Matrix(const Matrix& A) {

            rows = A.rows;
            columns = A.columns;
            M = new Element[rows * columns];

            memcpy(this -> M, A.M, rows * columns * sizeof(Element));

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

            if (columns == A.columns)
                memcpy(this -> M, A[row_i], rows * columns * sizeof(Element));
            else
                for (unsigned int i = row_i; i < row_f; i++)
                    memcpy((*this)[i-row_i], &(A[i][col_i]), columns * sizeof(Element));

        }

        // Default constructor
        Matrix() {
            M = nullptr;
            this -> rows = 0;
            this -> columns = 0;
        }

        unsigned int nrows() const {
            return rows;
        }

        unsigned int ncolumns() const {
            return columns;
        }

        //Seems like i won't be needing it
        /*Element* operator[](unsigned int i) {
            return M + i*columns;
        }*/

        Element* operator[](unsigned int i) const {
            return M + i*columns;
        }

        Matrix operator*(const Matrix &A) {
            
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

            memcpy(this -> M, A.M, rows * columns * sizeof(Element));

            return *this;

        }

        bool operator==(const Matrix &A) {
            if (rows != A.rows || columns != A.columns)
                return false;

            return memcmp(this -> M, A.M, rows * columns * sizeof(Element)) == 0;
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