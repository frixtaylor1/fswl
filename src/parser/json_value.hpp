#ifndef json_value_hpp
#define json_value_hpp

#include "json_types.hpp"
#include <new>

struct JsonValue {
    JsonType type = JSON_NULL;

    union Content {
        double numberValue;
        bool   boolValue;

        alignas(JsonString) char stringStorage[sizeof(JsonString)];
        alignas(JsonObject) char objectStorage[sizeof(JsonObject)];
        alignas(JsonArray)  char arrayStorage[sizeof(JsonArray)];

        Content() {} 
        ~Content() {}
    } content;
public:
    void dump(void) {
        _dump(this);
    }

    JsonString& getString() { 
        return *(JsonString*) content.stringStorage; 
    }
    
    const JsonString& getString() const { 
        return *(JsonString*) content.stringStorage; 
    }
    
    JsonObject& getObject() { 
        return *(JsonObject*) content.objectStorage; 
    }
    
    const JsonObject& getObject() const { 
        return *(JsonObject*) content.objectStorage; 
    }
    
    JsonArray& getArray() { 
        return *(JsonArray*) content.arrayStorage; 
    }
    
    const JsonArray& getArray() const { 
        return *(JsonArray*) content.arrayStorage; 
    }

public:
    JsonValue() : type(JSON_NULL) { content.numberValue = 0; }

    JsonValue(const JsonValue& other) : type(other.type) {
        switch (type) {
            case JSON_STRING:
                new (&content.stringStorage) JsonString(other.getString());
                break;
            case JSON_NUMBER:
                content.numberValue = other.content.numberValue;
                break;
            case JSON_BOOL:
                content.boolValue = other.content.boolValue;
                break;
            case JSON_OBJECT:
                new (&content.objectStorage) JsonObject(other.getObject());
                break;
            case JSON_ARRAY:
                new (&content.arrayStorage) JsonArray(other.getArray());
                break;
            case JSON_NULL:
            default:
                break;
        }
    }

    JsonValue(JsonValue&& other) : type(other.type) {
        switch (type) {
            case JSON_STRING:
                new (&content.stringStorage) JsonString(other.getString());
                break;
            case JSON_NUMBER:
                content.numberValue = other.content.numberValue;
                break;
            case JSON_BOOL:
                content.boolValue = other.content.boolValue;
                break;
            case JSON_OBJECT:
                new (&content.objectStorage) JsonObject(other.getObject());
                break;
            case JSON_ARRAY:
                new (&content.arrayStorage) JsonArray(other.getArray());
                break;
            case JSON_NULL:
            default:
                break;
        }
    } 

    ~JsonValue() {
        if (this) return;

        switch (type) {
            case JSON_STRING:
                getString().~JsonString();
                break;
            case JSON_OBJECT:
                getObject().~JsonObject();
                break;
            case JSON_ARRAY:
                getArray().~JsonArray();
                break;
            default:
                break;
        }
    }
    
    JsonValue& operator = (const JsonValue& other) {
        if (this != &other) {
            this->~JsonValue(); 
            
            type = other.type;
            switch (type) {
                case JSON_STRING:
                    new (&content.stringStorage) JsonString(other.getString());
                    break;
                case JSON_NUMBER:
                    content.numberValue = other.content.numberValue;
                    break;
                case JSON_BOOL:
                    content.boolValue = other.content.boolValue;
                    break;
                case JSON_OBJECT:
                    new (&content.objectStorage) JsonObject(other.getObject());
                    break;
                case JSON_ARRAY:
                    new (&content.arrayStorage) JsonArray(other.getArray());
                    break;
                case JSON_NULL:
                default:
                    break;
            }
        }
        return *this;
    }

    const JsonValue* get(const char* key) const {
        if (type == JSON_OBJECT) {
            return getObject().get(key);
        }
        return nullptr;
    }
    
    double asNumber() const {
        if (type == JSON_NUMBER) {
            return content.numberValue;
        }
        return 0.0; 
    }
    
    const JsonString& asString() const {
        return getString();
    }

    const JsonObject& asObject() const {
        return getObject();
    }

    const JsonArray& asArray() const {
        return getArray();
    }
    
    void setNumber(double val) {
        this->~JsonValue();
        type = JSON_NUMBER;
        content.numberValue = val;
    }

    void setBool(bool val) {
        this->~JsonValue();
        type = JSON_BOOL;

        content.boolValue = val;
    }
    
    void setString(const JsonString& s) {
        this->~JsonValue();
        type = JSON_STRING;
        new (&content.stringStorage) JsonString(s); 
    }

    void setNull() {
        this->~JsonValue();
        type = JSON_NULL;
    }

    void setObject(const JsonObject& obj) {
        this->~JsonValue();
        type = JSON_OBJECT;
        new (&content.objectStorage) JsonObject(obj);
    }
    JsonString serialize() const {
        switch (type) {
            case JSON_STRING:
                return getString();
            case JSON_NUMBER:
                return JsonString::toString(content.numberValue);
            case JSON_BOOL:
                return content.boolValue ? JsonString("true") : JsonString("false");
            case JSON_NULL:
                return JsonString("null");
            default:
                return JsonString("");
        }
    }

private:
    void _dump(const JsonValue* val, int indent = 0) {
        if (!val) {
            SA_PRINT("null (Error)");
            return;
        }

        for (int i = 0; i < indent; ++i) {
            SA_PRINT("  ");
        }

        switch (val->type) {
            case JSON_STRING:
                SA_PRINT("\"%s\"", val->asString().cstr());
                break;
            case JSON_NUMBER:
                SA_PRINT("%s", val->serialize().cstr());
                break;
            case JSON_BOOL:
                SA_PRINT(val->content.boolValue ? "true" : "false");
                break;
            case JSON_NULL:
                SA_PRINT("null");
                break;
            case JSON_OBJECT: {
                SA_PRINT("{\n");
                const JsonObject& obj = val->asObject();
                for (size_t i = 0; i < obj.container.length(); ++i) {
                    for (int j = 0; j < indent + 1; ++j) {
                        SA_PRINT("  ");
                    }
                    
                    const JsonKey& key = obj.container.keys.at(i);
                    const JsonValue* memberValue = obj.container.values.at(i);
                    
                    SA_PRINT("\"%s\": ", key.cstr());
                    
                    if (memberValue->type == JSON_OBJECT || memberValue->type == JSON_ARRAY) {
                        _dump(memberValue, indent + 1);
                    } else {
                        _dump(memberValue);
                    }
                    
                    if (i < obj.container.length() - 1) {
                        SA_PRINT(",\n");
                    } else {
                        SA_PRINT("\n");
                    }
                }
                for (int i = 0; i < indent; ++i) {
                    SA_PRINT("  ");
                }
                SA_PRINT("}");
                break;
            }
            case JSON_ARRAY:
                SA_PRINT("[ ... Array ... ]");
                break;
            default:
                SA_PRINT("Error: Tipo Desconocido");
                break;
        }
    }
};

#endif // json_value_hpp