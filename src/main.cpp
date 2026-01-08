#include "./server/http_request.hpp"
#include "./server/http_response.hpp"
#include "./server/http_server.hpp"
#include "./stl/static_collection.hpp"
#include "./parser/json.hpp"

static void handleHello(HttpRequest* req, HttpResponse* res) {
    SafeString responseBody = format("Hello, API World! : {}", req->path.c_str());
    res->setStatus(200, "OK");
    res->addHeader("X-Custom-Header", "Cpp-Rest");
    res->setBody(responseBody.c_str());
}

static void handlePost(HttpRequest* req, HttpResponse* res) {
    Json       reqJson;
    JsonParser jsonParser(&reqJson, req->body.c_str());

    if (!jsonParser.parse()) {
        SafeString responseBody {"Invalid Json format"};
        res->setStatus(400, "BadRquest");
        res->setBody(responseBody.c_str());
        return;
    }
    
    if (reqJson.get("name")) {
        SafeString responseBody = format("JSON parsed successfully: hello {}", reqJson.get("name")->asCString());
        res->setStatus(200, "OK");
        res->setBody(responseBody.c_str());
    }
}

static void handleStatus(HttpRequest* req, HttpResponse* res) {
    (void) req;

    res->setStatus(200, "OK");  
    res->setBody("API Status: Running");
}

int main() {
    HttpServer server;
    server.init(8081);

    server.router.addRoute("GET", "/",              &handleHello);
    server.router.addRoute("POST", "/something",    &handlePost);
    server.router.addRoute("GET", "/status",        &handleStatus);

    server.start();

    return 0;
}