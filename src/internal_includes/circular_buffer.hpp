/*
 * Copyright (c) 2024, ITGSS Corporation. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * Contact with ITGSS, 651 N Broad St, Suite 201, in the
 * city of Middletown, zip code 19709, and county of New Castle, state of Delaware.
 * or visit www.it-gss.com if you need additional information or have any
 * questions.
 *
 */
 

#ifndef _LOG_ALLOC_H
#define _LOG_ALLOC_H

#include <atomic>

namespace Atlas {
    
template<class T>
struct CbLog
{
    explicit CbLog(uint32_t sz, uint32_t is_filled,
          uint32_t start_cb, uint32_t end_cb)
        : Size{sz},
        isFilled{is_filled},
        Start{start_cb},
        End{end_cb},
        LogArray{nullptr} {}
    CbLog() = delete;
    CbLog(const CbLog&) = delete;
    CbLog(CbLog&&) = delete;
    CbLog& operator=(const CbLog&) = delete;
    CbLog& operator=(CbLog&&) = delete;
    
    uint32_t Size;
    std::atomic<uint32_t> isFilled;
    std::atomic<uint32_t> Start;
    std::atomic<uint32_t> End;
    T *LogArray;

    bool isFull() {
        return (End.load(std::memory_order_acquire)+1) % Size ==
            Start.load(std::memory_order_acquire);
    }

    bool isEmpty() {
        return Start.load(std::memory_order_acquire) ==
            End.load(std::memory_order_acquire);
    }
};

template<class T>
struct CbListNode
{
    explicit CbListNode(CbLog<T> *cb, char *start_addr, char *end_addr)
        : Cb{cb},
        StartAddr{start_addr},
        EndAddr{end_addr},
        Next{nullptr},
        Tid{pthread_self()},
        isAvailable{false} {}
    CbListNode() = delete;
    CbListNode(const CbListNode&) = delete;
    CbListNode(CbListNode&&) = delete;
    CbListNode& operator=(const CbListNode&) = delete;
    CbListNode& operator=(CbListNode&&) = delete;
    
    CbLog<T> *Cb;
    char *StartAddr;
    char *EndAddr;
    CbListNode<T> *Next;
    pthread_t Tid;
    std::atomic<uint32_t> isAvailable;
};

// CbList is a data structure shared among threads. When a new slot is
// requested, if the current buffer is found full, the current thread
// creates a new buffer, adds it to the CbList and
// return the first slot from this new buffer. If a buffer ever becomes
// empty, it can be reused. A partially empty buffer cannot be reused.

// TODO eventual GC on cb_list

} // namespace Atlas

#endif
