#ifndef LIGHTBRINGER_H
#define LIGHTBRINGER_H

#include "anomaly_api_generated.h"

class LightBringer {
public:
    LightBringer();

    void BlackIceTesting(void);

    std::queue<std::unique_ptr<anomaly::ActionResponseT>> actionResponseQueue; // Responses received from bot
    std::queue<std::unique_ptr<anomaly::ActionT>> actionQueue; // Actions to send bot

private:
    std::vector<std::string> upstreamServers;
};

#endif // LIGHTBRINGER_H