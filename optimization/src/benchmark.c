#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000

// Helper function to get array length (simulating a real getter)
int get_vec_length(int *vec) {
    return N;
}

// Helper to get element (simulating accessor function)
int get_vec_element(int *vec, int index) {
    return vec[index];
}

/* ============================================
   VERSION 1: BASELINE - No optimizations
   Computes sum of array elements
   ============================================ */
void combine1(int *vec, int *dest) {
    *dest = 0;
    for (int i = 0; i < get_vec_length(vec); i++) {
        *dest = *dest + get_vec_element(vec, i);
    }
}

/* ============================================
   VERSION 2: CODE MOTION
   Move loop-invariant computation outside loop
   ============================================ */
void combine2(int *vec, int *dest) {
    int length = get_vec_length(vec);  // MOVED OUT OF LOOP
    *dest = 0;
    for (int i = 0; i < length; i++) {
        *dest = *dest + get_vec_element(vec, i);
    }
}

/* ============================================
   VERSION 3: REDUCE PROCEDURE CALLS
   Eliminate function call in hot loop
   ============================================ */
void combine3(int *vec, int *dest) {
    int length = get_vec_length(vec);
    *dest = 0;
    for (int i = 0; i < length; i++) {
        *dest = *dest + vec[i];  // Direct access, no function call
    }
}

/* ============================================
   VERSION 4: ELIMINATE MEMORY REFERENCES
   Keep accumulator in a register (local variable)
   THIS IS THE KEY CSAPP OPTIMIZATION
   ============================================ */
void combine4(int *vec, int *dest) {
    int length = N;
    int acc = 0;  // Local variable stays in register!
    for (int i = 0; i < length; i++) {
        acc = acc + vec[i];  // No memory write each iteration
    }
    *dest = acc;  // Write once at the end
}

/* ============================================
   VERSION 5: LOOP UNROLLING (2x)
   Multiple accumulators for ILP
   ============================================ */
void combine5(int *vec, int *dest) {
    int length = N;
    int acc0 = 0;
    int acc1 = 0;
    int i;
    
    // Process 2 elements at a time with separate accumulators
    for (i = 0; i < length - 1; i += 2) {
        acc0 = acc0 + vec[i];
        acc1 = acc1 + vec[i+1];
    }
    
    // Handle leftover element
    for (; i < length; i++) {
        acc0 = acc0 + vec[i];
    }
    
    *dest = acc0 + acc1;
}

/* ============================================
   VERSION 6: LOOP UNROLLING (4x)
   Even more ILP (Instruction Level Parallelism)
   ============================================ */
void combine6(int *vec, int *dest) {
    int length = N;
    int acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    int i;
    
    // Process 4 elements at a time
    for (i = 0; i < length - 3; i += 4) {
        acc0 = acc0 + vec[i];
        acc1 = acc1 + vec[i+1];
        acc2 = acc2 + vec[i+2];
        acc3 = acc3 + vec[i+3];
    }
    
    // Handle leftover elements
    for (; i < length; i++) {
        acc0 = acc0 + vec[i];
    }
    
    *dest = (acc0 + acc1) + (acc2 + acc3);
}

/* ============================================
   VERSION 7: POINTER INCREMENT (bonus)
   Replace array indexing with pointer arithmetic
   ============================================ */
void combine7(int *vec, int *dest) {
    int length = N;
    int acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    int *ptr = vec;
    int *end = vec + length - 3;
    
    // Process 4 elements at a time with pointer increment
    while (ptr < end) {
        acc0 = acc0 + *ptr++;
        acc1 = acc1 + *ptr++;
        acc2 = acc2 + *ptr++;
        acc3 = acc3 + *ptr++;
    }
    
    // Handle leftover
    end = vec + length;
    while (ptr < end) {
        acc0 = acc0 + *ptr++;
    }
    
    *dest = (acc0 + acc1) + (acc2 + acc3);
}

/* ============================================
   TIMING AND PROFILING
   ============================================ */
double time_function(void (*func)(int*, int*), int *vec, int *dest, int iterations) {
    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        func(vec, dest);
    }
    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

