/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Executors.h"

#include <decaf/lang/Exception.h>
#include <decaf/lang/Pointer.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>
#include <decaf/util/concurrent/ThreadPoolExecutor.h>
#include <decaf/util/concurrent/ThreadFactory.h>
#include <decaf/util/concurrent/TimeUnit.h>
#include <decaf/util/concurrent/LinkedBlockingQueue.h>

using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::util::concurrent::atomic;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace {

    class DefaultThreadFactory : public ThreadFactory {
    public:

        static AtomicInteger* poolNumber;

    private:

        //ThreadGroup group;
        AtomicInteger threadNumber;
        std::string namePrefix;

    public:

        DefaultThreadFactory() : ThreadFactory(), threadNumber(1), namePrefix() {

            if(DefaultThreadFactory::poolNumber == NULL) {
                throw NullPointerException();
            }

            namePrefix = std::string("pool-") +
                         Integer::toString(poolNumber->getAndIncrement()) +
                         "-thread-";
        }

        Thread* newThread(Runnable* task) {
            Thread* thread = new Thread(task, namePrefix + Integer::toString(threadNumber.getAndIncrement()));

            if (thread->getPriority() != Thread::NORM_PRIORITY) {
                thread->setPriority(Thread::NORM_PRIORITY);
            }

            return thread;
        }
    };

    AtomicInteger* DefaultThreadFactory::poolNumber = NULL;
}

////////////////////////////////////////////////////////////////////////////////
Executors::Executors() {
}

////////////////////////////////////////////////////////////////////////////////
Executors::~Executors() {
}

////////////////////////////////////////////////////////////////////////////////
void Executors::initialize() {
    DefaultThreadFactory::poolNumber = new AtomicInteger(1);
}

////////////////////////////////////////////////////////////////////////////////
void Executors::shutdown() {
    delete DefaultThreadFactory::poolNumber;
}

////////////////////////////////////////////////////////////////////////////////
ThreadFactory* Executors::getDefaultThreadFactory() {
    return new DefaultThreadFactory();
}

////////////////////////////////////////////////////////////////////////////////
ExecutorService* Executors::newFixedThreadPool(int nThreads) {

    Pointer< BlockingQueue<Runnable*> > backingQ;

    try{

        backingQ.reset(new LinkedBlockingQueue<Runnable*>());
        ExecutorService* service = new ThreadPoolExecutor(
            nThreads, nThreads, 0, TimeUnit::MILLISECONDS, backingQ.get());

        backingQ.release();

        return service;

    } catch(NullPointerException& ex) {
        ex.setMark(__FILE__, __LINE__);
        throw ex;
    } catch(IllegalArgumentException& ex) {
        ex.setMark(__FILE__, __LINE__);
        throw ex;
    } catch(Exception& ex) {
        ex.setMark(__FILE__, __LINE__);
        throw ex;
    } catch(...) {
        throw Exception();
    }
}

////////////////////////////////////////////////////////////////////////////////
ExecutorService* Executors::newFixedThreadPool(int nThreads, ThreadFactory* threadFactory) {

    Pointer< BlockingQueue<Runnable*> > backingQ;

    try{

        backingQ.reset(new LinkedBlockingQueue<Runnable*>());
        ExecutorService* service = new ThreadPoolExecutor(
            nThreads, nThreads, 0, TimeUnit::MILLISECONDS, backingQ.get(), threadFactory);

        backingQ.release();

        return service;

    } catch(NullPointerException& ex) {
        ex.setMark(__FILE__, __LINE__);
        throw ex;
    } catch(IllegalArgumentException& ex) {
        ex.setMark(__FILE__, __LINE__);
        throw ex;
    } catch(Exception& ex) {
        ex.setMark(__FILE__, __LINE__);
        throw ex;
    } catch(...) {
        throw Exception();
    }
}
