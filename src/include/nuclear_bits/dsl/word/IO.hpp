/*
 * Copyright (C) 2013-2016 Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef NUCLEAR_DSL_WORD_IO_HPP
#define NUCLEAR_DSL_WORD_IO_HPP

#ifdef _WIN32
#include "nuclear_bits/util/windows_includes.hpp"
#else
#include <poll.h>
#endif

#include "nuclear_bits/dsl/operation/Unbind.hpp"
#include "nuclear_bits/dsl/store/ThreadStore.hpp"
#include "nuclear_bits/dsl/trait/is_transient.hpp"
#include "nuclear_bits/dsl/word/Single.hpp"
#include "nuclear_bits/dsl/word/emit/Direct.hpp"
#include "nuclear_bits/util/generate_reaction.hpp"
#include "nuclear_bits/util/platform.hpp"

namespace NUClear {
namespace dsl {
    namespace word {

        struct IOConfiguration {
            fd_t fd;
            int events;
            std::shared_ptr<threading::Reaction> reaction;
        };

        /**
         * @brief
         *  This is used to trigger reactions based on standard I/O operations.  This will work for any I/O
         *  communication which uses a file descriptor. The associated reaction is triggered when the communication
         *  line matches the descriptor.
         *
         * @details
				 *  For best use, runtime arguments should be provided, which specify the file descriptor. The generic form of
         *  this reaction is:
         *  @code on<IO>(file_descriptor) @endcode
				 *
				 *  When used, it will likely match one of the following:
         *  File reading:  triggers a reaction when the pipe/stream/communication line has data available to read.
         *  @code on<IO>(pipe/stream/comms, IO::READ) @endcode
				 *
				 *  File writing:  triggers a reaction when the pipe/stream/communication line has data to be written.
         *  @code on<IO>(pipe/stream/comms, IO::WRITE) @endcode
				 *
				 *  File close:  triggers a reaction when the pipe/stream/communication line is closed.
				 *  @code on<IO>(pipe/stream/comms, IO::CLOSE) @endcode
				 *
				 *  File error:  triggers a reaction when the pipe/stream/communication line reports an error.
         *  @code on<IO>(pipe/stream/comms, IO::ERROR) @endcode
         *
         *  Multiple states:  triggers a reaction when the pipe/stream/communication line matches multiple states.  For
         *  example:
         *  on<IO>(pipe/stream/comms, IO::READ | IO::CLOSE)
				 *
				 * @attention
				 *  Note that any reactions caused by on<IO> are implicitly single.
         *
         * @par Implements
         *  Bind
         */
        struct IO : public Single {

// On windows we use different wait events
#ifdef _WIN32
            enum EventType : short{READ = FD_READ | FD_OOB | FD_ACCEPT, WRITE = FD_WRITE, CLOSE = FD_CLOSE, ERROR = 0};
#else
            enum EventType : short { READ = POLLIN, WRITE = POLLOUT, CLOSE = POLLHUP, ERROR = POLLNVAL | POLLERR };
#endif

            struct Event {
                fd_t fd;
                int events;

                operator bool() const {
                    return fd != -1;
                }
            };

            using ThreadEventStore = dsl::store::ThreadStore<Event>;

            template <typename DSL, typename Function>
            static inline threading::ReactionHandle bind(Reactor& reactor,
                                                         const std::string& label,
                                                         Function&& callback,
                                                         fd_t fd,
                                                         int watch_set) {

                auto reaction = util::generate_reaction<DSL, IO>(reactor, label, std::forward<Function>(callback));

                auto io_config = std::make_unique<IOConfiguration>(IOConfiguration{fd, watch_set, std::move(reaction)});

                threading::ReactionHandle handle(io_config->reaction);

                // Send our configuration out
                reactor.powerplant.emit<emit::Direct>(io_config);

                // Return our handles
                return handle;
            }

            template <typename DSL>
            static inline Event get(threading::Reaction&) {

                // If our thread store has a value
                if (ThreadEventStore::value) {
                    return *ThreadEventStore::value;
                }
                // Otherwise return an invalid event
                else {
                    return Event{INVALID_SOCKET, 0};
                }
            }
        };

    }  // namespace word

    namespace trait {

        template <>
        struct is_transient<word::IO::Event> : public std::true_type {};

    }  // namespace trait
}  // namespace dsl
}  // namespace NUClear

#endif  // NUCLEAR_DSL_WORD_IO_HPP
