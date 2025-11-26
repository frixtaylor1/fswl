#include "./server/http_request.hpp"
#include "./server/http_response.hpp"
#include "./server/http_server.hpp"
#include "./stl/static_collection.hpp"
#include "./parser/json.hpp"

static void handleHello(HttpRequest* req, HttpResponse* res) {
    SafeString responseBody = SafeString::format("Hello, API World! :  %s", req->path.cstr());
    res->setStatus(200, "OK");
    res->addHeader("X-Custom-Header", "Cpp-Rest");
    res->setBody(responseBody.cstr());
}

static void handleStatus(HttpRequest* req, HttpResponse* res) {
    (void) req;
    res->setStatus(200, "OK");  
    res->setBody("API Status: Running");
}

int main() {
    const char json_input[] = 
        "{\n"
        "  \"nombre\": \"ejemplo\",\n"
        "  \"version\": 1.0,\n"
        "  \"activo\": true,\n"
        "  \"configuracion\": {\n"
        "    \"max_intentos\": 3,\n"
        "    \"timeout\": 15.5\n"
        "  },\n"
        "  \"datos_nulos\": null\n"
        "}";

    SA_PRINT("--- Input JSON ---\n");
    SA_PRINT("%s\n\n",json_input);
    SA_PRINT("------------------------------------------------\n");

    Json       json; 
    JsonParser parser(&json, json_input);

    if (parser.parse()) {
        const JsonObject& rootObject = json.object;
        SA_PRINT("\n Parsed structure \n");
        SA_PRINT("Root Object:\n");
        JsonValue rootValue;
        new (&rootValue.content.objectStorage) JsonObject(rootObject); 
        rootValue.type = JSON_OBJECT;

        rootValue.dump();

        const JsonValue* intentos_val = rootValue.get("nombre");
        SA_PRINT("%s\n", intentos_val->asString().cstr());
        rootValue.~JsonValue();
    }

    return 0;
}