#include "./server/http_request.hpp"
#include "./server/http_response.hpp"
#include "./server/http_server.hpp"
#include "./stl/static_collection.hpp"

void handleHello(HttpRequest* req, HttpResponse* res) {
    SafeString responseBody = SafeString::format("Hello, API World! :  %s", req->path.cstr());
    
    res->setStatus(200, "OK");
    res->addHeader("X-Custom-Header", "Cpp-Rest");
    res->setBody(responseBody.cstr());
}

void handleStatus(HttpRequest* req, HttpResponse* res) {
    (void) req;
    res->setStatus(200, "OK");
    res->setBody("API Status: Running");
}

int main(void) {
    HttpServer server;
    server.init(8080);

    server.router.addRoute("GET", "/",       handleHello);
    server.router.addRoute("GET", "/hello",  handleHello);
    server.router.addRoute("GET", "/status", handleStatus);

    server.start();

    return 0;
}