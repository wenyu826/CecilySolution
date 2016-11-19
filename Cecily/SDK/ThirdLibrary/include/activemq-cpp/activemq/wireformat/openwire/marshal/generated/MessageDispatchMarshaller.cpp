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

#include <activemq/wireformat/openwire/marshal/generated/MessageDispatchMarshaller.h>

#include <activemq/commands/MessageDispatch.h>
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
DataStructure* MessageDispatchMarshaller::createObject() const {
    return new MessageDispatch();
}

///////////////////////////////////////////////////////////////////////////////
unsigned char MessageDispatchMarshaller::getDataStructureType() const {
    return MessageDispatch::ID_MESSAGEDISPATCH;
}

///////////////////////////////////////////////////////////////////////////////
void MessageDispatchMarshaller::tightUnmarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs) {

    try {

        BaseCommandMarshaller::tightUnmarshal(wireFormat, dataStructure, dataIn, bs);

        MessageDispatch* info =
            dynamic_cast<MessageDispatch*>(dataStructure);
        info->setConsumerId(Pointer<ConsumerId>(dynamic_cast<ConsumerId* >(
            tightUnmarshalCachedObject(wireFormat, dataIn, bs))));
        info->setDestination(Pointer<ActiveMQDestination>(dynamic_cast<ActiveMQDestination* >(
            tightUnmarshalCachedObject(wireFormat, dataIn, bs))));
        info->setMessage(Pointer<Message>(dynamic_cast<Message* >(
            tightUnmarshalNestedObject(wireFormat, dataIn, bs))));
        info->setRedeliveryCounter(dataIn->readInt());
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
int MessageDispatchMarshaller::tightMarshal1(OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs) {

    try {

        MessageDispatch* info =
            dynamic_cast<MessageDispatch*>(dataStructure);

        int rc = BaseCommandMarshaller::tightMarshal1(wireFormat, dataStructure, bs);
        rc += tightMarshalCachedObject1(wireFormat, info->getConsumerId().get(), bs);
        rc += tightMarshalCachedObject1(wireFormat, info->getDestination().get(), bs);
        rc += tightMarshalNestedObject1(wireFormat, info->getMessage().get(), bs);

        return rc + 4;
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void MessageDispatchMarshaller::tightMarshal2(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs) {

    try {

        BaseCommandMarshaller::tightMarshal2(wireFormat, dataStructure, dataOut, bs );

        MessageDispatch* info =
            dynamic_cast<MessageDispatch*>(dataStructure);
        tightMarshalCachedObject2(wireFormat, info->getConsumerId().get(), dataOut, bs);
        tightMarshalCachedObject2(wireFormat, info->getDestination().get(), dataOut, bs);
        tightMarshalNestedObject2(wireFormat, info->getMessage().get(), dataOut, bs);
        dataOut->writeInt(info->getRedeliveryCounter());
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void MessageDispatchMarshaller::looseUnmarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn) {

    try {

        BaseCommandMarshaller::looseUnmarshal(wireFormat, dataStructure, dataIn);
        MessageDispatch* info =
            dynamic_cast<MessageDispatch*>(dataStructure);
        info->setConsumerId(Pointer<ConsumerId>(dynamic_cast<ConsumerId*>(
            looseUnmarshalCachedObject(wireFormat, dataIn))));
        info->setDestination(Pointer<ActiveMQDestination>(dynamic_cast<ActiveMQDestination*>(
            looseUnmarshalCachedObject(wireFormat, dataIn))));
        info->setMessage(Pointer<Message>(dynamic_cast<Message*>(
            looseUnmarshalNestedObject(wireFormat, dataIn))));
        info->setRedeliveryCounter(dataIn->readInt());
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void MessageDispatchMarshaller::looseMarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut) {

    try {

        MessageDispatch* info =
            dynamic_cast<MessageDispatch*>(dataStructure);
        BaseCommandMarshaller::looseMarshal(wireFormat, dataStructure, dataOut);
        looseMarshalCachedObject(wireFormat, info->getConsumerId().get(), dataOut);
        looseMarshalCachedObject(wireFormat, info->getDestination().get(), dataOut);
        looseMarshalNestedObject(wireFormat, info->getMessage().get(), dataOut);
        dataOut->writeInt(info->getRedeliveryCounter());
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

