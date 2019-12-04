#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cmath>
#include <omp.h>

using namespace std;
const double precision = 0.00001;
int threads = 1;

void swapRows (vector<vector<long double>>& matrix, int i, int j) {
    swap (matrix[i], matrix[j]);

}

void swapColumns (vector<vector<long double>>& matrix, int i, int j) {
    #pragma omp parallel for shared (matrix) num_threads(threads)
    for (int k = 0; k < matrix.size(); ++k) {
        swap (matrix[k][i], matrix[k][j]);
    }
}


bool isNull (const vector<long double>& vec) {
    for (int i = 0; i < vec.size(); ++i) {
        if (abs(vec[i]) > precision) {
            return false;
        }
    }
    return true;
}

bool nullRowFound (vector<vector<long double>>& matrix, int& k) {
    for (int j = k+1; j < matrix[k].size(); ++j){
        if (abs(matrix[k][j]) > precision) {
            swapColumns(matrix, k, j);
            return false;
        }
    }
    matrix.erase(matrix.begin() + k);
    --k;
    return true;
}

void toTrapeze (vector<vector<long double>>& matrix) {
    for (int k = 0; k < min (matrix.size(), matrix[0].size()) - 1; ++k) {
        if (abs (matrix[k][k]) < precision) {
            if (nullRowFound(matrix, k)) {
                continue;
            }
        }
        #pragma omp parallel for shared (matrix) num_threads(threads)
        for (int j = k+1; j < matrix.size(); ++j) {
            if (omp_get_num_threads() != int(threads)) {
                cerr << omp_get_num_threads() << endl;
            }
            long double tmp = matrix[j][k] / matrix[k][k];
            for (int i = k; i < matrix[j].size(); ++i) {
                matrix[j][i] -= tmp * matrix[k][i];
            }
        }
    }

    if (matrix[0].size() < matrix.size()) {
        matrix.erase(matrix.begin() + matrix[0].size(), matrix.end());
    }
    #pragma omp parallel for
    for (int i = 0; i < matrix.size(); ++i) {
        if (isNull (matrix[i])) {
            matrix.erase(matrix.begin() + i);
            --i;
        }
    }



}

vector<vector<long double>> fillMatrix (int size) {
    srand(time(NULL));
    vector<vector<long double> > matrix (size);
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i].push_back (rand());
        }
    }
    return matrix;
}

int main (int argc, char *argv[]) {
    int size;
    sscanf(argv[1], "%i", &size);
    sscanf(argv[2], "%i", &threads);

    vector<vector<long double>> matrix = fillMatrix (size);
    double time = 0;
    for (int i = 0; i < 3; i++) {
        double loc_time = omp_get_wtime();
        toTrapeze(matrix);
        time += (omp_get_wtime() - loc_time);
    }
    time /= 3;

    cout << "Threads: " << threads << "   Time: " << fixed << setprecision(8) << time << endl;
    return 0;
}
