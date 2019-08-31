#include <iostream>
#include <random>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "anomaly_api_generated.h"
#include "udp_frag_generated.h"

#define MAX_UDP_FRAG_SIZE 1000

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

APIT InitBeacon(std::string server) {
    APIT initBeacon;

    initBeacon.sessionId = ""; // Blank when initializing
    initBeacon.uuid = GetRandString(16);
    initBeacon.hostname = GetRandString(8);

    initBeacon.config = std::make_unique<anomaly::ConfigT>();
    initBeacon.config->agentVersion = "BlackIce-Test-Client";
    initBeacon.config->interval = 5;
    initBeacon.config->intervalDelta = 1;
    initBeacon.config->servers = { server };

    return initBeacon;
}

void SendUdpBeacon(int sock, sockaddr_in serverAddr, APIT beacon) {
    flatbuffers::FlatBufferBuilder beaconBuilder;
	flatbuffers::FlatBufferBuilder udpSendBuilder;
    UdpFragT udpSendFrag;
    flatbuffers::Offset<UdpFrag> tempUdpFrag;
    int ret;

    beaconBuilder.Finish(CreateAPI(beaconBuilder, &beacon));

    uint8_t *beacPtr = beaconBuilder.GetBufferPointer();
    int beacSize = beaconBuilder.GetSize();

    std::vector<uint8_t> rawBeacon(beacPtr, beacPtr + beacSize);

    int i = 0;
	int offset = 0;
	while (static_cast<size_t>(offset + MAX_UDP_FRAG_SIZE) <= rawBeacon.size()) {
		udpSendFrag.data.assign(rawBeacon.begin() + offset, rawBeacon.begin() + offset + MAX_UDP_FRAG_SIZE);
		i++;
		offset = i * MAX_UDP_FRAG_SIZE;

		tempUdpFrag = CreateUdpFrag(udpSendBuilder, &udpSendFrag);
		udpSendBuilder.Finish(tempUdpFrag);

		std::cout << "UDP Buffer Size: " << udpSendBuilder.GetSize() << std::endl;

        ret = sendto(sock, udpSendBuilder.GetBufferPointer(), udpSendBuilder.GetSize(), 0, (sockaddr *) &serverAddr, sizeof(serverAddr));

		udpSendFrag.sequenceNum++;
		udpSendBuilder.Clear();
	}
	int remain = rawBeacon.size() % MAX_UDP_FRAG_SIZE;
	if (remain > 0) {
		udpSendFrag.data.assign(rawBeacon.begin() + offset, rawBeacon.end());

		tempUdpFrag = CreateUdpFrag(udpSendBuilder, &udpSendFrag);
		udpSendBuilder.Finish(tempUdpFrag);

		ret = sendto(sock, udpSendBuilder.GetBufferPointer(), udpSendBuilder.GetSize(), 0, (sockaddr *) &serverAddr, sizeof(serverAddr));
	}
}

int main() {
    std::cout << "BlackIce Test" << std::endl;

    int sock, ret;
    struct sockaddr_in servAddr;

    // Initialize C2 Server Info
    std::string server = "127.0.0.1";
    int serverPort = 20000;

    // Initialize UDP Socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket: " << sock << std::endl;
        return 1;
    }
    std::cout << "Successfully create socket." << std::endl;

    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(serverPort);
    servAddr.sin_addr.s_addr = inet_addr(server.c_str());

    // Create Initialization Beacon
    APIT beacon = InitBeacon(server);

    //do {
        SendUdpBeacon(sock, servAddr, std::move(beacon));

    //} while(1);

    return 0;
}