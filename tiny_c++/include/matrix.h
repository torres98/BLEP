#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <sstream>
#include <cstring>

template <typename Element>
class Matrix {
    protected:
        Element* M;
        unsigned int rows, columns;

    public:
        
        /**
         * Create a Matrix object with specified sizes .
         *
         * @param rows number of rows.
         * @param columns number of columns.
         * @param init tells the constructor wheter to fill the matrix with the default 
         *     element or not (false default).
         */
        Matrix(unsigned int rows, unsigned int columns, bool init = false) {

            this -> rows = rows;
            this -> columns = columns;
            
            if (init)
                M = new Element[rows * columns]();
            else
                M = new Element[rows * columns];
        }

        /**
         * Create a Matrix object using a pointer to a static array. 
         *
         * @param A pointer to static array.
         */
        Matrix(Element* A, unsigned int rows, unsigned int columns) {
            this -> rows = rows;
            this -> columns = columns;
            M = A;
        }

        /**
         * Create a Matrix object from a 2D static array. 
         *
         * @param A 2D static array to be copied.
         */
        template <unsigned int rows, unsigned int columns>
        Matrix(const Element (&A)[rows][columns]) {

            this -> rows = rows;
            this -> columns = columns;
            M = new Element[rows * columns];

            memcpy(this -> M, A, rows * columns * sizeof(Element));

        }

        /**
         * Create a Matrix object from another one. 
         *
         * @param A Matrix object to be copied.
         */
        Matrix(const Matrix& A) {

            this -> rows = A.rows;
            this -> columns = A.columns;
            this -> M = new Element[rows * columns];

            memcpy(this -> M, A.M, rows * columns * sizeof(Element));

        }

        /**
         * Create a Matrix object by slicing another one. 
         *
         * @param A Matrix object to be sliced.
         * @param row_i row index to start slicing.
         * @param row_f row index to finish slicing (excluded).
         * @param col_i column index to start slicing.
         * @param col_f column index to finish slicing (excluded).
         * 
         * @throws std::invalid_argument Thrown if any of the given indices exceed
         *     the matrix sizes.
         */
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

            this -> rows = row_f - row_i;
            this -> columns = col_f - col_i;
            this -> M = new Element[rows * columns];

            if (columns == A.columns)
                memcpy(this -> M, A[row_i], rows * columns * sizeof(Element));
            else
                for (unsigned int i = row_i; i < row_f; i++)
                    memcpy((*this)[i-row_i], &(A[i][col_i]), columns * sizeof(Element));

        }

        /**
         * Create an empty Matrix object. 
         */
        Matrix() {
            this -> rows = 0;
            this -> columns = 0;
            this -> M = nullptr;
        }

        unsigned int nrows() const {
            return rows;
        }

        unsigned int ncolumns() const {
            return columns;
        }
        
        /*static void get_row(Matrix<Element> &dest, Matrix<Element> &src, unsigned int row_index) {
            dest.M = src.M + row_index * src.columns;
        }*/

        //TIP: i should find a way to set only that element...
        Element* operator[](unsigned int i) const { //this const is really bad
            return M + i*columns;
        }

        /**
         * Compute the product between two Matrix objects.
         * 
         * @param A Matrix object for the product.
         * 
         * @return A new Matrix object containing the result of the product.
         * 
         * @throws std::invalid_argument Thrown if the number of columns of the first
         *     matrix is different from the number of rows of the second one.
         */
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

        /**
         * Copies the content of the given Matrix object in the current one.
         * 
         * @param A Matrix object to be copied.
         * 
         * @return Reference to the current Matrix object.
         */
        Matrix& operator=(const Matrix &A) {

            if (rows != A.rows || columns != A.columns) {
                delete[] M; // the object is still in scope, so the destructor won't be called

                this -> rows = A.rows;
                this -> columns = A.columns;
                this -> M = new Element[rows * columns];
            }

            memcpy(this -> M, A.M, rows * columns * sizeof(Element));

            return *this;

        }

        /**
         * Check that the two Matrix object have the same size and content.
         * 
         * @param A Matrix object to compare 
         * 
         * @return true if the two Matrix objects have the same size and content, false
         *     otherwise.
         */
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

    if (A.nrows() == 0 || A.ncolumns() == 0) {
        s << "[]";
    } else {

        for (unsigned int i = 0; i < A.nrows(); i++) {
            s << "[";

            for (unsigned int j = 0; j < A.ncolumns() - 1; j++)
                s << A[i][j] << ", ";

            s << A[i][A.ncolumns() - 1] << "]" << std::endl;
        }

    }

    return s;
}

#endif