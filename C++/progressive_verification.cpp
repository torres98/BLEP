#include "matrix.cpp"
#include "vector.cpp"
#include "utils.cpp"
#include "standard_verification.cpp"

template <typename Element>
bool progVer(Matrix<Element> &M, Vector<Element> &v, unsigned int t) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (matrix of size " << M.nrows() << "x" << M.ncolumns() << " and vector of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t > M.nrows()) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of M (" << M.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    Vector<Element> M_row = Vector<Element>(M.ncolumns());
    
    for (unsigned int i = 0; i < t; i++) {
        M_row = Vector<Element>(M, i);

        if (!verify_signature(M_row, v))
            return false;
    }

    return true;
}

template <typename Element>
bool progVerRand(Matrix<Element> &M, Vector<Element> &v, unsigned int t) {

    if (M.ncolumns() != v.nrows()) {
        std::ostringstream error_log;
        error_log << "Incompatible sizes for progressive signature verification (matrix of size " << M.nrows() << "x" << M.ncolumns() << " and vector of size " << (v.nrows() == 1 ? v.ncolumns(): v.nrows()) << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t > M.nrows()) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of M (" << M.nrows() << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    unsigned int *row_indices = new unsigned int[M.nrows()]; //tip: maybe make it different?

    for (unsigned int i = 0; i < M.nrows(); i++)
        row_indices[i] = i;
    
    shuffle_array(row_indices, M.nrows());

    Vector<Element> M_row = Vector<Element>(M.ncolumns());
    
    for (unsigned int i = 0; i < t; i++) {
        M_row = Vector<Element>(M, row_indices[i]);

        if (!verify_signature(M_row, v))
            return false;
    }

    delete[] row_indices;

    return true;
}

/*template <typename Element>
bool progVerRand(Matrix<Element> &M, Matrix<Element> &v, unsigned int t) {

    if (t == 0)
        return false;

    unsigned int row_indices[M.rows];

    for (unsigned int i = 0; i < M.rows; i++)
        row_indices[i] = i;
    
    shuffle_array(row_indices, M.rows);

    Matrix<Element> M_row;
    M_row.rows = 1;
    M_row.columns = M.columns;
    //cout << "OBJ: " << &M_row << endl;

    for (unsigned int i = 0; i < t; i++) {
        Matrix<Element>::get_row(M_row, M, row_indices[i]);
        
        if (!verify_signature(M_row, v)) {
            M_row.M = nullptr;
            return false;
        }
            
    }

    M_row.M = nullptr;
    return true;
}*/