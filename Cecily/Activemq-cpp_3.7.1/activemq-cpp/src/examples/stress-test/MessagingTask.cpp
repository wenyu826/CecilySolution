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

#include "MessagingTask.h"

using namespace decaf::lang;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace cms::stress;

////////////////////////////////////////////////////////////////////////////////
ThreadPoolExecutor* MessagingTask::threadPoolExecutor = NULL;

////////////////////////////////////////////////////////////////////////////////
MessagingTask::MessagingTask(Receiver* receiver, const std::string& message) :
    receiver(receiver), message(message) {
}

////////////////////////////////////////////////////////////////////////////////
MessagingTask::~MessagingTask() {
}

////////////////////////////////////////////////////////////////////////////////
void MessagingTask::queue() {
    if (threadPoolExecutor != NULL) {
        threadPoolExecutor->execute(this);
    } else {
        run();
        delete this;
    }
}

////////////////////////////////////////////////////////////////////////////////
void MessagingTask::run() {
    try {
        if (receiver != NULL) {
            receiver->executeMessagingTask(message);
        }
    } catch (...) {
    }
}

////////////////////////////////////////////////////////////////////////////////
void MessagingTask::initializeThreads(int min, int max) {
    if (min > 0) {
        threadPoolExecutor = new ThreadPoolExecutor(min, max, 5, TimeUnit::SECONDS, new LinkedBlockingQueue<Runnable*>());
    } else {
        threadPoolExecutor = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
void MessagingTask::terminateThreads() {
    if (threadPoolExecutor != NULL) {
        threadPoolExecutor->shutdown();
        //threadPoolExecutor->awaitTermination(10000, TimeUnit::MILLISECONDS);
        threadPoolExecutor->awaitTermination(-1, TimeUnit::SECONDS);
        delete threadPoolExecutor;
        threadPoolExecutor = NULL;
    }
}
