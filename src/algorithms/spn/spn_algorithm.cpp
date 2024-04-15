#include "algorithms/spn/spn_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"
#include "utilities/stable_priority_queue/stable_priority_queue.hpp"

// Constructor
SPNScheduler::SPNScheduler(int slice)
{
    if (slice != -1)
    {
        throw("SPN must have a timeslice of -1");
    }
}

// Gets the next thread based on SPN
std::shared_ptr<SchedulingDecision> SPNScheduler::get_next_thread()
{
    if (priorityQueue.empty())
    {
        SchedulingDecision emptyDecision;
        emptyDecision.explanation = "No threads available for scheduling.";
        emptyDecision.thread = nullptr;
        emptyDecision.time_slice = -1;

        return std::make_shared<SchedulingDecision>(emptyDecision);
    }
    std::shared_ptr<Thread> nextThread = priorityQueue.top();
    priorityQueue.pop();
    SchedulingDecision decision;
    decision.explanation = "Selected from " + std::to_string(get_num_ready_threads()) + " threads. Will run to completion of burst.";
    decision.thread = nextThread;
    decision.time_slice = -1;

    return std::make_shared<SchedulingDecision>(decision);
}

// Adds a thread to the ready queue
void SPNScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread)
{
    priorityQueue.push(get_next_cpu_burst_length(thread), thread);
}

// Gets the size of the queue
size_t SPNScheduler::size() const
{
    return priorityQueue.size();
}

// Gets the number of ready threads in the queue
int SPNScheduler::get_num_ready_threads()
{
    int readyCount = 0;
    Stable_Priority_Queue<std::shared_ptr<Thread>> tempQueue = priorityQueue;
    while (!tempQueue.empty())
    {
        std::shared_ptr<Thread> thread = tempQueue.top();
        tempQueue.pop();

        if (thread->current_state == ThreadState::READY)
        {
            readyCount++;
        }
    }
    return readyCount + 1;
}

int SPNScheduler::get_next_cpu_burst_length(std::shared_ptr<Thread> thread)
{
    std::queue<std::shared_ptr<Burst>> burstsCopy = thread->bursts;
    if (burstsCopy.empty())
    {
        return 0;
    }
    std::shared_ptr<Burst> currentBurst = burstsCopy.front();
    burstsCopy.pop();
    while (currentBurst->burst_type != BurstType::CPU)
    {
        if (burstsCopy.empty())
        {
            return 0;
        }
        currentBurst = burstsCopy.front();
        burstsCopy.pop();
    }
    return currentBurst->length;
}
