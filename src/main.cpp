#include "./server/implementations/http_request.hpp"
#include "./server/implementations/http_response.hpp"
#include "./server/implementations/http_server.hpp"
#include "./stl/static_collection.hpp"
#include "./routes.hpp"

interface IMiddleware {
    virtual void process(IRequest* req, IResponse* res) = 0;
};

struct Middleware : implements IMiddleware {
    void process(IRequest* req, IResponse* res) {
        (void) req; (void) res;
        return ;
    }
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
            RouteHandler   router;
            Middleware     middleware;

            With() {
                server.init();
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