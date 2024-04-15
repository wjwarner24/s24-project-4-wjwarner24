#include "algorithms/rr/rr_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the round robin algorithm.
*/

RRScheduler::RRScheduler(int slice)
{
    if (slice == -1)
    {
        time_slice = 3;
    }
    else
    {
        time_slice = slice;
    }
}

std::shared_ptr<SchedulingDecision> RRScheduler::get_next_thread()
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
    decision.explanation = "Selected from " + std::to_string(get_num_ready_threads()) + " threads. Will run for at most " + std::to_string(time_slice) + " ticks.";
    decision.thread = nextThread;
    decision.time_slice = time_slice;

    return std::make_shared<SchedulingDecision>(decision);
}

void RRScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread)
{
    readyQueue.push(thread);
}

size_t RRScheduler::size() const
{
    return readyQueue.size();
}

// Gets the number of ready threads in the queue
int RRScheduler::get_num_ready_threads()
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
