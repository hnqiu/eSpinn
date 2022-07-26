/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once
#include <iostream>
// #include <boost/circular_buffer.hpp>

/* @brief: circular buffer
 * generic data struct
 */
namespace eSpinn {
    template <typename T>
    class CircularBuffer {
        /* @brief: overloaded << 
         * print buffer elements
         */
        friend std::ostream& operator<<(std::ostream &os, const CircularBuffer &b) {
            if (!b.is_full()) {
                for (std::size_t i = 0; i < b.last; ++i) {
                    os << b.buffer[i] << " ";
                }
            }
            else {
                for (std::size_t i = b.last; i < b.cap(); ++i) {
                    os << b.buffer[i] << " ";
                }
                for (std::size_t i = 0; i < b.last; ++i) {
                    os << b.buffer[i] << " ";
                }
            }
            // os << std::endl;
            return os;
        }
    protected:
        /* data */
        const std::size_t buf_cap;
        std::size_t buf_size;
        T *buffer;
        std::size_t last;
    public:
        /* @brief: constructor */
        CircularBuffer(const std::size_t n) : 
            buf_cap(n), buf_size(0), buffer(new T[n]),
            last(0) 
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Create circular buffer, size of " << n << std::endl;
            #endif
        }

        /* @brief: destructor */
        virtual ~CircularBuffer() {
            delete [] buffer;
            #ifdef ESPINN_VERBOSE
            std::cout << "Circular buffer deleted." << std::endl;
            #endif
        }

        /* @brief: push an element */
        void push(const T e) {
            buffer[last] = e;
            if (buf_size < buf_cap)
                ++buf_size;

            if (last != buf_cap-1) {
                ++last;
            }
            else {
                last = 0;
            }
        }

        /* @brief: check if buffer is full */
        const bool is_full() const {
            if (buf_size == buf_cap)
                return true;
            return false;
        }

        /* @brief: return the size of buffer */
        const std::size_t size() const {
            return buf_size;
        }

        /* @brief: return the capacity of buffer */
        const std::size_t cap() const {
            return buf_cap;
        }
    };
}
