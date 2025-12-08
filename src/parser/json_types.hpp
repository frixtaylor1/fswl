#ifndef json_types_hpp
#define json_types_hpp

#include "../stl/static_collection.hpp"

enum JsonType {
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_BOOL,
    JSON_NULL
};

typedef AnsiString<64>                                JsonKey;
struct                                                JsonValue;
typedef AssociativeContainer<JsonKey, JsonValue*, 64> JsonAssociativeContainer;
typedef Collection<JsonValue*, 64>                    JsonCollection;

struct JsonObject {
    JsonAssociativeContainer container;

    JsonObject() {}
    ~JsonObject() {
        for (uint i = 0; i < container.length(); ++i) {
            container.values.at(i) = nullptr;
        }
        container.keys.length = 0;
    }

    const JsonValue* get(const char* key) const {
        JsonKey searchKey(key); 
        if (container.exists(searchKey)) {
            return container.getValue(searchKey);
        }
        return nullptr;
    }
};

struct JsonArray {
    JsonCollection items;

    JsonArray() {}
    ~JsonArray() {
        for (uint i = 0; i < items.length; ++i) {
            items.items[i] = nullptr;
        }
        items.length = 0;
    }

    JsonArray(const JsonArray& rhs) {
        this->items = rhs.items;
    }

    JsonArray& operator = (const JsonArray& rhs) {
        this->items = rhs.items;
        return *this;
    }
};


#endif // json_types_hpp