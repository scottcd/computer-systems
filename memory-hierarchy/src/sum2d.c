#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

static inline uint64_t nsec(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec*1000000000ull + ts.tv_nsec;
}

static inline void cache_flush(void){
    // Touch a big buffer (~64 MiB) to evict prior working set.
    static size_t bytes = 64ull<<20;
    static unsigned char *buf;
    if(!buf){ posix_memalign((void**)&buf, 64, bytes); memset(buf, 1, bytes); }
    volatile unsigned char s=0;
    for(size_t i=0;i<bytes;i+=64) s ^= buf[i];
    (void)s;
}

static long long sum_rows(const int *a, int N, int reps){
    long long s = 0;
    for(int r=0;r<reps;r++){
        for(int i=0;i<N;i++){
            const int *row = a + (size_t)i*N;
            for(int j=0;j<N;j++) s += row[j];
        }
    }
    return s;
}

static long long sum_cols(const int *a, int N, int reps){
    long long s = 0;
    for(int r=0;r<reps;r++){
        for(int j=0;j<N;j++){
            for(int i=0;i<N;i++) s += a[(size_t)i*N + j];
        }
    }
    return s;
}

static double bench(long long (*fn)(const int*,int,int), const int *a, int N, int reps){
    cache_flush();
    uint64_t t0 = nsec();
    volatile long long guard = fn(a, N, reps);
    uint64_t t1 = nsec();
    (void)guard; // keep the loops “live”
    double loads = (double)N*N*reps;
    return (t1 - t0) / loads; // ns per element
}

int main(int argc, char **argv){
    // Defaults good for your caches (L1d≈576 KiB, L2≈24 MiB, L3≈30 MiB)
    int Nmin = 256, Nmax = 8192, factor = 2, reps = 8;

    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--min") && i+1<argc) Nmin = atoi(argv[++i]);
        else if(!strcmp(argv[i],"--max") && i+1<argc) Nmax = atoi(argv[++i]);
        else if(!strcmp(argv[i],"--factor") && i+1<argc) factor = atoi(argv[++i]);
        else if(!strcmp(argv[i],"--reps") && i+1<argc) reps = atoi(argv[++i]);
    }

    printf("N,bytes,ns_per_elem_rows,ns_per_elem_cols\n");
    for(int N=Nmin; N<=Nmax; N*=factor){
        size_t elems = (size_t)N*N;
        size_t bytes = elems*sizeof(int);

        int *a;
        if(posix_memalign((void**)&a, 64, bytes) != 0) { perror("alloc"); return 1; }
        // Initialize with a deterministic pattern to avoid easy constant-folding.
        for(size_t i=0;i<elems;i++) a[i] = (int)(i*1315423911u);

        // Scale reps so each point takes long enough (target ~20–50 ms)
        int dyn_reps = reps;
        if(bytes < (512<<10))      dyn_reps = reps*64;      // < 512 KiB
        else if(bytes < (8<<20))   dyn_reps = reps*16;      // < 8 MiB
        else if(bytes < (64<<20))  dyn_reps = reps*4;

        double nse_rows = bench(sum_rows, a, N, dyn_reps);
        double nse_cols = bench(sum_cols, a, N, dyn_reps);

        printf("%d,%zu,%.3f,%.3f\n", N, bytes, nse_rows, nse_cols);
        free(a);
    }
    return 0;
}

