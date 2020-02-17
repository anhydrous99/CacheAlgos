#include <iostream>
#include <algorithm>
#include <cassert>
#include <chrono>

// Square Matrix Size
#define N 10000

// Transpose Block Size
#define B 16

void naive_transpose(const double *input, double *output, uint64_t n) {
    for (uint64_t i = 0; i < n - 1; i++) {
        for (uint64_t j = 0; j < n; j++)
            output[i * n + j] = input[j * n + i];
    }
}

void cache_transpose(const double *input, double *output, uint64_t n) {
    uint64_t blocksize = B;
    for (uint64_t i = 0; i < n - 1; i += blocksize) {
        for (uint64_t j = 0; j < n; j++) {
            for (uint64_t b = 0; b < blocksize && i + b < n; b++)
                output[j * n + i + b] = input[(i + b) * n + j];
        }
    }
}

int main() {
    // Import high resolution timer
    using namespace std::chrono;
    typedef high_resolution_clock hrc;

    // Allocate matricies of R^{NxN} size as a linear array
    constexpr uint64_t array_size = N * N;
    auto *matrix = new double[array_size];
    auto *naive_matrix = new double[array_size];
    auto *cache_matrix = new double[array_size];

    // Start timer
    auto naive_start_time = hrc::now();
    // Perform naive transpose
    naive_transpose(matrix, naive_matrix, N);
    // Stop timer
    auto naive_end_time = hrc::now();

    // Start timer
    auto cache_start_time = hrc::now();
    // Perform cache-oblivious transpose
    cache_transpose(matrix, cache_matrix, N);
    // Stop timer
    auto cache_end_time = hrc::now();

    std::cout << "Naive time: " << duration_cast<milliseconds>(naive_end_time - naive_start_time).count()
              << " milliseconds\n";
    std::cout << "Cache time: " << duration_cast<milliseconds>(cache_end_time - cache_start_time).count()
              << " milliseconds\n";

    // Check if algorithms are correct
    double sanity[9] = {1, 2, 3,
                        4, 5, 6,
                        7, 8, 9};
    double trans_sanity[9] = {1, 4, 7,
                              2, 5, 8,
                              3, 6, 9};
    auto* naive_sanity = new double[9];
    auto* cache_sanity = new double[9];
    naive_transpose(sanity, naive_sanity, 3);
    cache_transpose(sanity, cache_sanity, 3);
    for (int i = 0; i < 9; i++) {
        assert(trans_sanity[i] == naive_sanity[i]);
        assert(trans_sanity[i] == cache_sanity[i]);
    }

    // always clean-up
    delete [] cache_sanity;
    delete [] naive_sanity;
    delete [] cache_matrix;
    delete [] naive_matrix;
    delete [] matrix;
    return 0;
}
