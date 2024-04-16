#include "algorithms/priority/priority_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"


/*
    Here is where you should define the logic for the priority algorithm.
*/

PRIORITYScheduler::PRIORITYScheduler(int slice) {
    if (slice != -1) {
        throw("PRIORITY must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> PRIORITYScheduler::get_next_thread() {
    if (priorityQueue.empty())
    {
        SchedulingDecision emptyDecision;
        emptyDecision.explanation = "No threads available for scheduling.";
        emptyDecision.thread = nullptr;
        emptyDecision.time_slice = -1;

        return std::make_shared<SchedulingDecision>(emptyDecision);
    }
    std::vector<int>*  before_nums = get_num_threads();
    std::shared_ptr<Thread> nextThread = priorityQueue.top();
    priorityQueue.pop();
    SchedulingDecision decision;
    std::vector<int>*  after_nums = get_num_threads();
    decision.explanation = "[S: " + std::to_string((*before_nums)[0]) + " I: " + std::to_string((*before_nums)[1]) + 
                            " N: " + std::to_string((*before_nums)[2]) + " B: " + std::to_string((*before_nums)[3]) + 
                            "] -> [S: " + std::to_string((*after_nums)[0]) + " I: " + std::to_string((*after_nums)[1]) + 
                            " N: " +std::to_string((*after_nums)[2]) + " B: " + std::to_string((*after_nums)[3]) + 
                            "]. Will run to completion of burst.";
    decision.thread = nextThread;
    decision.time_slice = -1;

    return std::make_shared<SchedulingDecision>(decision);
}

void PRIORITYScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    priorityQueue.push(get_priority(thread), thread);
}

size_t PRIORITYScheduler::size() const {
    return priorityQueue.size();
}

int PRIORITYScheduler::get_priority(std::shared_ptr<Thread> thread) {
    switch(thread->priority) {
        case ProcessPriority::SYSTEM:
            return 0;
        case ProcessPriority::INTERACTIVE:
            return 1;
        case ProcessPriority::NORMAL:
            return 2;
        case ProcessPriority::BATCH:
            return 3;
        default:
            throw("Error getting priority");
    }
}

std::vector<int>* PRIORITYScheduler::get_num_threads() {
    // Assuming there are a fixed number of priorities that matches the vector size.
    std::vector<int> *array = new std::vector<int>(4, 0); // Initialize vector with 4 zeros
    Stable_Priority_Queue<std::shared_ptr<Thread>> queueCopy = priorityQueue;

    while (!queueCopy.empty()) {
        std::shared_ptr<Thread> thread = queueCopy.top();
        queueCopy.pop();
        (*array)[get_priority(thread)]++;
    }
    return array;
}
