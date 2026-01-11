#include "./server/implementations/http_request.hpp"
#include "./server/implementations/http_response.hpp"
#include "./server/implementations/http_server.hpp"
#include "./stl/static_collection.hpp"
#include "./parser/json.hpp"

static void handleHello(IRequest* req, IResponse* res) {
    String responseBody = format("Hello, API World! : {}", req->getPath().c_str());
    res->setStatus(200, "OK");
    res->addHeader("X-Custom-Header", "Cpp-Rest");
    res->setBody(responseBody.c_str());
}

static void ensureRequest(JsonParser &jsonParser, IResponse *res, bool &retFlag) {
    retFlag = true;
    if (!jsonParser.parse()) {
        String responseBody{"Invalid Json format"};
        res->setStatus(400, "BadRquest");
        res->setBody(responseBody.c_str());
        return;
    }
    retFlag = false;
}

static void handlePost(IRequest* req, IResponse* res) {
    Json       reqJson;
    JsonParser jsonParser(&reqJson, req->getBody().c_str());

    bool retFlag;
    ensureRequest(jsonParser, res, retFlag);
    if (retFlag)
        return;

    if (reqJson.get("name")) {
        String responseBody = format("JSON parsed successfully: hello {}", reqJson.get("name")->asCString());
        res->setStatus(200, "OK");
        res->setBody(responseBody.c_str());
    }
}

static void handleStatus(IRequest *req, IResponse *res) {
    (void) req;

    res->setStatus(200, "OK");  
    res->setBody("API Status: Running");
}

interface IMiddleware {
    virtual void process(IRequest* req, IResponse* res) = 0;
};

struct Middleware : implements IMiddleware {
    void process(IRequest* req, IResponse* res) {
        (void) req; (void) res;
        return ;
    }
};

void assignRoutes(IRouter* router) {
    router->add("GET",  "/",          &handleHello);
    router->add("POST", "/something", &handlePost);
    router->add("GET",  "/status",    &handleStatus);
}

template <class Application>
implementing < IServer, Application >
struct Serve {

    template < class RouteHandler >
    implementing < IRouter, RouteHandler >
    struct Publishing {

        template < class Middleware >
        implementing < IMiddleware, Middleware >
        struct With {
            Application    server;
            RouteHandler   router;
            Middleware     middleware;

            With() {
                server.init(8081);
                server.bindRouter(&router);
                assignRoutes(&router);
                server.start();
            }
        };
    };
};

typedef Serve< HttpServer >::Publishing< HttpRouter >::With< Middleware > ApiRest;

int main(void) {
    ApiRest();
    return 0; 
}