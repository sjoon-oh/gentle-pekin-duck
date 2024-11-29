/*
 * buffer.hh
 * Author: Sukjoon Oh (sjoon@kaist.ac.kr)
 */

#ifndef _BUFFER_H
#define _BUFFER_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <algorithm>

namespace pduck
{
    namespace memory
    {
        /**
         * @class Buffer
         * @brief Abstract base class for managing memory buffers.
         * 
         * The Buffer class provides a base interface for managing memory buffers.
         * It stores a block of memory and provides methods to get the size and 
         * address of the memory block.
         */
        class Buffer
        {
        protected:
            size_t m_blockSize; ///< Size of the memory block.
            std::unique_ptr<std::uint8_t[]> m_blockContainer; ///< Unique pointer to the memory block.

        public:
            /**
             * @brief Constructs a Buffer object.
             * @param p_size Size of the buffer.
             */
            Buffer(size_t p_size) noexcept
                : m_blockSize(p_size)
            {
                m_blockContainer.reset(new std::uint8_t[m_blockSize]);
            }

            /**
             * @brief Constructs a Buffer object.
             * @param p_buffer Pointer to the initial buffer.
             * @param p_size Size of the buffer.
             */
            Buffer(uint8_t* p_buffer, size_t p_size) noexcept
                : m_blockSize(p_size)
            {
                m_blockContainer.reset(new std::uint8_t[m_blockSize]);
                std::memcpy(m_blockContainer.get(), p_buffer, m_blockSize);
            }

            /**
             * @brief Virtual destructor for Buffer.
             */
            virtual ~Buffer() = default;

            /**
             * @brief Gets the size of the buffer.
             * @return Size of the buffer.
             */
            virtual size_t getSize() const noexcept
            {
                return m_blockSize;
            }

            /**
             * @brief Gets the address of the buffer.
             * 
             * @return Pointer to the buffer.
             */
            virtual std::uint8_t* getAddr() noexcept 
            {
                return m_blockContainer.get();
            }
        };

        /**
         * @class FixedBuffer
         * @brief Concrete implementation of Buffer with a fixed size.
         * 
         * The FixedBuffer class provides a fixed-size implementation of the Buffer
         * class. It inherits from Buffer and implements the getSize and getAddr 
         * methods.
         */
        class FixedBuffer : public Buffer
        {
        public:

            /**
             * @brief Constructs a FixedBuffer object.
             * @param p_size Size of the buffer.
             */
            FixedBuffer(size_t p_size) noexcept
                : Buffer(p_size)
            {

            }

            /**
             * @brief Constructs a FixedBuffer object.
             * @param p_buffer Pointer to the initial buffer.
             * @param p_size Size of the buffer.
             */
            FixedBuffer(uint8_t* p_buffer, size_t p_size) noexcept
                : Buffer(p_buffer, p_size)
            {

            }

            /**
             * @brief Destructor for FixedBuffer.
             */
            virtual ~FixedBuffer() noexcept = default;
        };
    }
}

#endif