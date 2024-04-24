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

    while (queues.at(index).empty())
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
    }
    std::shared_ptr<Thread> nextThread = queues.at(index).top();
    queues.at(index).pop();

    SchedulingDecision decision;
    decision.thread = nextThread;
    nextThread->prev_service_time = nextThread->service_time;
    nextThread->last_queue_level = index;
    this->time_slice = std::pow(2, index);
    decision.time_slice = -1;
    decision.explanation = "Selected from queue " + std::to_string(index) + " (priority = " + 
                            get_priority_str(nextThread) + ", runtime = " + std::to_string(nextThread->mlfq_time) + 
                            "). Will run for at most " + std::to_string(time_slice) + " ticks.";

    return std::make_shared<SchedulingDecision>(decision);

}

void MLFQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread)
{
    if (thread->last_queue_level == -1) {
        thread->last_queue_level = 0;
        queues.at(thread->last_queue_level).push(get_priority(thread), thread);
        return;
    }
    thread->mlfq_time += thread->service_time - thread->prev_service_time;
    if (thread->mlfq_time >= std::pow(2, thread->last_queue_level)) {
        thread->last_queue_level += 1;
        thread->mlfq_time = 0;
    }
    queues.at(thread->last_queue_level).push(get_priority(thread), thread);
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

std::string MLFQScheduler::get_priority_str(std::shared_ptr<Thread> thread) {
    switch (thread->priority)
    {
    case ProcessPriority::SYSTEM:
        return "SYSTEM";
    case ProcessPriority::INTERACTIVE:
        return "INTERACTIVE";
    case ProcessPriority::NORMAL:
        return "NORMAL";
    case ProcessPriority::BATCH:
        return "BATCH";
    default:
        throw std::runtime_error("Error getting priority");
    }
}

