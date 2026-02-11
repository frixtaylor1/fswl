#include "http_router.hpp"

void HttpRouter::add(const char* method, const char* path, RequestHandler handler) {
    routes.tryAdd({method, path, handler});
}

bool HttpRouter::handle(IRequest* req, IResponse* res) {
    String reqPath = req->getPath();
    size_t qpos    = reqPath.find('?');
    if (qpos != String::npos) {
        reqPath = reqPath.substr(0, qpos);
    }
    if (reqPath.length() > 1 && reqPath.back() == '/') {
        reqPath.pop_back();
    }

    for (auto&& it = routes.begin(); it != routes.end(); ++it) {
        if (it->method == req->getMethod() && it->path == reqPath) {
            it->handler(req, res);
            return true;
        }
    }
    
    res->setStatus(404, "Not Found");
    res->setBody("Resource not found");
    return false;
}