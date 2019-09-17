#ifndef BLACKICE_H
#define BLACKICE_H

#include "lightBringer.h"
#include "udp_frag_generated.h"

typedef std::unique_ptr<std::vector<uint8_t>> PayloadPtr;
typedef std::shared_ptr<std::vector<PayloadPtr>> VecToPayloadVecPtr;

class BlackIce {
public:
    BlackIce(void);
    explicit BlackIce(LightBringer *man);
    void Start(void);
    void Init(void);
    void SetListeningPort(uint16_t port);

    void QueueTest(void);

private:
    class ClientInfo {
    public:
        in_port_t sin_port;
        uint32_t s_addr;
        VecToPayloadVecPtr payloads;
    };
    void UdpBind(void);
    void ProcessUdpPacket(std::string clientId, PayloadPtr payload);
    void ProcessClient(std::string clientId);
    [[ noreturn ]] void ProcessBotActions(void);
    uint16_t listeningPort;
    LightBringer *manager;

    // typedef std::unique_ptr<ClientInfo> ClientInfoPtr;
    typedef std::unordered_map<std::string, VecToPayloadVecPtr> UdpClientMap;
    typedef std::unordered_map<std::string, std::string> UuidToClientId;

    // Important to how packets get processed correctly
    // A thread will be created for each client hash entry in the map
    // When the thread starts, the vector of UDP_Frag Objs must have at least 1 object in it
    // The thread inspects the first object and determines how many Fragments there will be
    // This value is used to initialize a while loop that sleeps until vector.size == total fragments
    // The thread will then put the fragment's data in order of sequence
    // The full data will be used to serialize to an ActionResponse object
    // This object will then be queued to the Manager (LightBringer) and processed there
    UdpClientMap udpClientMap;
    UuidToClientId clientIdMap;  // UUID **OR** SessionId -> Client Info (Where to send packets back)
    int sock;
};

#endif  // BLACKICE_H
