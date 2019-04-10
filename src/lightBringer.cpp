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
    this->actionResponseQueue.push(actionResponse);
}

int LightBringer::GetActionResponseSize() {
    return static_cast<int>(this->actionResponseQueue.size());
}

APITPtr LightBringer::PopAction(void) {
    APITPtr temp = std::move(this->actionQueue.front());
    this->actionQueue.pop();

    return std::move(temp);
}

void LightBringer::PushAction(APITPtr action) {
    this->actionQueue.push(action);
}

int LightBringer::GetActionSize(void) {
    return static_cast<int>(this->actionQueue.size());
}

void LightBringer::BlackIceTesting() {
    BlackIce blackIce(this);
    blackIce.Init();
}

void LightBringer::ArsenalHttpTesting() {
    ArsenalHttp arsenal(this);
    arsenal.Init();
}