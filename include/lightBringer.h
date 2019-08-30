#ifndef LIGHTBRINGER_H
#define LIGHTBRINGER_H

#include "anomaly_api_generated.h"

typedef std::unique_ptr<anomaly::APIT> APITPtr;

typedef std::queue<APITPtr> ActionResponseQueue;
typedef std::queue<APITPtr> ActionQueue;

class LightBringer {
 public:
    LightBringer();

    void InitCli();

    APITPtr PopActionResponse(void);
    void PushActionResponse(APITPtr actionResponse);
    int GetActionResponseSize(void);
    APITPtr PopAction(void);
    void PushAction(APITPtr action);
    int GetActionSize(void);

 private:
    std::vector<std::string> upstreamServers;
    ActionResponseQueue actionResponseQueue;  // Responses received from bot
    ActionQueue actionQueue;  // Actions to send bot

    // Mutexes
    std::mutex actionResponseQueueLock;
    std::mutex actionQueueLock;
};

#endif  // LIGHTBRINGER_H
