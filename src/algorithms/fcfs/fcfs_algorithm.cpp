#include "algorithms/fcfs/fcfs_algorithm.hpp"

#include <cassert>
#include <stdexcept>
#include <queue>
#include <memory>
#include <iostream>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the FCFS algorithm.
*/

// Constructor
FCFSScheduler::FCFSScheduler(int slice)
{
    if (slice != -1)
    {
        throw("FCFS must have a timeslice of -1");
    }
}

// Gets the next thread based on FCFS
std::shared_ptr<SchedulingDecision> FCFSScheduler::get_next_thread()
{
    if (readyQueue.empty())
    {
        SchedulingDecision emptyDecision;
        emptyDecision.explanation = "No threads available for scheduling.";
        emptyDecision.thread = nullptr;
        emptyDecision.time_slice = -1;

        return std::make_shared<SchedulingDecision>(emptyDecision);
    }

    std::shared_ptr<Thread> nextThread = readyQueue.front();
    readyQueue.pop();

    SchedulingDecision decision;
    decision.explanation = "Selected from " + std::to_string(get_num_ready_threads()) + " threads. Will run to completion of burst.";
    decision.thread = nextThread;
    decision.time_slice = -1;

    return std::make_shared<SchedulingDecision>(decision);
}

// Adds a thread to the queue
void FCFSScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread)
{
    readyQueue.push(thread);
}

// Gets the size of the queue
size_t FCFSScheduler::size() const
{
    return readyQueue.size();
}

// Gets the number of ready threads in the queue
int FCFSScheduler::get_num_ready_threads()
{
    int readyCount = 0;
    std::queue<std::shared_ptr<Thread>> tempQueue = readyQueue;
    while (!tempQueue.empty())
    {
        std::shared_ptr<Thread> thread = tempQueue.front();
        tempQueue.pop();
        if (thread->current_state == ThreadState::READY)
        {
            readyCount++;
        }
    }
    return readyCount + 1;
}
