/**
 *
 */

#include <tervel/util/tervel.h>
#include <tervel/containers/wf/vector/vector.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// glibc-specific includes
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

tervel::Tervel* g_tervel;

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
void add_hello_world(uint64_t thread_id, tervelVector* vector)
{
    if(vector == nullptr) { printf("vector object null!\n"); return; }

    tervel::ThreadContext* thread_context = new tervel::ThreadContext(g_tervel);

    while(!vector->push_back(thread_id)) {}
}

// This is needed so that tervel doesn't assert.
typedef union
{
    uint64_t value;

    struct
    {
        uint64_t tervel_reserved : 2;   // These are reserved by Tervel
        uint64_t tervel_value : 62;     // The actual integer value
    };
} tervel_uint64_t;

int main (int argc, char** argv)
{
    printf("Registering signal handler...\n");
    signal(SIGSEGV, handler);

    printf("Command line arguments:\n");
    for(int i = 0; i < argc; i++)
    {
        printf("\t%s\n", argv[i]);
    }

    tervelVector* test_vector;
    
    // Tervel initialization code
    g_tervel = new tervel::Tervel(4);
    test_vector = new tervelVector(64);

    // Initialize and start the threads
    std::vector<std::thread> threads;
    for(int i = 0; i < 4; i++)
    {
        tervel_uint64_t v;
        v.tervel_reserved = 0;
        v.tervel_value = i;

        std::thread th(add_hello_world, v.value, test_vector);
        threads.push_back(std::move(th));
    }

    // Wait for the threads to finish
    threads[0].join();
    threads[1].join();
    threads[2].join();
    threads[3].join();

    // Print out what was put into the vector
    for(int i = 0; i < test_vector->size(); i++)
    {
        uint64_t t = 0;
        tervel_uint64_t id;
        id.value = 0;

        while(!test_vector->pop_back(t)) {}

        id.value = t;
        printf("Hello World %lu\n", id.tervel_value);
    }

    return 0;
}
