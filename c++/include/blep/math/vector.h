#ifndef VECTOR_H
#define VECTOR_H

#include "blep/math/matrix.h"

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
        
        /**
         * Return the result of the inner product between the VectorDS object and the specified
         * row of the MatrixDS object.
         * 
         * @param M MatrixDS object.
         * @param i Row index.
         * 
         * @return Result of the inner product.
         * 
         * @throw Throws a std::invalid_argument exception if one of the following conditions is met: 
         *            - the row index is greater or equal than the number of rows of the MatrixDS
         *              object
         *            - the number of columns of the MatrixDS object is different from the size of
         *              the VectorDS object
         */
        Element row_vector_product(const MatrixDS<Element> &A, uint16_t i) const {
            uint16_t vector_size = this -> size();

            if (i >= A.nrows()) {
                std::ostringstream error_log;
                error_log << "Row index " << i << " out of bounds for matrix of size " << A.nrows() << "x" << A.ncolumns();
                throw std::invalid_argument(error_log.str());
            } else if (A.ncolumns() != vector_size) {
                std::ostringstream error_log;
                error_log << "Incompatible sizes for matrix row-vector product between matrix of size " << A.nrows() << "x" << A.ncolumns() << " and result vector of size " << vector_size << ")";
                throw std::invalid_argument(error_log.str());
            } 

            Element result = Element();
            uint32_t row_offset = i * A.ncolumns();
            
            for (uint16_t j = 0; j < vector_size; j++) {
                result += A[row_offset + j] * this -> M[j];
            }

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
