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

#include <activemq/wireformat/openwire/marshal/generated/ProducerAckMarshaller.h>

#include <activemq/commands/ProducerAck.h>
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
DataStructure* ProducerAckMarshaller::createObject() const {
    return new ProducerAck();
}

///////////////////////////////////////////////////////////////////////////////
unsigned char ProducerAckMarshaller::getDataStructureType() const {
    return ProducerAck::ID_PRODUCERACK;
}

///////////////////////////////////////////////////////////////////////////////
void ProducerAckMarshaller::tightUnmarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn, BooleanStream* bs) {

    try {

        BaseCommandMarshaller::tightUnmarshal(wireFormat, dataStructure, dataIn, bs);

        ProducerAck* info =
            dynamic_cast<ProducerAck*>(dataStructure);

        int wireVersion = wireFormat->getVersion();

        if (wireVersion >= 3) {
            info->setProducerId(Pointer<ProducerId>(dynamic_cast<ProducerId* >(
                tightUnmarshalNestedObject(wireFormat, dataIn, bs))));
        }
        if (wireVersion >= 3) {
            info->setSize(dataIn->readInt());
        }
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
int ProducerAckMarshaller::tightMarshal1(OpenWireFormat* wireFormat, DataStructure* dataStructure, BooleanStream* bs) {

    try {

        ProducerAck* info =
            dynamic_cast<ProducerAck*>(dataStructure);

        int rc = BaseCommandMarshaller::tightMarshal1(wireFormat, dataStructure, bs);

        int wireVersion = wireFormat->getVersion();

        if (wireVersion >= 3) {
            rc += tightMarshalNestedObject1(wireFormat, info->getProducerId().get(), bs);
        }
        if (wireVersion >= 3) {
        }

        return rc + 4;
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void ProducerAckMarshaller::tightMarshal2(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut, BooleanStream* bs) {

    try {

        BaseCommandMarshaller::tightMarshal2(wireFormat, dataStructure, dataOut, bs );

        ProducerAck* info =
            dynamic_cast<ProducerAck*>(dataStructure);

        int wireVersion = wireFormat->getVersion();

        if (wireVersion >= 3) {
            tightMarshalNestedObject2(wireFormat, info->getProducerId().get(), dataOut, bs);
        }
        if (wireVersion >= 3) {
            dataOut->writeInt(info->getSize());
        }
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT( exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void ProducerAckMarshaller::looseUnmarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataInputStream* dataIn) {

    try {

        BaseCommandMarshaller::looseUnmarshal(wireFormat, dataStructure, dataIn);
        ProducerAck* info =
            dynamic_cast<ProducerAck*>(dataStructure);

        int wireVersion = wireFormat->getVersion();

        if (wireVersion >= 3) {
            info->setProducerId(Pointer<ProducerId>(dynamic_cast<ProducerId*>(
                looseUnmarshalNestedObject(wireFormat, dataIn))));
        }
        if (wireVersion >= 3) {
            info->setSize(dataIn->readInt());
        }
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

///////////////////////////////////////////////////////////////////////////////
void ProducerAckMarshaller::looseMarshal(OpenWireFormat* wireFormat, DataStructure* dataStructure, DataOutputStream* dataOut) {

    try {

        ProducerAck* info =
            dynamic_cast<ProducerAck*>(dataStructure);
        BaseCommandMarshaller::looseMarshal(wireFormat, dataStructure, dataOut);

        int wireVersion = wireFormat->getVersion();

        if (wireVersion >= 3) {
            looseMarshalNestedObject(wireFormat, info->getProducerId().get(), dataOut);
        }
        if (wireVersion >= 3) {
            dataOut->writeInt(info->getSize());
        }
    }
    AMQ_CATCH_RETHROW(decaf::io::IOException)
    AMQ_CATCH_EXCEPTION_CONVERT(exceptions::ActiveMQException, decaf::io::IOException)
    AMQ_CATCHALL_THROW(decaf::io::IOException)
}

