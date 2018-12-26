
// arman.nersisyan111@gmail.com, angela.shahbazyan98@gmail.com

#include <stdio.h>
#include <Windows.h>

#include <assert.h>
#include <stdint.h>
#include <iostream>

#define K 10 // blocks count
#define N 8 // theards count in one block
#define L (K * N)

static double F[L];
SYNCHRONIZATION_BARRIER barrier;

DWORD WINAPI ThreadFunction(void* params)
{
    assert(NULL != params);
    int i = *((int*)params);

    assert(i < N);
    assert(0 <= i);

    for (auto j = 0; j < K - 1; ++j) {
        auto n = (j + 1) * N + i;
        //std::cout << "N " << n << std::endl;
        if (1 == (n % 2)) {
            auto idx = (n - 1) / 2;
            auto f0 = F[idx];
            auto f1 = F[idx - 1];
            F[n - 1] = f0 * f0 + f1 * f1;
        } else {
            auto idx = n / 2;
            auto f0 = F[idx];
            auto f1 = F[idx - 1];
            F[n + 1] = f0 * (2 * f1 + f0);
        }
        //std::cout << i << "-th thread for block " << j << std::endl;
        EnterSynchronizationBarrier(&barrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);
    }
    return 0;
}

int main()
{
    int param[N];
    HANDLE h[N];

    F[0] = 1;
    F[1] = 1;
    for (int i = 2; i < N; i++) {
        F[i] = F[i - 1] + F[i - 2];
    }
    InitializeSynchronizationBarrier(&barrier, N, -1);
    for (int i = 0; i < N; i++) {
        DWORD id;
        param[i] = i;
        h[i] = CreateThread(NULL, 100, ThreadFunction, &param[i], 0, &id);
        if (NULL == h[i]) {
            std::cout << "Faild to create thread" << std::endl;
            return 1;
        }
    }
    // Wait for thread to exit
    ::WaitForMultipleObjects(L, h, true, INFINITE);
    for (int i = 0; i < N; i++) {
        CloseHandle(h[i]);
    }
    DeleteSynchronizationBarrier(&barrier);

    for (int i = 0; i < L; i++) {
        std::cout << "F[" << i << "] = " << F[i] << std::endl;
    }

    getchar();
    return 0;
}
