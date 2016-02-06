/**
 * SIGMusic Lights 2015
 * Websocket server class
 */

#include <string>
#include <iostream>
#include <algorithm>
#include <arpa/inet.h>
#include "server.h"
#include "manager.h"


using websocketpp::connection_hdl;
using namespace websocketpp::frame;
using std::cout;
using std::endl;
using std::string;
using std::vector;


#define WS_PROTOCOL_NAME        "nlcp" // Networked Lights Control Protocol
#define WS_PORT                 7446


ws_server * Server::ws;

color_t* Server::colors;
sem_t* Server::colors_sem;
uint32_t* Server::connected;
sem_t* Server::connected_sem;


Server::Server() {

    ws = new ws_server();
    ws->set_message_handler(&onMessage);
    ws->set_validate_handler(&shouldConnect);
}

void Server::run(color_t* colors, sem_t* colors_sem,
    uint32_t* connected, sem_t* connected_sem) {
    
    Server::colors = colors;
    Server::colors_sem = colors_sem;
    Server::connected = connected;
    Server::connected_sem = connected_sem;

    ws->init_asio();
    ws->listen(WS_PORT);
    ws->start_accept();

    cout << "Running websocket event loop" << endl;
    ws->run();
}

void Server::onMessage(connection_hdl client, ws_server::message_ptr msg) {
    processMessage(client, msg->get_payload());
}

bool Server::shouldConnect(connection_hdl client) {

    // Get the connection so we can get info about it
    ws_server::connection_ptr connection = ws->get_con_from_hdl(client);

    // Figure out if the client knows the protocol
    vector<string> p = connection->get_requested_subprotocols();
    if (std::find(p.begin(), p.end(), WS_PROTOCOL_NAME) != p.end()) {
        // Tell the client we're going to use this protocol
        connection->select_subprotocol(WS_PROTOCOL_NAME);
        return true;
    } else {
        return false;
    }
}

void Server::processMessage(connection_hdl client, const std::string message) {

    uint8_t id, r, g, b;

    if (!message.compare(0, 7, "setrgb ")) {
        
        if (sscanf(message.c_str(), "setrgb %hhu %hhu %hhu %hhu", &id, &r, &g, &b) == 4) {

            sem_wait(colors_sem);
            colors[id].r = r;
            colors[id].g = g;
            colors[id].b = b;
            sem_post(colors_sem);

            ws->send(client, "OK", opcode::text);

        } else {
            // One or more arguments were invalid
            ws->send(client, "Error: invalid arguments", opcode::text);
        }

    } else if (!message.compare("list")) {
        
        string list = "";

        sem_wait(connected_sem);
        
        // Get the list of connected lights
        for (int i = 0; i < NUM_IDS; i++) {

            if (isLightConnected(i)) {
                char num[4];
                sprintf(num, "%d,", i);
                list.append(num);
            }
        }

        sem_post(connected_sem);

        ws->send(client, list, opcode::text);

    } else if (!message.compare(0, 4, "ping")) {
        // Echo the message - useful for measuring roundtrip latency
        ws->send(client, message, opcode::text);

    } else {
        ws->send(client, "Error: unrecognized command", opcode::text);
    }
}

bool Server::isLightConnected(uint8_t id) {

    int index = id / sizeof(uint32_t);
    int bit = id % sizeof(uint32_t);

    uint32_t flags = connected[index];
    uint32_t mask = 1 << bit;

    return (flags & mask);
}
