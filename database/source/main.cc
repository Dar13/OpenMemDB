/**
 *
 */

#include <tervel/util/tervel.h>
#include <tervel/containers/wf/vector/vector.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <cstdint>

// glibc-specific includes
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

// This is needed so that tervel doesn't assert.
struct tervel_uint64_t
{
    uint64_t value_;
    tervel_uint64_t(uint64_t v=0)
        : value_(v) {}


    uint64_t value() {
        return value_;
    }

    uint64_t value_a() {
        return value_ << 3;
    }

    uint64_t value_b() {
        return value_ >> 3;
    }
};

void handler(int signal)
{
    void* array[10];
    size_t size;

    size = backtrace(array,10);

    fprintf(stderr, "Error: signal %d thrown\n", signal);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

typedef tervel::containers::wf::vector::Vector<uint64_t> tervelVector;

// This is the thread function
void add_hello_world(uint64_t thread_id, tervel::Tervel* tervel, tervelVector* vector)
{
    if(vector == nullptr) {
        printf("vector object null!\n");
        return;
    }

    tervel::ThreadContext* thread_context = new tervel::ThreadContext(tervel);

    tervel_uint64_t temp(thread_id);

    size_t ret = vector->push_back_only(temp.value_a());
    printf("idx: %u tid: %lu pushed: %lu\n",ret, thread_id, temp.value_a());

    delete thread_context;

    std::this_thread::yield();

    return;
}



int main (int argc, char** argv)
{
    printf("Registering signal handler...\n");
    signal(SIGSEGV, handler);

    printf("Command line arguments:\n");
    for(int i = 0; i < argc; i++)
    {
        printf("\t%s\n", argv[i]);
    }

    tervel::Tervel* tervel;
    tervelVector* test_vector;

    int nThreads = 10;
    // Tervel initialization code
    tervel = new tervel::Tervel(nThreads);
    test_vector = new tervelVector(64);

    // The main thread counts as a thread
    tervel::ThreadContext* thread_context = new tervel::ThreadContext(tervel);

    // Initialize and start the threads
    std::vector<std::thread> threads;

    for(int i = 1; i < nThreads; i++)
    {
        std::thread th(add_hello_world, i, tervel, test_vector);
        threads.push_back(std::move(th));
    }

    for(auto i = threads.begin(); i != threads.end(); i++)
    {
        (*i).join();
    }

    for(int i = 1; i < nThreads; i++)
    {
        // Print out what was put into the vector
        uint64_t t = 0;

        if(test_vector->pop_back_only(t))
        {
            tervel_uint64_t temp(t);
            printf("PoPed: %lu %lu\n", t, temp.value_b());
        }
    }

    return 0;
}
