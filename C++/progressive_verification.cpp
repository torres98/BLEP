#include "matrix.cpp"
#include "utils.cpp"
#include "standard_verification.cpp"

// make it step by step bro
template <typename Element>
bool progVer(Matrix<Element> &M, Vector<Element> &v, unsigned int t) {

    if (t == 0)
        return false;
    
    Matrix<Element> M_reduced = Matrix<Element>(M, 0, t, 0, M.columns);;

    return (M_reduced * v).is_zero();
}

template <typename Element>
bool progVerRand(Matrix<Element> &M, Vector<Element> &v, unsigned int t) {

    if (t > M.rows) {
        std::stringstream error_log;
        error_log << "Number of steps t (" << t << ") can't be bigger than the number of rows of M (" << M.rows << ")";
        throw std::invalid_argument(error_log.str());
    }

    if (t == 0)
        return false;

    unsigned int *row_indices = new unsigned int[M.rows]; //tip: maybe make it different?

    for (unsigned int i = 0; i < M.rows; i++)
        row_indices[i] = i;
    
    shuffle_array(row_indices, M.rows);

    Vector<Element> M_row = Vector<Element>(M.columns);
    
    for (unsigned int i = 0; i < t; i++) {
        M_row = Vector<Element>(M, row_indices[i]);
        
        if (M_row * v)
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