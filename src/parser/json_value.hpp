#ifndef json_value_hpp
#define json_value_hpp

#include "json_types.hpp"
#include "../stl/ansi_string.hpp"
#include <new>

struct JsonValue {
    JsonType type = JSON_NULL;

    union Content {
        double numberValue;
        bool   boolValue;

        alignas(String)     char stringStorage[sizeof(String)];
        alignas(JsonObject) char objectStorage[sizeof(JsonObject)];
        alignas(JsonArray)  char arrayStorage[sizeof(JsonArray)];

        Content() {} 
        ~Content() {}
    } content;
public:
    JsonValue();
    JsonValue(const JsonValue& other);
    JsonValue(JsonValue&& other);
    ~JsonValue();

    void dump(void);

    String&       getString(void);
    const String& getString(void) const;
    JsonObject&       getObject(void);
    const JsonObject& getObject(void) const;
    JsonArray&        getArray(void);
    const JsonArray&  getArray(void)  const;

    void              setNumber(double val);
    void              setBool(bool val);
    void              setString(const String& s);
    void              setNull(void);
    void              setObject(const JsonObject& obj);
    JsonValue&        operator = (const JsonValue& other);
    
public:
    const JsonValue*  get(const char* key) const;
    String        serialize(void) const;

    double            asNumber() const;
    const String& asString(void) const;
    const JsonObject& asObject(void) const;
    const JsonArray&  asArray(void) const;

private:
    void              _dump(const JsonValue* val, int indent = 0);
};

void JsonValue::dump(void) {
    _dump(this);
}

String& JsonValue::getString(void) { 
    return *(String*) content.stringStorage; 
}

const String& JsonValue::getString(void) const { 
    return *(String*) content.stringStorage; 
}

JsonObject& JsonValue::getObject(void) { 
    return *(JsonObject*) content.objectStorage; 
}

const JsonObject& JsonValue::getObject(void) const { 
    return *(JsonObject*) content.objectStorage; 
}

JsonArray& JsonValue::getArray(void) { 
    return *(JsonArray*) content.arrayStorage; 
}

const JsonArray& JsonValue::getArray(void) const { 
    return *(JsonArray*) content.arrayStorage; 
}

JsonValue::JsonValue() : type(JSON_NULL) { content.numberValue = 0; }

JsonValue::JsonValue(const JsonValue& other) : type(other.type) {
    switch (type) {
        case JSON_STRING:
            new (&content.stringStorage) String(other.getString());
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

JsonValue::JsonValue(JsonValue&& other) : type(other.type) {
    switch (type) {
        case JSON_STRING:
            new (&content.stringStorage) String(other.getString());
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

JsonValue::~JsonValue() {
    switch (type) {
        case JSON_STRING:
            getString().~String();
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

JsonValue& JsonValue::operator = (const JsonValue& other) {
    if (this != &other) {
        this->~JsonValue(); 
        
        type = other.type;
        switch (type) {
            case JSON_STRING:
                new (&content.stringStorage) String(other.getString());
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

const JsonValue* JsonValue::get(const char* key) const {
    if (type == JSON_OBJECT) {
        return getObject().get(key);
    }
    return nullptr;
}

double JsonValue::asNumber() const {
    if (type == JSON_NUMBER) {
        return content.numberValue;
    }
    return 0.0; 
}

const String& JsonValue::asString(void) const {
    return getString();
}

const JsonObject& JsonValue::asObject(void) const {
    return getObject();
}

const JsonArray& JsonValue::asArray(void) const {
    return getArray();
}

void JsonValue::setNumber(double val) {
    this->~JsonValue();
    type = JSON_NUMBER;
    content.numberValue = val;
}

void JsonValue::setBool(bool val) {
    this->~JsonValue();
    type = JSON_BOOL;

    content.boolValue = val;
}

void JsonValue::setString(const String& s) {
    this->~JsonValue();
    type = JSON_STRING;
    new (&content.stringStorage) String(s); 
}

void JsonValue::setNull() {
    this->~JsonValue();
    type = JSON_NULL;
}

void JsonValue::setObject(const JsonObject& obj) {
    this->~JsonValue();
    type = JSON_OBJECT;
    new (&content.objectStorage) JsonObject(obj);
}

String JsonValue::serialize() const {
    switch (type) {
        case JSON_NULL:
            return "null";

        case JSON_BOOL:
            return content.boolValue ? "true" : "false";

        case JSON_NUMBER: {
            return String::format("%lf", asNumber()); 
        }

        case JSON_STRING: {
            String raw = asString(); 
            String encoded = String::format("\"%s\"", raw.cstr());
            return encoded; 
        }

        case JSON_ARRAY: {
            String result("{");
            const JsonArray& arr = asArray();
            for (uint i = 0; i < arr.items.length; ++i) {
                if (i > 0) result.concat(", ");
                result.concat(arr.items.items[i]->serialize().cstr()); 
            }
            result.concat("}");
            return result;
        }

        case JSON_OBJECT: {
            String result("{");
            const JsonObject& obj = asObject();
            
            /** @TODO: MUST IMPLEMENT */

            bool first = true;
            
            result.concat("}");
            return result;
        }
        default:
            return String("null");
    }
}

void JsonValue::_dump(const JsonValue* val, int indent) {
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
            for (uint i = 0; i < obj.container.length(); ++i) {
                for (int j = 0; j < indent + 1; ++j) {
                    SA_PRINT("  ");
                }
                
                const JsonKey&   key         = obj.container.getKeyAt(i);
                const JsonValue* memberValue = obj.container.getValueAt(i);
                
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

#endif // json_value_hpp