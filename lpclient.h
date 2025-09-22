//
// Created by linhdh on 22/09/2025.
//

#ifndef ZMQ_SIMPLE_PIRATE_PATTERN_LPCLIENT_H
#define ZMQ_SIMPLE_PIRATE_PATTERN_LPCLIENT_H

#include "zhelpers.h"
#include <sstream>

#define REQUEST_TIMEOUT     2500    //  msecs, (> 1000!)
#define REQUEST_RETRIES     3       //  Before we abandon

//  Helper function that returns a new configured socket
//  connected to the Hello World server
//
static zmq::socket_t * s_client_socket (zmq::context_t & context) {
    std::cout << "I: connecting to server..." << std::endl;
    zmq::socket_t * client = new zmq::socket_t (context, ZMQ_REQ);
    client->connect ("tcp://localhost:5555");

    //  Configure socket to not wait at close time
    int linger = 0;
    client->setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
    return client;
}

#endif //ZMQ_SIMPLE_PIRATE_PATTERN_LPCLIENT_H