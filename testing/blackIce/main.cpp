#include <iostream>
#include <random>
#include <string>

#include <sys/socket.h>

#include "anomaly_api_generated.h"

using anomaly::APIT;

std::string GetRandString(int len) {
    std::mt19937 generator{std::random_device{}()};
   //modify range according to your need "A-Z","a-z" or "0-9" or whatever you need.
    std::uniform_int_distribution<int> distribution{'a', 'z'};

    std::string rand_str(len, '\0');
    for(auto& dis: rand_str)
        dis = distribution(generator);

    return rand_str;
}

APIT InitUdpSocket() {

}

APIT InitBeacon(std::vector<std::string> servers) {
    APIT initBeacon;

    initBeacon.sessionId = ""; // Blank when initializing
    initBeacon.uuid = GetRandString(16);
    initBeacon.hostname = GetRandString(8);

    initBeacon.config = std::make_unique<anomaly::ConfigT>();
    initBeacon.config->agentVersion = "BlackIce-Test-Client";
    initBeacon.config->interval = 5;
    initBeacon.config->intervalDelta = 1;
    initBeacon.config->servers = servers;

    return initBeacon;
}

void SendUdpBeacon(int sock, APIT beacon) {

}

int main() {
    std::cout << "BlackIce Test" << std::endl;

    int sock, ret;
    struct sockaddr_in servAddr;

    // Initialize C2 Server Info
    std::vector<std::string> servers = {"127.0.0.1"};
    int serverPort = 20000;

    APIT beacon = InitBeacon(servers);

    // Initialize UDP Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::err << "Error creating socket: " << sock << std::endl;
        return 1;
    }

    memset((char *) servAddr, 0, sizeof(servaddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(serverPort);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    ret = connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (ret < 0) {
        std::err << "Error connecting to server: " << ret << std::endl;
        return 1;
    }

    do {
        SendUdpBeacon(sock, beacon);

    } while(1);

    return 0;
}