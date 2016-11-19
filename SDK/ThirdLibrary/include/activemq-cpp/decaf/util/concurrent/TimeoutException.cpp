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

#include "TimeoutException.h"

using namespace decaf;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace decaf::lang;

////////////////////////////////////////////////////////////////////////////////
TimeoutException::TimeoutException() : decaf::lang::Exception() {
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException::TimeoutException(const decaf::lang::Exception& ex) : decaf::lang::Exception() {
    *(Exception*) this = ex;
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException::TimeoutException(const TimeoutException& ex) : decaf::lang::Exception() {
    *(Exception*) this = ex;
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException::TimeoutException(const std::exception* cause) : decaf::lang::Exception(cause) {
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException::TimeoutException(const char* file, const int lineNumber, const char* msg, ...) : decaf::lang::Exception() {

    va_list vargs;
    va_start( vargs, msg);
    buildMessage(msg, vargs);

    // Set the first mark for this exception.
    setMark(file, lineNumber);
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException::TimeoutException(const char* file, const int lineNumber, const std::exception* cause, const char* msg, ...) : decaf::lang::Exception(cause) {

    va_list vargs;
    va_start(vargs, msg);
    buildMessage(msg, vargs);

    // Set the first mark for this exception.
    setMark(file, lineNumber);
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException::~TimeoutException() throw () {
}

////////////////////////////////////////////////////////////////////////////////
TimeoutException* TimeoutException::clone() const {
    return new TimeoutException(*this);
}