int main() {
    // Allocate test vectors
    int *vec = malloc(N * sizeof(int));
    int dest = 0;
    
    // Initialize with some data
    for (int i = 0; i < N; i++) {
        vec[i] = i % 100;  // Keep values small to avoid overflow
    }
    
    int iterations = 1000;
    
    printf("CSAPP Chapter 5 - Code Optimization\n");
    printf("Computing sum of %d elements, %d iterations\n\n", N, iterations);
    
    double t1 = time_function(combine1, vec, &dest, iterations);
    printf("Version 1 (Baseline):           %.4f sec  Result: %d\n", t1, dest);
    
    double t2 = time_function(combine2, vec, &dest, iterations);
    printf("Version 2 (Code Motion):        %.4f sec  (%.2fx speedup)\n", t2, t1/t2);
    
    double t3 = time_function(combine3, vec, &dest, iterations);
    printf("Version 3 (Reduce Calls):       %.4f sec  (%.2fx speedup)\n", t3, t1/t3);
    
    double t4 = time_function(combine4, vec, &dest, iterations);
    printf("Version 4 (Eliminate Mem Ref):  %.4f sec  (%.2fx speedup) *** KEY\n", t4, t1/t4);
    
    double t5 = time_function(combine5, vec, &dest, iterations);
    printf("Version 5 (Loop Unroll 2x):     %.4f sec  (%.2fx speedup)\n", t5, t1/t5);
    
    double t6 = time_function(combine6, vec, &dest, iterations);
    printf("Version 6 (Loop Unroll 4x):     %.4f sec  (%.2fx speedup)\n", t6, t1/t6);
    
    double t7 = time_function(combine7, vec, &dest, iterations);
    printf("Version 7 (Pointer Increment):  %.4f sec  (%.2fx speedup)\n", t7, t1/t7);
    
    printf("\nFinal result: %d\n", dest);
    
    free(vec);
    
    return 0;
}

/* ============================================
   DISCUSSION POINTS FOR CLASS:
   
   1. CODE MOTION (v1 -> v2)
      - get_vec_length() called N times vs once
      - Compiler can't optimize if it thinks function has side effects
      - Simple fix: hoist invariant computations
   
   2. REDUCE PROCEDURE CALLS (v2 -> v3)
      - Function call overhead: push args, call, return, pop
      - get_vec_element() eliminated in hot loop
      - Huge win on simple operations
   
   3. ELIMINATE MEMORY REFERENCES (v3 -> v4) *** BIGGEST WIN ***
      - v3: writes *dest every iteration (LOAD + ADD + STORE)
      - v4: acc in register (just ADD), write once at end
      - This is the classic CSAPP optimization!
      - Memory operations are 50-100x slower than register ops
   
   4. LOOP UNROLLING (v4 -> v5 -> v6)
      - Multiple accumulators break data dependency chain
      - CPU can execute additions in parallel (ILP)
      - Reduces loop overhead (increment, compare, branch)
      - Modern CPUs have ~4-6 functional units
      - Diminishing returns after 4x-8x
   
   5. POINTER INCREMENT (v6 -> v7)
      - Eliminates index multiplication (i in vec[i])
      - Pointer++ is one instruction
      - Compiler usually does this at -O2+
   
   6. PROFILING
      - Measure with: perf stat -e cycles,instructions ./program
      - Look at CPI (cycles per instruction)
      - v1-v3: high CPI from memory stalls
      - v4-v7: lower CPI, better ILP
   
   COMPILER NOTE:
   - Compile with: gcc -O0 -fno-inline to see differences
   - With -O2: compiler does v2, v3, v7 automatically
   - With -O3: may do unrolling, but can't do v4 optimization
     (doesn't know *dest isn't aliased)
   - Use -fopt-info to see what compiler does
   
   WHY V4 IS SO IMPORTANT:
   The compiler CANNOT do the v3->v4 optimization automatically
   because it doesn't know if 'dest' points into 'vec'. If it did,
   then the optimization would be incorrect! This is called aliasing.
   You can hint with 'restrict' keyword, but algorithmic changes
   like this are still your responsibility.
   ============================================ */
