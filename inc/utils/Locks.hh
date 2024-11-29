#ifndef _LOCK_H
#define _LOCK_H

#include <atomic>

namespace pduck
{
    namespace utils
    {
        /**
         * @class Spinlock
         * @brief A simple and fast spinlock implementation using std::atomic_flag.
         *
         * The Spinlock class provides a simple and efficient spinlock mechanism
         * for mutual exclusion. It uses the std::atomic_flag to implement the
         * lock and unlock operations.
         */
        class Spinlock {
            std::atomic_flag flag = ATOMIC_FLAG_INIT;

        public:
            /**
             * @brief Acquires the lock.
             *
             * This function will busy-wait until the lock is acquired.
             * It uses std::atomic_flag::test_and_set with memory_order_acquire
             * to ensure proper synchronization.
             */
            void lock() noexcept 
            {
                while (flag.test_and_set(std::memory_order_acquire))
                    ;
            }

            /**
             * @brief Releases the lock.
             *
             * This function releases the lock by clearing the atomic flag.
             * It uses std::atomic_flag::clear with memory_order_release to
             * ensure proper synchronization.
             */
            void unlock() noexcept
            {
                flag.clear(std::memory_order_release);
            }
        };
    }
}

#endif // _LOCK_H