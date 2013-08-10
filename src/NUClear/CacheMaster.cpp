/**
 * Copyright (C) 2013 Jake Woods <jake.f.woods@gmail.com>, Trent Houliston <trent@houliston.me>
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

#include "NUClear/PowerPlant.h"

namespace NUClear {
    
    PowerPlant::CacheMaster::CacheMaster(PowerPlant* parent) :
    PowerPlant::BaseMaster(parent) {}
    
    void PowerPlant::CacheMaster::setCurrentTask(std::thread::id threadId, const Internal::Reaction::Task* args) {
        // TODO replace this with thread_local if possible (c++11 keyword thread_local)
        threadArgs[threadId] = args;
    }
    
    void PowerPlant::CacheMaster::linkCache(void* data, std::vector<std::pair<std::type_index, std::shared_ptr<void>>> args) {
        
        linkedCache.insert(std::make_pair(data, args));
    }
    
    const Internal::Reaction::Task* PowerPlant::CacheMaster::getCurrentTask(std::thread::id threadId) {
        auto task = threadArgs.find(threadId);
        
        if(task != std::end(threadArgs)) {
            return task->second;
        }
        else {
            return nullptr;
        }
    }
}
