#ifndef LIGHTBRINGER_H
#define LIGHTBRINGER_H

#include "anomaly_api_generated.h"

typedef std::queue<std::unique_ptr<anomaly::ActionResponseT>> ActionResponseQueue;
typedef std::unique_ptr<anomaly::ActionResponseT> ActionResponsePtr;

typedef std::queue<std::unique_ptr<anomaly::ActionT>> ActionQueue;
typedef std::unique_ptr<anomaly::ActionT> ActionPtr;

class LightBringer {
 public:
    LightBringer();

    ActionResponsePtr PopActionResponse(void);
    void PushActionResponse(ActionResponsePtr actionResponse);
    ActionPtr PopAction(void);
    void PushAction(ActionPtr action);

    void BlackIceTesting(void);

 private:
    std::vector<std::string> upstreamServers;
    ActionResponseQueue actionResponseQueue;  // Responses received from bot
    ActionQueue actionQueue;  // Actions to send bot
};

#endif  // LIGHTBRINGER_H
