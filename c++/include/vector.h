#ifndef VECTOR_H
#define VECTOR_H

#include "matrix.h"

#include <iostream>
#include <sstream>
#include <cstring>


template <typename Element>
class Vector: public Matrix<Element> {

    public:
        Vector(uint16_t size, bool row_vector = false, bool init = false) :
            Matrix<Element>(row_vector ? 1: size, row_vector ? size: 1, init) {} //maybe fix zero size

        Vector(const Matrix<Element> &A) : 
            Matrix<Element>(A) {}

        //Constructor from matrix row
        Vector(const Matrix<Element> &A, uint16_t row) : 
            Matrix<Element>(A, row, row + 1, 0, A.ncolumns()) {}
        
        Vector() :
            Matrix<Element>() {}

        Element operator() (uint16_t i) const {
            if (i >= this -> size()) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << this -> size();
                throw std::invalid_argument(error_log.str());
            }

            return (this -> M)[i];
        }

        Element& operator() (uint16_t i) {
            if (i >= this -> size()) {
                std::ostringstream error_log;
                error_log << "Index " << i << " out of bounds for vector of size " << this -> size();
                throw std::invalid_argument(error_log.str());
            }

            return (this -> M)[i];
        }

        Element operator*(const Vector &v) const {
            
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

        friend std::ostream& operator<<(std::ostream &s, const Vector<Element> &v) {

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
