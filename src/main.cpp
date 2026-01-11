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

static void handlePost(IRequest* req, IResponse* res) {
    Json       reqJson;
    JsonParser jsonParser(&reqJson, req->getBody().c_str());

    if (!jsonParser.parse()) {
        String responseBody {"Invalid Json format"};
        res->setStatus(400, "BadRquest");
        res->setBody(responseBody.c_str());
        return;
    }
    
    if (reqJson.get("name")) {
        String responseBody = format("JSON parsed successfully: hello {}", reqJson.get("name")->asCString());
        res->setStatus(200, "OK");
        res->setBody(responseBody.c_str());
    }
}

static void handleStatus(IRequest* req, IResponse* res) {
    (void) req;

    res->setStatus(200, "OK");  
    res->setBody("API Status: Running");
}

interface IMiddleware {
    virtual void process(IRequest* req, IResponse* res) = 0;
};

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
            RouteHandler   routeHandler;
            Middleware     middleware;
            
            With() {

                SA_ASSERT(false, "MUST IMPLEMENT!");
            }
        };
    };
};

int main() {
    HttpRouter router;
    HttpServer server(router);
    server.init(8081);

    server.router.add("GET", "/",              &handleHello);
    server.router.add("POST", "/something",    &handlePost);
    server.router.add("GET", "/status",        &handleStatus);

    server.start();

    return 0;
}