#ifndef LIGHTBRINGER_H
#define LIGHTBRINGER_H

#include "anomaly_api_generated.h"

typedef std::unique_ptr<anomaly::APIT> APITPtr;

typedef std::queue<APITPtr> ActionResponseQueue;
typedef std::queue<APITPtr> ActionQueue;

class LightBringer {
 public:
    LightBringer();

    APITPtr PopActionResponse(void);
    void PushActionResponse(APITPtr actionResponse);
    int GetActionResponseSize(void);
    APITPtr PopAction(void);
    void PushAction(APITPtr action);
    int GetActionSize(void);

    // Testing Functions
    void BlackIceTesting(void);
    void ArsenalHttpTesting(void);

 private:
    std::vector<std::string> upstreamServers;
    ActionResponseQueue actionResponseQueue;  // Responses received from bot
    ActionQueue actionQueue;  // Actions to send bot
};

#endif  // LIGHTBRINGER_H
