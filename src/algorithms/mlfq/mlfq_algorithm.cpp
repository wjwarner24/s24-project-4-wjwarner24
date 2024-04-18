#include "algorithms/mlfq/mlfq_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"
#include "utilities/stable_priority_queue/stable_priority_queue.hpp"

/*
    Here is where you should define the logic for the MLFQ algorithm.
*/

MLFQScheduler::MLFQScheduler(int slice)
{
    if (slice != -1)
    {
        throw("MLFQ does NOT take a customizable time slice");
    }
    for (int i = 0; i < 10; i++) {
        queues.push_back(Stable_Priority_Queue<std::shared_ptr<Thread>>());
    }
}

std::shared_ptr<SchedulingDecision> MLFQScheduler::get_next_thread()
{
    int index = 0;
    Stable_Priority_Queue<std::shared_ptr<Thread>> nextQueue = queues.at(index);

    while (nextQueue.empty())
    {
        index++;
        if (index > 9)
        {
            SchedulingDecision emptyDecision;
            emptyDecision.explanation = "No threads available for scheduling.";
            emptyDecision.thread = nullptr;
            emptyDecision.time_slice = -1;

            return std::make_shared<SchedulingDecision>(emptyDecision);
        }
        nextQueue = queues.at(index);
    }
    std::shared_ptr<Thread> nextThread = nextQueue.top();
    nextQueue.pop();
    SchedulingDecision decision;
    decision.thread = nextThread;
    int priority = std::pow(2, index);
    decision.time_slice = priority;
    decision.explanation = "Selected from queue " + std::to_string(index) + " (priority = " + 
                            std::to_string(get_priority(nextThread)) + ", runtime = " + std::to_string(priority - 1) + 
                            "). Will run for at most " + std::to_string(priority) + " ticks.";

    return std::make_shared<SchedulingDecision>(decision);
}

void MLFQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread)
{
    auto key = std::make_pair(thread->process_id, thread->thread_id);
    if (threadMap.find(key) != threadMap.end())
    {
        // std::cout << "Key found in the map." << std::endl;
        threadMap[key]++;
    }
    else
    {
        // std::cout << "Key not found in the map." << std::endl;
        threadMap.insert(std::make_pair(key, 0));
    }
    queues.at(threadMap[key]).push(get_priority(thread), thread);
}

size_t MLFQScheduler::size() const
{
    int size = 0;
    for (int i = 0; i < (int) queues.size(); i++)
    {
        size += queues.at(i).size();
    }
    return size;
}

int MLFQScheduler::get_priority(std::shared_ptr<Thread> thread)
{
    switch (thread->priority)
    {
    case ProcessPriority::SYSTEM:
        return 0;
    case ProcessPriority::INTERACTIVE:
        return 1;
    case ProcessPriority::NORMAL:
        return 2;
    case ProcessPriority::BATCH:
        return 3;
    default:
        throw std::runtime_error("Error getting priority");
    }
}
