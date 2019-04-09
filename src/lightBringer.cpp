#include "pch.h"
#include "lightBringer.h"
#include "blackIce.h"
#include "arsenalHttp.h"

LightBringer::LightBringer() {}

APITPtr LightBringer::PopActionResponse() {
    APITPtr temp = std::move(this->actionResponseQueue.front());
    this->actionResponseQueue.pop();

    return std::move(temp);
}

void LightBringer::PushActionResponse(APITPtr actionResponse) {
    this->actionResponseQueue.push(std::move(actionResponse));
}

APITPtr LightBringer::PopAction(void) {
    APITPtr temp = std::move(this->actionQueue.front());
    this->actionQueue.pop();

    return std::move(temp);
}

void LightBringer::PushAction(APITPtr action) {
    this->actionQueue.push(std::move(action));
}

void LightBringer::BlackIceTesting() {
    BlackIce blackIce(this);
    blackIce.Start();
}

void LightBringer::ArsenalHttpTesting() {
    ArsenalHttp arsenal;
    arsenal.Init();
}