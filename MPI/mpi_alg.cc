#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cmath>
#include <mpi.h>

#define ld long double
#define ll unsigned long long
#define mat ld*
#define matRow ld*

using namespace std;
const double precision = 0.0001;

int size;

void swapRows (mat matrix, int i, int j) {
    for (int k = 0; k < size; ++k) {
        swap (matrix[size*i + k], matrix[size*j + k]);
    }

}

void swapColumns (mat matrix, int i, int j) {
    for (int k = 0; k < size; ++k) {
        swap (matrix[size*k + i], matrix[size*k + j]);
    }
}


bool isNull (matRow vec) {
    for (int i = 0; i < size; ++i) {
        if (abs(vec[i]) > precision) {
            return false;
        }
    }
    return true;
}

bool nullRowFound (mat matrix, int k) {
    for (int j = k+1; j < size; ++j){
        if (abs(matrix[size * k + j]) > precision) {
            swapColumns(matrix, k, j);
            return false;
        }
    }
    return true;
}

void propagateDown (mat matrix, int k, int& nullRows) {
    int i = size - 1 - nullRows;
    while (isNull(&matrix[size * i])) {
        ++nullRows;
        --i;
    }
    swapRows (matrix, k, i);
}

void toTrapezeMPI (mat matrix, mat trp, int rank, int nProcs, int& nullRows) {
    MPI_Bcast (matrix, size*size, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
    for (int k = 0; k < size - 1 - nullRows; ++k) {
        /*if (nullRowFound(matrix, k)) {
            if (!rank) {
                propagateDown (matrix, k, nullRows);
            }
            --k;
            MPI_Bcast (matrix, size*size, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
            continue;
        }
        MPI_Barrier (MPI_COMM_WORLD);*/

        for (int i = 0; i < size*size; ++i) {
            trp[i] = 0;
        }

        for (int j = k+1; j < size; ++j) {
            ld tmp = matrix[size*j + k - nullRows] / matrix[size*k + k - nullRows];
            for (int i = 0; i < size/nProcs; ++i) {
                trp[size*j + i*size/nProcs+rank - nullRows] -= tmp * matrix[size*k + i*size/nProcs+rank - nullRows];
            }
        }
        MPI_Allreduce (trp, matrix, size*size, MPI_LONG_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    }
}


void fillMatrix (mat matrix, int size) {
    srand(time(NULL));
    for (int i = 0; i < size*size; ++i) {
        matrix[i] = rand();
    }
}

int main (int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, nProcs;
    MPI_Comm_size(MPI_COMM_WORLD, &nProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    sscanf(argv[1], "%i", &size);
    //To make testing easier, assume that matrix is N*N;
    //But the algorithm can be easily modified for non-square matrix

    mat matrix = new ld [size * size];
    mat trp = new ld [size * size];
    double time = 0;
    int nullRows = 0;
    for (int i = 0; i < 3; i++) {
        if (!rank) {
            fillMatrix (matrix, size);
        }
        MPI_Barrier (MPI_COMM_WORLD);
        double loc_time = MPI_Wtime ();
        toTrapezeMPI (matrix, trp, rank, nProcs, nullRows);
        MPI_Barrier (MPI_COMM_WORLD);
        time += MPI_Wtime () - loc_time;
        if (!rank) {
            cout << "Rank is " << size - nullRows << endl;
        }
    }
    delete[] matrix;
    delete[] trp;
    time /= 3;
    if (!rank) {
        cout << "Time: " << fixed << setprecision(8) << time << endl;
    }
    MPI_Finalize();
    return 0;
}
