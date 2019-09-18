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
#define BUFF_SIZE 2000

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

std::unique_ptr<APIT> InitBeacon(std::string server) {
    std::unique_ptr<APIT> initBeacon = std::make_unique<APIT>();

    initBeacon->sessionId = ""; // Blank when initializing
    initBeacon->uuid = GetRandString(16);
    initBeacon->hostname = GetRandString(8);

    initBeacon->config = std::make_unique<anomaly::ConfigT>();
    initBeacon->config->agentVersion = "BlackIce-Test-Client";
    initBeacon->config->interval = 5;
    initBeacon->config->intervalDelta = 1;
    initBeacon->config->servers = {server};

    return initBeacon;
}

void SendUdpBeacon(int sock, sockaddr_in serverAddr, APIT *beacon) {
    flatbuffers::FlatBufferBuilder beaconBuilder;
    flatbuffers::FlatBufferBuilder udpSendBuilder;
    UdpFragT udpSendFrag;
    flatbuffers::Offset<UdpFrag> tempUdpFrag;
    int ret;

    beaconBuilder.Finish(CreateAPI(beaconBuilder, beacon));

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

std::unique_ptr<APIT> ProcessServerAction(std::unique_ptr<APIT> serverAction) {
    std::unique_ptr<APIT> response = std::make_unique<APIT>();
    std::unique_ptr<anomaly::ActionT> currentAction;
    std::unique_ptr<anomaly::ActionResponseT> actionResponse;

    response->sessionId = serverAction->sessionId;

    if (serverAction->actions.size() == 0) {
        return response;
    }

    for (int i=0; i<serverAction->actions.size(); i++) {
        using anomaly::ActionType;

        // Initialize Action Response object now that we have an Action
		actionResponse = std::make_unique<anomaly::ActionResponseT>();

        // Process Action Object
		switch (currentAction->actionType) {
		case ActionType::ActionType_Config: // Config Change
			std::cout << "ActionType 0: Config Change" << std::endl;

			actionResponse->actionId = currentAction->actionId;
			actionResponse->config = std::move(currentAction->config);
			break;
		case ActionType::ActionType_Exec: // Execute Command
			std::cout << "ActionType 1: Execute Command" << std::endl;

            actionResponse->actionId = currentAction->actionId;
            actionResponse->startTime = "now";
            actionResponse->endTime = "quickly";
            actionResponse->stdoutput = "Executed: " + currentAction->command;
            actionResponse->stderror = "";
            actionResponse->error = false;
			break;
		case ActionType::ActionType_Spawn: // Spawn Process
			std::cout << "ActionType 2: Spawn Process" << std::endl;
			std::cout << "NOT YET IMPLEMENTED" << std::endl;
			break;
		case ActionType::ActionType_TimedExec: // Timed Command Execution
			std::cout << "ActionType 3: Timed Execution" << std::endl;
			std::cout << "NOT YET IMPLEMENTED" << std::endl;
			break;
		case ActionType::ActionType_TimedSpawn: // Timed Spawn Process
			std::cout << "ActionType 4: Timed Spawn Process" << std::endl;
			std::cout << "NOT YET IMPLEMENTED" << std::endl;
			break;
		case ActionType::ActionType_Upload: // Upload File
			std::cout << "ActionType 5: Upload File" << std::endl;
			std::cout << "NOT YET IMPLEMENTED" << std::endl;
			break;
		case ActionType::ActionType_Download: // Download File
			std::cout << "ActionType 6: Download File" << std::endl;
			std::cout << "NOT YET IMPLEMENTED" << std::endl;
			break;
		case ActionType::ActionType_Gather: // Gather Facts
			std::cout << "ActionType 7: Gather Facts" << std::endl;
			std::cout << "NOT YET IMPLEMENTED" << std::endl;
			break;
		case ActionType::ActionType_Reset: // Reset / Reinitializa
			std::cout << "ActionType 999: Reset / Reinitialize" << std::endl;
			break;
		default:
			std::cout << "Reached Default Condiiton.  NOT GOOD" << std::endl;
			break;
		}

		// Push actionResponse onto the response
		response->responses.push_back(std::move(actionResponse));
    }

    return response;
}

int main() {
    std::cout << "BlackIce Test" << std::endl;

    int sock, ret, len;
    struct sockaddr_in servAddr;
    char buff[BUFF_SIZE];
    std::string sessionId;

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
    std::unique_ptr<APIT> beacon = InitBeacon(server);
    std::vector<uint8_t> payload, rawAPIT;
    const UdpFrag *frag;
    std::unique_ptr<APIT> apit;

    // Send Initialization beacon
    SendUdpBeacon(sock, servAddr, beacon.get());

    do {
        // Wait until we receive a packet
        ret = recvfrom(sock, (char *) buff, BUFF_SIZE, MSG_WAITALL, (struct sockaddr *) &servAddr, (socklen_t *) &len);
        std::cout << "Received packet." << std::endl;

        // Copy Recieve buffer into payloadresponse.sess vector
        payload.assign(&buff[0], &buff[0] + ret);

        // Verify this is our FlatBuffer payload
        flatbuffers::Verifier verifyUdpFrag(payload.data(), payload.size());
        if (!verifyUdpFrag.VerifyBuffer<UdpFrag>()) {
            std::cout << "UdpFrag Verify Failed" << std::endl;
            
            // Clear payloads
            payload.clear();
            rawAPIT.clear();
            continue;
        }

        std::cout << "UdpFrag Verify Successful" << std::endl;

        // Convert payload of UdpFrag into APIT object
        frag = flatbuffers::GetRoot<UdpFrag>(payload.data());
        rawAPIT.insert(rawAPIT.end(), frag->data()->begin(), frag->data()->end());

        flatbuffers::Verifier verifyAPIT(rawAPIT.data(), rawAPIT.size());
        if(!verifyAPIT.VerifyBuffer<anomaly::API>()) {
            std::cout << "APIT Verify Failed" << std::endl;
            
            // Clear payloads
            payload.clear();
            rawAPIT.clear();
            continue;
        }

        apit = anomaly::UnPackAPI(rawAPIT.data());

        sessionId = apit->sessionId;

        beacon = ProcessServerAction(std::move(apit));
        
        // Clear payloads
        payload.clear();
        rawAPIT.clear();
        
    } while(1);

    return 0;
}