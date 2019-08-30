#include "pch.h"
#include "lightBringer.h"
#include "blackIce.h"
#include "arsenalHttp.h"

LightBringer::LightBringer() {}

void LightBringer::InitCli() {
    std::cout << "Stub CLI Here:" << std::endl;

    pause();
}

APITPtr LightBringer::PopActionResponse() {
    APITPtr temp;

    this->actionResponseQueueLock.lock();

    temp = std::move(this->actionResponseQueue.front());
    this->actionResponseQueue.pop();

    this->actionResponseQueueLock.unlock();

    return temp;
}

void LightBringer::PushActionResponse(APITPtr actionResponse) {
    this->actionResponseQueueLock.lock();

    this->actionResponseQueue.push(std::move(actionResponse));

    this->actionResponseQueueLock.unlock();
}

int LightBringer::GetActionResponseSize() {
    this->actionResponseQueueLock.lock();

    int size = static_cast<int>(this->actionResponseQueue.size());

    this->actionResponseQueueLock.unlock();

    return size;
}

APITPtr LightBringer::PopAction(void) {
    APITPtr temp;

    this->actionQueueLock.lock();

    temp = std::move(this->actionQueue.front());
    this->actionQueue.pop();

    this->actionQueueLock.unlock();

    return temp;
}

void LightBringer::PushAction(APITPtr action) {
    this->actionQueueLock.lock();

    this->actionQueue.push(std::move(action));

    this->actionQueueLock.unlock();
}

int LightBringer::GetActionSize(void) {
    this->actionQueueLock.lock();

    int size = static_cast<int>(this->actionQueue.size());

    this->actionQueueLock.unlock();

    return size;
}
