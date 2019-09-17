#ifndef ARSENALHTTP_H
#define ARSENALHTTP_H

#include "lightBringer.h"
#include "anomaly_api_generated.h"
#include "json.hpp"

class ArsenalHttp {
public:
    ArsenalHttp(void);
    explicit ArsenalHttp(LightBringer* man);

    void Init(void);
    [[ noreturn ]] void Start(void);

private:
    void APIT_To_ArsenalJson(nlohmann::json& j, const anomaly::APIT& a);
    void APIT_From_ArsenalJson(const nlohmann::json& j, anomaly::APIT& a);
    LightBringer *manager;
};

#endif  // ARSENALHTTP_H
