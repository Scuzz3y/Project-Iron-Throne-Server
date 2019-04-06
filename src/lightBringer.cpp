#include "pch.h"
#include "lightBringer.h"
#include "blackIce.h"

LightBringer::LightBringer() {}

ActionResponsePtr LightBringer::PopActionResponse() {
    ActionResponsePtr temp = std::move(this->actionResponseQueue.front());
    this->actionResponseQueue.pop();

    return std::move(temp);
}

void LightBringer::PushActionResponse(ActionResponsePtr actionResponse) {
    this->actionResponseQueue.push(std::move(actionResponse));
}

ActionPtr LightBringer::PopAction(void) {
    ActionPtr temp = std::move(this->actionQueue.front());
    this->actionQueue.pop();

    return std::move(temp);
}

void LightBringer::PushAction(ActionPtr action) {
    this->actionQueue.push(std::move(action));
}

void LightBringer::BlackIceTesting() {
    BlackIce blackIce(this);
    blackIce.Start();
}
