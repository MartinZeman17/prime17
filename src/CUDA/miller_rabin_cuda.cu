/**
 * @file miller_rabin_cuda.cu
 *
 * @brief This module provides functions for performing the deterministic Miller-Rabin primality test.
 *        This version of the test is deterministic all unsigned 64 bit integers.
 *        The test is accelerated on a GPU through CUDA.
 *        Run the test on a wide rangle of numbers for maximizing perfoormance.
 */

#include <iostream>
#include <limits.h>
#include <assert.h>
#include <chrono>


/**
 * @brief Performs the square-and-multiply algorithm to calculate a**b % m.
 *
 * This function is executed on the GPU device.
 * This function works properly for all unsigned 64-bit integers - it uses 128-bit integers to prevent overflow.
 *
 * @param a The base value.
 * @param b The exponent value.
 * @param m The modulus value.
 *
 * @return The result of a^b mod m.
 */
__device__ u_int64_t squareAndMultiply(u_int64_t a, u_int64_t b, u_int64_t m) {
    a = a % m;  // Reduce a modulo m to ensure a is within the range [0, m-1]
    u_int64_t result = 1;

    while (b > 0) {
        if (b & 1) {
            // Multiply result with a, and then reduce modulo m
            result = ((__uint128_t)result * (__uint128_t)a) % m;
        }
        // Square a, and then reduce modulo m
        a = ((__uint128_t)a * (__uint128_t)a) % m;
        // Right shift b by 1 (equivalent to dividing by 2)
        b >>= 1;
        
    }

    return result;
}




/**
 * @brief Performs the Miller-Rabin primality test to check if a number is probably prime.
 *
 * This test is deterministic for all unsigned 64-bit integers.
 * This function is executed on the GPU device.
 *
 * @param n The number to be tested for primality.
 *
 * @return true if n is probably prime, false if n is composite.
 */
__device__ bool millerRabin(u_int64_t n) {

    // Predefined bases for uint64_t
    u_int8_t BASE[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

    // Check if n is a small prime number
    if (n <= 41) {
        if (n <= 1) {
            return false;
        }
        for (int i = 0; i < 13; i++) {
            if (n == BASE[i]) {
                return true;
            }
        }
    }

    // Check if n is divisible by 2
    if (n % 2 == 0) {
        return false;
    }

    // Compute n - 1 = 2^s * d
    u_int64_t d = n - 1;
    u_int64_t s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        s++;
    }


    // Perform the Miller-Rabin test with the predefined bases
    for (int i = 0; i < 13; i++) {
        u_int64_t a = BASE[i];
        u_int64_t x = squareAndMultiply(a, d, n);

        if (x == 1 || x == n - 1) {
            // Continue to the next base if x is a probable witness
            continue;
        }

        bool continueLoop = false;

        for (int j = 0; j < s - 1; j++) {
            x = squareAndMultiply(x, 2, n);

            if (x == 1) {
                // n is composite, return false
                return false;
            }

            if (x == n - 1) {
                // x is a probable witness, continue to the next base
                continueLoop = true;
                break;
            }
        }

        if (continueLoop) {
            continue;
        }

        // x is not a probable witness, n is composite
        return false;
    }

    // n passed the Miller-Rabin test for all bases, it is probably prime
    return true;
}


/**
 * @brief CUDA kernel function for parallel prime testing using the Miller-Rabin algorithm.
 *
 * This function is executed on the GPU device.
 *
 * @param boolArray A pointer to the boolean array indicating whether each number is prime.
 * @param primes A pointer to the array of numbers to be tested for primality.
 * @param size The size of the array.
 */
__global__ void kernel(bool* boolArray, const u_int64_t* primes, const u_int64_t size) {
    // Calculate the global thread ID
    u_int64_t tid = blockIdx.x * blockDim.x + threadIdx.x;

    // Check if the thread ID is within the valid range
    if (tid < size) {
        // Test the primality of the number at the current thread ID
        boolArray[tid] = millerRabin(primes[tid]);
    }
}



/**
 * @brief Tests whether each number in the given array of numbers is prime.
 *
 * This function utilizes CUDA to parallelize the prime testing on the GPU.
 *
 * @param numbers A pointer to an array of numbers to be tested for primality.
 * @param size The size of the array.
 *
 * @return A pointer to a bool array indicating whether each number is prime.
 *         The caller is responsible for freeing the memory allocated for the bool array.
 */
bool* testPrimes(u_int64_t* numbers, size_t size) {
    u_int64_t* d_numbers;   // Device memory pointer for numbers
    bool* d_boolArray;      // Device memory pointer for bool array

    // Allocate device memory for numbers and bools
    cudaMalloc((void**)&d_numbers, sizeof(u_int64_t) * size);
    cudaMalloc((void**)&d_boolArray, sizeof(bool) * size);

    // Copy the numbers from host to device memory
    cudaMemcpy(d_numbers, numbers, sizeof(u_int64_t) * size, cudaMemcpyHostToDevice);

    // Allocate host memory for the resulting bool array
    bool* boolArray = (bool*)malloc(sizeof(bool) * size);

    // Set the block size for CUDA kernel execution and calculate the grid size
    constexpr u_int64_t BLOCKSIZE = 256;
    u_int64_t gridSize = (size + BLOCKSIZE - 1) / BLOCKSIZE;

    // Launch the CUDA kernel to test the primality of numbers
    kernel<<<gridSize, BLOCKSIZE>>>(d_boolArray, d_numbers, size);

    // Copy the bool array from device to host memory
    cudaMemcpy(boolArray, d_boolArray, sizeof(bool) * size, cudaMemcpyDeviceToHost);

    // Free the allocated device memory
    cudaFree(d_numbers);
    cudaFree(d_boolArray);

    return boolArray;
}

int test_from_file(const char* filename, bool correctResultValue)
{
    // load the test data
    std::cout << "loading testing data..." << std::endl;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    u_int64_t *primes = NULL;
    u_int64_t number;
    size_t size = 0;
    while (fscanf(file, "%llu\n", &number) == 1) {
        primes = (u_int64_t *)realloc(primes, (size + 1) * sizeof(u_int64_t));
        primes[size++] = number;
    }
    fclose(file);
    std::cout << "about to test " << size << " numbers" << std::endl;

    // time function and print results
    auto start = std::chrono::high_resolution_clock::now();
    bool* result = testPrimes(primes, size);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // results
    printf("testing took: %.3f sesonds\n\n", duration / 1000);

    // test correctness
    for (int i = 0; i < size; i++) {
        assert (result[i] == correctResultValue);
    }

    // free memory
    free(result);
    free(primes);

    return 0;
}



int main() {

    // load the test data
    std::cout << "note - first warmup run is expected to be longer" << std::endl;
    std::cout << "testing over non primes" << std::endl;
    test_from_file("./data/non_primes.csv", false);

    std::cout << "testing over pseudo primes" << std::endl;
    test_from_file("./data/pseudo_primes.csv", false);

    std::cout << "testing over edge cases" << std::endl;
    test_from_file("./data/edge_cases_false.csv", false);
    test_from_file("./data/edge_cases_true.csv", true);

    std::cout << "testing over sieve of erathostenes for all unsigned numbers < 2**20" << std::endl;
    test_from_file("./data/primes_2**20.csv", true);

    std::cout << "testing over primes ~ 2**43" << std::endl;
    test_from_file("./data/primes_2**43.csv", true);

    std::cout << "testing over primes ~ 2**63" << std::endl;
    test_from_file("./data/primes_2**63.csv", true);

    return 0;
}
