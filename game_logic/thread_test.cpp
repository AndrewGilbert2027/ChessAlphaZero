#include <pthread.h>
#include <iostream>


int main() {
    pthread_t thread1, thread2;

    // Function to be executed by the threads
    auto thread_function = [](void* arg) -> void* {
        int id = *(int*)arg;
        std::cout << "Thread " << id << " is running.\n";
        return nullptr;
    };

    int id1 = 1, id2 = 2;

    // Create threads
    pthread_create(&thread1, nullptr, thread_function, &id1);
    pthread_create(&thread2, nullptr, thread_function, &id2);

    // Wait for threads to finish
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    std::cout << "Both threads have finished execution.\n";

    return 0;
}