#ifndef json_hpp
#define json_hpp

#include "../stl/static_collection.hpp"
#include "json_types.hpp"
#include "json_value.hpp"

#include <ctype.h>

struct Json;
struct JsonParser;


struct Json {
    enum { JSON_valueArena_CAPACITY = 128 };
    alignas(JsonValue) char valueArena[sizeof(JsonValue) * JSON_valueArena_CAPACITY];
    uint valueCount = 0;
    
    JsonObject object;
    
    JsonValue* allocateValue() {
        if (valueCount >= JSON_valueArena_CAPACITY) {
            return nullptr;
        }
        
        void* ptr = valueArena + (valueCount * sizeof(JsonValue));
        
        JsonValue* newValue = new (ptr) JsonValue();
        valueCount++;
        return newValue;
    }
    
    Json() : object(), valueCount(0) {}
    
    ~Json() {
        for (uint i = 0; i < valueCount; ++i) {
            JsonValue* value = (JsonValue*) valueArena + (i * sizeof(JsonValue));
            value->~JsonValue();
        }
    }
};

/** @TODO: generalize (templates), reader, from, analizer, rules, etc... */
struct JsonParser {
private:
    Json*       json;
    const char* input;
    uint      cursor;
    
    void skipWhiteSpaces() {
        while (input[cursor] && isspace(input[cursor])) {
            cursor++;
        }
    }

    JsonValue* parseValue() {
        skipWhiteSpaces();

        switch (input[cursor]) {
            case '{': return parseObject();
            case '[': return parseArray();
            case '"': return parseString();
            case 't':
            case 'f':
                return parseBool();
            case 'n':
                return parseNull();
            case '-':
            case '0'...'9':
                return parseNumber();
            default:
                return nullptr; 
        }
    }
    
    JsonValue* parseNull() {
        if (strncmp(&input[cursor], "null", 4) == 0) {
            JsonValue* val = json->allocateValue();
            if (val) {
                val->setNull();
                cursor += 4;
            }
            return val;
        }
        return nullptr;
    }

    JsonValue* parseBool() {
        if (strncmp(&input[cursor], "true", 4) == 0) {
            JsonValue* val = json->allocateValue();
            if (val) {
                val->setBool(true);
                cursor += 4;
            }
            return val;
        }
        if (strncmp(&input[cursor], "false", 5) == 0) {
            JsonValue* val = json->allocateValue();
            if (val) {
                val->setBool(false);
                cursor += 5;
            }
            return val;
        }
        return nullptr;
    }

    JsonValue* parseNumber() {
        const char* start = &input[cursor];
        
        while (input[cursor] && (isdigit(input[cursor]) || input[cursor] == '.' || input[cursor] == '-' || input[cursor] == 'e' || input[cursor] == 'E' || input[cursor] == '+')) {
            cursor++;
        }
        
        uint len = &input[cursor] - start;
        
        if (len == 0) {
            return nullptr;
        }

        char numberStr[64];
        if (len >= sizeof(numberStr)) len = sizeof(numberStr) - 1; 
        strncpy(numberStr, start, len);
        numberStr[len] = '\0';

        double numberValue = 0.0;
        if (sscanf(numberStr, "%lf", &numberValue) != 1) {
            return nullptr;
        }

        JsonValue* val = json->allocateValue();
        if (val) {
            val->setNumber(numberValue);
        }
        return val;
    }
    
    JsonValue* parseString() {
        cursor++;
        const char* start = &input[cursor];
        
        uint len = 0;
        while (input[cursor] && input[cursor] != '"') {
            cursor++;
            len++;
        }
        
        if (input[cursor] != '"') {
            return nullptr;
        }
        
        cursor++;

        char tempStr[128]; 
        if (len >= sizeof(tempStr)) len = sizeof(tempStr) - 1;
        strncpy(tempStr, start, len);
        tempStr[len] = '\0';
        
        JsonString jsonStr(tempStr);
        
        JsonValue* val = json->allocateValue();
        if (val) {
            val->setString(jsonStr);
        }
        return val;
    }

    JsonValue* parseArray() {
        cursor++;
        JsonValue* val = json->allocateValue();
        if (!val) return nullptr;
        
        new (&val->content.arrayStorage) JsonArray(); 
        val->type = JSON_ARRAY;
        JsonArray& contentArray = val->getArray();
        
        skipWhiteSpaces();
        if (input[cursor] == ']') {
            cursor++;
            return val;
        }
        
        while (true) {
            JsonValue* item = parseValue();
            if (!item) {
                return nullptr;
            }

            if (contentArray.items.length < contentArray.items.DEFAULT_CAPACITY) {
                 contentArray.items.items[contentArray.items.length] = item;
                 contentArray.items.length++;
            } else {
                return nullptr; 
            }
            
            skipWhiteSpaces();
            if (input[cursor] == ']') {
                cursor++;
                break;
            } else if (input[cursor] == ',') {
                cursor++;
            } else {
                return nullptr; 
            }
        }
        return val;
    }

    JsonKey parseKey() {

        if (input[cursor] != '"') {
            return JsonKey();
        }
        cursor++;
        const char* start = &input[cursor];
        
        uint len = 0;
        while (input[cursor] && input[cursor] != '"') {
            cursor++;
            len++;
        }
        
        if (input[cursor] != '"') {
            return JsonKey();
        }
        
        cursor++;

        char tempKeyStr[64]; 
        if (len >= sizeof(tempKeyStr)) len = sizeof(tempKeyStr) - 1;
        strncpy(tempKeyStr, start, len);
        tempKeyStr[len] = '\0';
        
        return JsonKey(tempKeyStr);
    }

    JsonValue* parseObject() {
        cursor++;
        JsonValue* val = json->allocateValue();
        if (!val) return nullptr;
        
        new (&val->content.objectStorage) JsonObject(); 
        val->type = JSON_OBJECT;
        JsonObject& currentObject = val->getObject();

        skipWhiteSpaces();
        if (input[cursor] == '}') {
            cursor++;
            return val;
        }

        while (true) {
            skipWhiteSpaces();
            
            JsonKey key = parseKey();
            
            if (key.length() == 0 && key == "\"\"") {
                return nullptr;
            }

            skipWhiteSpaces();
            if (input[cursor] != ':') {
                return nullptr;
            }
            cursor++;

            JsonValue* value = parseValue();
            if (!value) {
                return nullptr;
            }

            currentObject.container.add(key, value);
            
            skipWhiteSpaces();
            if (input[cursor] == '}') {
                cursor++;
                break;
            } else if (input[cursor] == ',') {
                cursor++;
            } else {
                return nullptr; 
            }
        }
        return val;
    }


public:
    JsonParser(Json* json, const char* input) : json(json), input(input), cursor(0) {}

    bool parse() {
        skipWhiteSpaces();
        if (input[cursor] != '{') {
            return false;
        }

        JsonValue* rootValue = parseObject();
        
        skipWhiteSpaces();
        if (rootValue && !input[cursor]) {
            json->object = rootValue->asObject(); 
            return true;
        }
        
        return false;
    }
};

#endif // json_hpp
