#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wasm_simd128.h>

#define SOKOL_IMPL
#include "sokol_time.h"

////////////////////////////////////////////////////////////////////////////////

typedef uint32_t ValueType;
typedef ValueType (*TestFunction)(ValueType *, size_t);

void runTest(const char *pName, int runsPerTest, TestFunction func, ValueType *pData, size_t count)
{
    uint64_t start = stm_now();
    ValueType total = 0;
    for (int i = 0; i < runsPerTest; i++)
    {
        total += func(pData, count);
    }
    uint64_t end = stm_now();
    double averageTimeInMS = stm_ms(stm_diff(end, start)) / runsPerTest;

    printf("[%.5f ms][%s] (x%d) %d\n", averageTimeInMS, pName, runsPerTest, total);
}

////////////////////////////////////////////////////////////////////////////////

ValueType test_loop(ValueType *pData, size_t count)
{
    ValueType total = 0;
    for (int i = 0; i < count; i++)
    {
        total += (pData[i] * pData[i]);
    }
    return total;
}

#if defined(__wasm_simd128__)
ValueType test_intrinsics(ValueType *pData, size_t count)
{
    v128_t acc = wasm_i32x4_splat(0);
    for (int i = 0; i < count / 4; i++)
    {
        v128_t src = wasm_v128_load(&pData[i * 4]);
        v128_t dst = wasm_i32x4_mul(src, src);
        acc = wasm_i32x4_add(acc, dst);
    }
    return wasm_i32x4_extract_lane(acc, 0) + wasm_i32x4_extract_lane(acc, 1) + wasm_i32x4_extract_lane(acc, 2) + wasm_i32x4_extract_lane(acc, 3);
}
#endif

////////////////////////////////////////////////////////////////////////////////

int main()
{
    stm_setup();

    // The number of times we run a test. The final results are averaged out.
    const size_t RUNS_PER_TEST = 1000;

    // The data we are processing.
    const size_t COUNT = 1024 * 1024;
    ValueType *pData = (ValueType *)malloc(sizeof(ValueType) * COUNT);
    for (ValueType i = 0; i < COUNT; i++)
    {
        pData[i] = i;
    }

#if defined(__wasm_simd128__)
    runTest("SIMD Auto Vectorization", RUNS_PER_TEST, test_loop, pData, COUNT);
    runTest("SIMD Manual Intrinsics", RUNS_PER_TEST, test_intrinsics, pData, COUNT);
#else
    runTest("Loop", RUNS_PER_TEST, test_loop, pData, COUNT);
#endif

    free(pData);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////