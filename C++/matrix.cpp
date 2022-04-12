#ifndef _MATRIX
#define _MATRIX

#include <iostream>
#include <sstream>

template <typename Element>
class Matrix {
    public:
        Element* M;
        unsigned int rows, columns;

        Matrix(unsigned int rows, unsigned int columns) {
            M = new Element[rows * columns];
            this -> rows = rows;
            this -> columns = columns;
        }

        // Copy constructor
        Matrix(const Matrix& A) {

            rows = A.rows;
            columns = A.columns;
            M = new Element[rows * columns];

            for (unsigned int i = 0; i < rows; i++)
                for (unsigned int j = 0; j < columns; j++)
                    (*this)[i][j] = A[i][j];

        }

        // Slicing constructor
        Matrix(const Matrix& A, unsigned int row_i, unsigned int row_f, unsigned int column_i, unsigned int column_f) {
            /*std::cout << "Slicing " << this -> rows << "x" << this -> columns << "   " << A.rows << "x" << A.columns << "\n";
            std::cout << this << " " << &A << std::endl;*/

            if (row_i >= A.rows || row_f > A.rows) {
                std::stringstream error_log;
                error_log << "Row indices [" << row_i << ":" << row_f << "] out of bounds for matrix of size " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            } else if (column_i >= A.columns || column_f > A.columns) {
                std::ostringstream error_log;
                error_log << "Column indices [" << column_i << ":" << column_f << "] out of bounds for matrix of size " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            rows = row_f - row_i;
            columns = column_f - column_i;
            M = new Element[rows * columns];

            memcpy(this -> M, &A.M[row_i * A.columns], rows * columns * sizeof(Element));

        }

        // Default constructor
        Matrix() {
            M = nullptr;
            this -> rows = 0;
            this -> columns = 0;
        }

        static void get_row(Matrix<Element> &dest, Matrix<Element> &src, unsigned int row_index) {
            dest.M = src.M + row_index * src.columns;
        }

        bool is_zero() {
            for (unsigned int i = 0; i < rows; i++)
                for (unsigned int j = 0; j < columns; j++)
                    if ((*this)[i][j]) return false;

            return true;
        }

        //Seems like i won't be needing it
        /*Element* operator[](unsigned int i) {
            return M + i*columns;
        }*/

        Element* operator[](unsigned int i) const {
            return M + i*columns;
        }

        Matrix operator*(Matrix &A) {
            
            if (columns != A.rows) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for product between matrices of size " << rows << "x" << columns << " and " << A.rows << "x" << A.columns;
                throw std::invalid_argument(error_log.str());
            }

            Matrix O = Matrix(rows, A.columns);
            
            for (unsigned int i = 0; i < rows; i++)
                for (unsigned int j = 0; j < A.columns; j++)
                    for (unsigned int h = 0; h < columns; h++)
                        O[i][j] += (*this)[i][h] * A[h][j];

            return O;
        }

        Matrix& operator=(const Matrix &A) {
            //std::cout << "Assignment " << rows << "x" << columns << "   " << A.rows << " " << A.columns << "\n";
            //std::cout << this << " " << A.M << std::endl;

            if (rows != A.rows || columns != A.columns) {
                //std::cout << "Deleting M now\n";
                delete[] M; // the object is still in scope, so the destructor won't be called

                rows = A.rows;
                columns = A.columns;
                M = new Element[rows * columns];
            }

            memcpy(this -> M, A.M, rows * columns * sizeof(Element));

            return *this;

        }

        ~Matrix() {
            /*std::cout << "DELETE " << rows << "x" << columns << "\n";
            std::cout << "OBJ POINTER: " << this << std::endl;
            std::cout << "M POINTER: " << this -> M << std::endl << std::endl;*/
            delete[] M;
        }

};

template <typename Element>
std::ostream& operator<<(std::ostream &s, const Matrix<Element> &A) {
    std::string output = "[";
    unsigned int rows, columns;

    if (A.columns == 1) {
        s << "[";

        for (unsigned int i = 0; i < A.rows - 1; i++)
            s << A.M[i] << ", ";

        s << A.M[A.rows - 1];

    } else {

        s << "[" << std::endl;

        for (unsigned int i = 0; i < A.rows; i++) {
            s << "[";

            for (unsigned int j = 0; j < A.columns - 1; j++)
                s << A.M[i*A.columns + j] << ", ";

            s << A.M[(i+1)*A.columns - 1] << "]" << std::endl;
        }
    }

    s << "]";

    return s;
}

// for now it's just a column vector
template <typename Element>
class Vector: public Matrix<Element> {

    public:
        Vector(unsigned int elements) : Matrix<Element>(elements, 1) {}

    Element& operator[](unsigned int i) {
        return this -> M[i];
    }

};

#endif