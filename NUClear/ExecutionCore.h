#ifndef NUCLEAR_EXECUTIONCORE_H
#define NUCLEAR_EXECUTIONCORE_H

#include <thread>
#include <ctime>

namespace NUClear {
    class ExecutionCore {
    public:
        ExecutionCore();
        ~ExecutionCore();
        void kill();
    private:
        void core();
        bool execute;
        std::thread thread;
    };
}

#endif