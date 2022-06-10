#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <sstream>
#include <cstring>

#include "matrix.h"

template <typename Element>
class Vector: public Matrix<Element> {

    public:
        Vector(unsigned int size, bool row_vector = false, bool init = false) :
            Matrix<Element>(row_vector ? 1: size, row_vector ? size: 1, init) {}

        Vector(const Matrix<Element> &A) : Matrix<Element>(A) {}
        
        //Constructor from matrix row
        Vector(const Matrix<Element> &A, unsigned int row) : Matrix<Element>(1, A.ncolumns()) {
            if (row >= A.nrows()) {
                std::stringstream error_log;
                error_log << "Row index " << row << " out of bounds for matrix of size " << A.nrows() << "x" << A.ncolumns() << ".";
                throw std::invalid_argument(error_log.str());
            }

            memcpy(this -> M, A[row], A.ncolumns() * sizeof(Element));
        }
        
        Vector() :
            Matrix<Element>() {}

        Element operator[] (unsigned int i) const {
            if (i >= std::max(this -> rows, this -> columns)) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << std::max(this -> rows, this -> columns) << ".";
                throw std::invalid_argument(error_log.str());
            }

            return this -> M[i];
        }

        Element& operator[] (unsigned int i) {
            if (i >= std::max(this -> rows, this -> columns)) {
                std::ostringstream error_log;
                //error_log << "Index " << i << " out of bounds for vector of size " << std::max(this -> rows, this -> columns) << ".";
                throw std::invalid_argument("error_log.str()");
            }

            return this -> M[i];
        }

        Element operator*(const Vector &v) {
            
            if (this -> rows != v.rows) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for product between vectors of size " << std::max(this -> rows, this -> columns) << " and " << max(v.rows, v.columns) << ".";
                throw std::invalid_argument(error_log.str());
            }

            Element result = Element();
            
            for (unsigned int i = 0; i < std::max(this -> rows, this -> columns); i++)
                result += (*this)[i] * v[i];

            return result;
        }

};

template <typename Element>
std::ostream& operator<<(std::ostream &s, const Vector<Element> &v) {

    s << "[";

    if (v.nrows() > 0) {
        for (unsigned int i = 0; i < v.nrows() - 1; i++)
            s << v[i] << ", ";

        s << v[v.nrows() - 1];
    }

    s << "]" << std::endl;

    return s;

}

#endif