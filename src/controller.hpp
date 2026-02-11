#ifndef controller_hpp
#define controller_hpp

constexpr const uint32 HTTP_STATUS_OK = 200;
constexpr const uint32 HTTP_STATUS_BAD_REQUEST = 400;

#include "./server/interfaces/irequest.hpp"
#include "./server/interfaces/iresponse.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static void handleHello(IRequest* req, IResponse* res) {
    String responseBody = format("Hello, API World! : {}", req->getPath().c_str());
    res->setStatus(HTTP_STATUS_OK, "OK");
    res->addHeader("X-Custom-Header", "Cpp-Rest");
    res->setBody(responseBody.c_str());
}

static void handlePost(IRequest* req, IResponse* res) {
    try {
        json reqJson = json::parse(req->getBody().c_str());
        
        if (reqJson.contains("name")) {
            String responseBody = format("JSON parsed successfully: hello {}", reqJson["name"].get<String>().c_str());
            res->setStatus(HTTP_STATUS_OK, "OK");
            res->setBody(responseBody.c_str());
        }
    } catch (const json::exception& e) {
        String responseBody{"Invalid Json format"};
        res->setStatus(HTTP_STATUS_BAD_REQUEST, "BadRequest");
        res->setBody(responseBody.c_str());
    }
}

static void handleStatus(IRequest *req, IResponse *res) {
    (void) req;

    res->setStatus(HTTP_STATUS_OK, "OK");  
    res->setBody("API Status: Running");
}

#endif // controller_hpp