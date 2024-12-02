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

#include <cstdlib>
#include <cassert>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


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
            size_t                                      m_blockSize; // Size of the memory block.
            std::unique_ptr<std::uint8_t[]>             m_blockContainer; // Unique pointer to the memory block.

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
            virtual std::uint8_t* getBlock() noexcept 
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


        /**
         * @class DynamicBuffer
         * @brief Concrete implementation of Buffer with a dynamic size.
         * 
         * The DynamicBuffer class provides a dynamic-size implementation of the Buffer
         * class. It inherits from Buffer and implements the getSize and getAddr 
         * methods.
         */

        class DynamicAlignedBuffer
        {
        protected:
            size_t                                      m_blockSize;            // Size of the memory block.
            std::unique_ptr<std::uint8_t[]>             m_blockContainer;       // Unique pointer to the memory block.

        public:
            /**
             * @brief Constructs a DynamicBuffer object.
             * @param p_size Size of the buffer.
             */
            DynamicAlignedBuffer(size_t p_size, size_t p_alignment = 4) noexcept
                : m_blockSize(p_size)
            {
                // Initial size: p_size
                m_blockContainer.reset(
                    static_cast<std::uint8_t*>(aligned_alloc(p_alignment, m_blockSize)));
                std::memset(m_blockContainer.get(), 0, m_blockSize);
            }

            /**
             * @brief Constructs a DynamicBuffer object.
             * @param p_buffer Pointer to the initial buffer.
             * @param p_size Size of the buffer.
             */
            DynamicAlignedBuffer(uint8_t* p_buffer, size_t p_size, size_t p_alignment = 4) noexcept
                : m_blockSize(p_size)
            {
                m_blockContainer.reset(
                    static_cast<std::uint8_t*>(aligned_alloc(p_alignment, m_blockSize)));

                std::memcpy(m_blockContainer.get(), p_buffer, m_blockSize);
            }

            /**
             * @brief Virtual destructor for DynamicBuffer.
             */
            virtual ~DynamicAlignedBuffer() noexcept = default;
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
             * @return Pointer to the buffer.
             */
            virtual std::uint8_t* getBlock() noexcept
            {
                return m_blockContainer.get();
            }

            /**
             * @brief Resets the content of the buffer to zero.
             * If resize size is less than the current size, the buffer content is reset to zero.
             */
            virtual void resetContent() noexcept
            {
                std::memset(m_blockContainer.get(), 0, m_blockSize);
            }

            /**
             * @brief Resizes the buffer to the specified size.
             * @param p_size New size of the buffer.
             */
            virtual void resizeAlloc(size_t p_size, size_t p_alignment = 4) noexcept
            {
                if (p_size < m_blockSize)
                {
                    m_blockContainer.reset(
                        static_cast<std::uint8_t*>(aligned_alloc(p_alignment, m_blockSize)));
                    std::memset(m_blockContainer.get(), 0, m_blockSize);
                }
                else
                {
                    m_blockSize = p_size;
                    uint8_t* oldContainer = m_blockContainer.release();

                    static_cast<std::uint8_t*>(
                        std::realloc(oldContainer, m_blockSize));

                    m_blockContainer.reset(oldContainer);                    
                }
            }
        };

        /**
            * @brief 
            * @param p_size New size of the buffer.
            */
        class MmappedFixedBuffer
        {
        protected:
            
            void*                                       m_block;                // Unique pointer to the memory block.
            size_t                                      m_blockSize;            // Size of the memory block.
            int                                         m_fd;
            std::string                                 m_fileName;             // Name of the file to be mmaped.

        public:

            /**
                * @brief Constructs a DynamicBuffer object.
                * @param p_size Size of the buffer.
                */
            MmappedFixedBuffer(size_t p_size, const char* p_fileName) noexcept
                : m_blockSize(p_size)
            {
                // Initial size: p_size
                //  
                m_fd = open(p_fileName, O_RDWR | O_CREAT, 0644);
                if (m_fd < 0)
                    assert(0);
                
                // Set the file size to the block size.
                if (ftruncate(m_fd, m_blockSize) != 0) {
                    close(m_fd);
                    assert(0 && "Failed to resize file");
                }
                
                // Map the file to the memory block.
                m_block = mmap(nullptr, m_blockSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
                if (m_block == MAP_FAILED)
                    assert(0);
                
                // Optional
                m_fileName = p_fileName;
                std::memset(m_block, 0, m_blockSize);
            }

            virtual ~MmappedFixedBuffer() noexcept
            {
                munmap(m_block, m_blockSize);
                if (m_fd > 0)
                    close(m_fd);
            }

            virtual size_t getSize() noexcept
            {
                return m_blockSize;
            }

            virtual void* getBlock() noexcept
            {
                return m_block;
            }

            virtual int getFd() noexcept
            {
                return m_fd;
            }

            virtual const char* getFileName() noexcept
            {
                return m_fileName.c_str();
            }

            virtual void resetContent() noexcept
            {
                std::memset(m_block, 0, m_blockSize);
            }

            // Flush the block to the file asynchronously.
            virtual bool flushBlockAsync(uint64_t p_offset = 0, uint64_t p_size = 0) noexcept
            {
                if (p_size == 0)
                    p_size = m_blockSize;

                if ((p_size + p_offset) > m_blockSize)
                    return false;

                uint64_t addr = reinterpret_cast<uint64_t>(m_block) + p_offset;

                if (msync((char*)addr, p_size, MS_ASYNC) == -1)
                    false;                              // Case when error occurred.

                return true;
            }

            virtual bool flushBlockWait() noexcept
            {
                if (fsync(m_fd) == -1)
                    return false;                       // Case when error occurred.

                return true;
            }
        };
    }
}

#endif