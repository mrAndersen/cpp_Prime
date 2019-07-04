#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <unistd.h>


struct ThreadBorders {
    int min = 0;
    int max = 0;
};

ThreadBorders getThreadBorders(int index, int threadRange = 100000) {
    int min = index == 0 ? 0 : index * threadRange;
    int max = min + threadRange;

    return {min, max};
}

bool isPrime(int64_t integer) {
    int popular[] = {2, 3, 5, 7, 10, 11, 13, 17, 19, 21};

    for (int i = 0; i < sizeof(popular) / sizeof(int); ++i) {
        auto dev = popular[i];

        if (integer % dev == 0 && integer != dev) {
            return false;
        }
    }

    int devs = 2;

    for (int j = 2; j < integer; ++j) {
        if (integer % j == 0) {
            devs++;
        }
    }

    return devs <= 2;
}


std::mutex v_mutex;
std::vector<int> primeBuf;
std::vector<int> lastPrimes;

int main(int argc, char **argv) {
    int size = 2;
    int range = 1000000;

    if (argc > 1) {
        size = atoi(argv[1]);
    }

    std::vector<std::thread> pool;
    primeBuf.resize(size);
    lastPrimes.resize(size);

    for (int i = 0; i < size; ++i) {
        auto borders = getThreadBorders(i, range);

        std::thread thread([borders, i]() {
            int primes = 0;

            for (int j = borders.min; j < borders.max; ++j) {
                if (isPrime(j)) {
                    primes++;

                    v_mutex.lock();
                    primeBuf[i] = primes;
                    lastPrimes[i] = j;
                    v_mutex.unlock();
                }
            }
        });

        pool.emplace_back(std::move(thread));
    }

    for (auto &thread:pool) {
        thread.detach();
    }

    int count = 0;

    while (true) {
        system("clear");
        printf("%d ms, range = %d\n", count * 500, range);

        for (int i = 0; i < size; ++i) {
            auto borders = getThreadBorders(i, range);
            printf("Thread %d [%d - %d] -> %d (%d)\n", i, borders.min, borders.max, primeBuf[i], lastPrimes[i]);
        }

        count++;
        usleep(500000);
    }
}