// Here lies yet another artifact of the terrible C++ "module" system, and I use that term sparingly. 
// Reactor.h needs to be included before ReactorController in all contexts, this is neccecary because of the
// three class circular dependency between [ReactorController -> ReactorMaster -> Reactor]. By forcing Reactor.h
// here we can ensure that the defintion of Reactor will exist for ReactorMaster avoiding compile time errors
// and making the whole mess work. If C++ had a module system this would be a non-issue but we're looking at
// sometime between 2014-2017 to get one of those! It needs to be outside of the macro guards because 
// NUCLEAR_REACTORCONTROLLER_H needs to not be defined yet so it is included within the appropriate context
// in ReactorMaster.h
//#include "Reactor.h"

#ifndef NUCLEAR_REACTORCONTROLLER_H
#define NUCLEAR_REACTORCONTROLLER_H
#include <memory>
#include <set>
#include <thread>
#include <vector>
#include <typeindex>
#include <map>
#include <iostream>
#include "Internal/Every.h"
#include "Internal/ThreadWorker.h"
#include "Internal/TaskScheduler.h"

namespace NUClear {
    class Reactor;
    class ReactorController {
        private:
            friend class Reactor;

            class BaseMaster {
                public:
                    BaseMaster(ReactorController* parent); 
                protected:
                    ReactorController* m_parent;
            };

            class ChronoMaster : public BaseMaster {
                public:
                    ChronoMaster(ReactorController* parent);
                    ~ChronoMaster();

                    void run();

                    template <int ticks, class period>
                    void add();
                private:
                    /// @brief this class holds the callbacks to emit events, as well as when to emit these events.
                    struct Step {
                        std::chrono::nanoseconds step;
                        std::chrono::nanoseconds next;
                        std::vector<std::function<void ()>> callbacks;
                    };
                
                    /// @brief If the system should continue to execute or if it should stop
                    bool m_execute;
                
                    /// @brief A vector of steps containing the callbacks to execute, is sorted regularly to maintain the order
                    std::vector<std::unique_ptr<Step>> m_steps;
                
                    /// @brief A list of types which have already been loaded (to avoid duplication)
                    std::set<std::type_index> m_loaded;
            };

            class ReactorMaster : public BaseMaster {
                public:
                    ReactorMaster(ReactorController* parent);

                    template <typename TTrigger>
                    void emit(TTrigger* data);

                    template <typename TData>
                    std::shared_ptr<TData> get();

                    template <typename TReactor>
                    void install();

                    template <typename TTrigger>
                    void subscribe(NUClear::Reactor* reactor);
                private:
                    template <typename TTrigger>
                    void cache(TTrigger* data);

                    template <typename TTrigger>
                    void notifyReactors();

                    template <typename TTrigger>
                    std::set<NUClear::Reactor*>& getReactorBindings();

                    std::vector<std::unique_ptr<NUClear::Reactor>> m_reactors;
                    std::map<std::type_index, std::shared_ptr<void> > m_cache;
                    std::map<std::type_index, std::set<NUClear::Reactor*> > m_reactorBindings;
            };

            class ThreadMaster : public BaseMaster {
                public:
                    ThreadMaster(ReactorController* parent);

                    void start();
                private:
                    std::map<std::thread::id, std::unique_ptr<Internal::ThreadWorker>> m_threads;

                    Internal::TaskScheduler scheduler;
                    int numThreads = 4;
            };
        protected:
            ChronoMaster chronomaster;
            ReactorMaster reactormaster;
            ThreadMaster threadmaster;
        public:
            ReactorController();

            void start();

            template <typename TData>
            std::shared_ptr<TData> get();
        
            template <typename TReactor>
            void install();
            
            template <typename TTrigger>
            void emit(TTrigger* data);

            // Chrono functions

            // Threading Functions

    };
}

#include "Reactor.h"
#include "ChronoMaster.ipp"
#include "ReactorMaster.ipp"
#include "ReactorController.ipp"
#endif

