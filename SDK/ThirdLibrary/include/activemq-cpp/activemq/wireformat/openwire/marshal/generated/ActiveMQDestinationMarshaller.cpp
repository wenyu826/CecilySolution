/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <activemq/wireformat/openwire/marshal/generated/ActiveMQDestinationMarshaller.h>

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <decaf/lang/Pointer.h>

//
//     NOTE!: This file is autogenerated - do not modify!
//            if you need to make a change, please see the Java Classes in the
//            activemq-core module
//

using namespace std;
using namespace activemq;
using namespace activemq::exceptions;
using namespace activemq::commands;
using namespace activemq::wireformat;
using namespace activemq::wireformat::openwire;
using namespace activemq::wireformat::openwire::marshal;
using namespace activemq::wireformat::openwire::utils;
using namespace activemq::wireformat::openwire::marshal::generated;
using namespace decaf;
using namespace decaf::io;
using namespace decaf::lang;

///////////////////////////////////////////////////////////////////////////////
void ActiveMQDestinationMarshaller::tightUnmarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs) {

    try {

        BaseDataStreamMarshaller::tightUnmarshal(wireFormat, dataStructure, dataIn, bs);

        ActiveMQDestination* info =
            dynamic_cast<ActiveMQDestination*>(dataStructure);
        info->setPhysicalName(tightUnmarshalString(dataIn, bs));
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
int ActiveMQDestinationMarshaller::tightMarshal1(OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs) {

    try {

        ActiveMQDestination* info =
            dynamic_cast<ActiveMQDestination*>(dataStructure);

        int rc = BaseDataStreamMarshaller::tightMarshal1(wireFormat, dataStructure, bs);
        rc += tightMarshalString1(info->getPhysicalName(), bs);

        return rc + 0;
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void ActiveMQDestinationMarshaller::tightMarshal2(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs) {

    try {

        BaseDataStreamMarshaller::tightMarshal2(wireFormat, dataStructure, dataOut, bs );

        ActiveMQDestination* info =
            dynamic_cast<ActiveMQDestination*>(dataStructure);
        tightMarshalString2(info->getPhysicalName(), dataOut, bs);
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void ActiveMQDestinationMarshaller::looseUnmarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn) {

    try {

        BaseDataStreamMarshaller::looseUnmarshal(wireFormat, dataStructure, dataIn);
        ActiveMQDestination* info =
            dynamic_cast<ActiveMQDestination*>(dataStructure);
        info->setPhysicalName(looseUnmarshalString(dataIn));
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void ActiveMQDestinationMarshaller::looseMarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut) {

    try {

        ActiveMQDestination* info =
            dynamic_cast<ActiveMQDestination*>(dataStructure);
        BaseDataStreamMarshaller::looseMarshal(wireFormat, dataStructure, dataOut);
        looseMarshalString(info->getPhysicalName(), dataOut);
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

