#include "pch.h"
#include "lightBringer.h"
#include "blackIce.h"
#include "arsenalHttp.h"

LightBringer::LightBringer() {}

APITPtr LightBringer::PopActionResponse() {
    APITPtr temp;

    this->actionResponseQueueLock.lock();

    temp = std::move(this->actionResponseQueue.front());
    this->actionResponseQueue.pop();

    this->actionResponseQueueLock.unlock();

    return std::move(temp);
}

void LightBringer::PushActionResponse(APITPtr actionResponse) {
    this->actionResponseQueueLock.lock();

    this->actionResponseQueue.push(std::move(actionResponse));

    this->actionResponseQueueLock.unlock();
}

int LightBringer::GetActionResponseSize() {
    return static_cast<int>(this->actionResponseQueue.size());
}

APITPtr LightBringer::PopAction(void) {
    APITPtr temp;

    this->actionQueueLock.lock();

    temp = std::move(this->actionQueue.front());
    this->actionQueue.pop();

    this->actionQueueLock.unlock();

    return std::move(temp);
}

void LightBringer::PushAction(APITPtr action) {
    this->actionQueueLock.lock();

    this->actionQueue.push(std::move(action));

    this->actionQueueLock.unlock();
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
