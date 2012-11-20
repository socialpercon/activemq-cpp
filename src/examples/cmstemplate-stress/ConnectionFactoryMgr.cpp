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

#include "ConnectionFactoryMgr.h"
#include <activemq/core/ActiveMQConnectionFactory.h>

using namespace std;
using namespace cms;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace activemq::core;
using namespace cmstemplate;

////////////////////////////////////////////////////////////////////////////////
StlMap<std::string, ConnectionFactory*> * ConnectionFactoryMgr::m_connectionFactories;

////////////////////////////////////////////////////////////////////////////////
ConnectionFactoryMgr::ConnectionFactoryMgr() {
}

////////////////////////////////////////////////////////////////////////////////
ConnectionFactoryMgr::~ConnectionFactoryMgr() {
    try {
        UnInitialize();
    } catch(...) {
    }
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionFactoryMgr::Initialize() {
    m_connectionFactories = new StlMap<std::string, ConnectionFactory*>();
}

////////////////////////////////////////////////////////////////////////////////
void ConnectionFactoryMgr::UnInitialize() {
    m_connectionFactories->lock();

    Pointer<Iterator<ConnectionFactory*> > iter(m_connectionFactories->values().iterator());
    while (iter->hasNext()) {
        ConnectionFactory* connectionFactory = iter->next();
        if (connectionFactory != NULL) {
            delete connectionFactory;
            connectionFactory = NULL;
        }
    }
    m_connectionFactories->clear();

    m_connectionFactories->unlock();

    delete m_connectionFactories;
    m_connectionFactories = NULL;
}

////////////////////////////////////////////////////////////////////////////////
ConnectionFactory* ConnectionFactoryMgr::GetConnectionFactory(const std::string& url) {
    ConnectionFactory* connectionFactory = NULL;

    m_connectionFactories->lock();
    try {
        connectionFactory = m_connectionFactories->get(url);
    } catch (NoSuchElementException& ex) {
    }

    if (!connectionFactory) {
        connectionFactory = new ActiveMQConnectionFactory(url);
        m_connectionFactories->put(url, connectionFactory);
    }
    m_connectionFactories->unlock();

    return connectionFactory;
}