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

#ifndef _DECAF_INTERNAL_NET_SSL_OPENSSL_OPENSSLSOCKETFACTORY_H_
#define _DECAF_INTERNAL_NET_SSL_OPENSSL_OPENSSLSOCKETFACTORY_H_

#include <decaf/util/Config.h>

#include <decaf/net/ssl/SSLSocketFactory.h>

namespace decaf {
namespace internal {
namespace net {
namespace ssl {
namespace openssl {

    class OpenSSLContextSpi;

    /**
     * Client Socket Factory that creates SSL based client sockets using the OpenSSL library.
     *
     * @since 1.0
     */
    class DECAF_API OpenSSLSocketFactory : public decaf::net::ssl::SSLSocketFactory {
    private:

        OpenSSLContextSpi* parent;

    private:

        OpenSSLSocketFactory( const OpenSSLSocketFactory& );
        OpenSSLSocketFactory& operator= ( const OpenSSLSocketFactory& );

    public:

        OpenSSLSocketFactory( OpenSSLContextSpi* parent );

        virtual ~OpenSSLSocketFactory();

        /**
         * {@inheritDoc}
         */
        virtual decaf::net::Socket* createSocket();

        /**
         * {@inheritDoc}
         */
        virtual decaf::net::Socket* createSocket( const decaf::net::InetAddress* host, int port );

        /**
         * {@inheritDoc}
         */
        virtual decaf::net::Socket* createSocket( const decaf::net::InetAddress* host, int port,
                                                  const decaf::net::InetAddress* ifAddress, int localPort );

        /**
         * {@inheritDoc}
         */
        virtual decaf::net::Socket* createSocket( const std::string& hostname, int port );

        /**
         * {@inheritDoc}
         */
        virtual decaf::net::Socket* createSocket( const std::string& name, int port,
                                                  const decaf::net::InetAddress* ifAddress, int localPort );

        /**
         * {@inheritDoc}
         */
        virtual std::vector<std::string> getDefaultCipherSuites();

        /**
         * {@inheritDoc}
         */
        virtual std::vector<std::string> getSupportedCipherSuites();

        /**
         * {@inheritDoc}
         */
        virtual decaf::net::Socket* createSocket( decaf::net::Socket* socket, std::string host,
                                                  int port, bool autoClose );

    };

}}}}}

#endif /* _DECAF_INTERNAL_NET_SSL_OPENSSL_OPENSSLSOCKETFACTORY_H_ */
