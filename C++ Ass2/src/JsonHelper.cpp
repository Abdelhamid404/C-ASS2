/*
 * ============================================
 * JSONHELPER.CPP - JSON Utilities Implementation
 * ============================================
 */

#include "../include/JsonHelper.h"
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

/*
 * escapeString - Escapes special characters in JSON strings
 */
string JsonHelper::escapeString(const string& str) {
    stringstream ss;
    for (char c : str) {
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (c >= 0 && c < 32) {
                    // Control characters - encode as unicode
                    ss << "\\u" << hex << setfill('0') 
                       << setw(4) << static_cast<int>(c);
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

/*
 * makeString - Creates a JSON string value
 */
string JsonHelper::makeString(const string& value) {
    return "\"" + escapeString(value) + "\"";
}

/*
 * makeNumber - Creates a JSON number value (int)
 */
string JsonHelper::makeNumber(int value) {
    return to_string(value);
}

/*
 * makeNumber - Creates a JSON number value (double)
 */
string JsonHelper::makeNumber(double value) {
    stringstream ss;
    ss << fixed << setprecision(2) << value;
    return ss.str();
}

/*
 * makeBool - Creates a JSON boolean value
 */
string JsonHelper::makeBool(bool value) {
    return value ? "true" : "false";
}

/*
 * makeNull - Creates a JSON null value
 */
string JsonHelper::makeNull() {
    return "null";
}

/*
 * makeArray - Creates a JSON array from vector of strings
 */
string JsonHelper::makeArray(const vector<string>& items) {
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) ss << ",";
        ss << items[i];
    }
    ss << "]";
    return ss.str();
}

/*
 * makeObject - Creates a JSON object from key-value pairs
 */
string JsonHelper::makeObject(const vector<pair<string, string>>& pairs) {
    stringstream ss;
    ss << "{";
    for (size_t i = 0; i < pairs.size(); ++i) {
        if (i > 0) ss << ",";
        ss << makeString(pairs[i].first) << ":" << pairs[i].second;
    }
    ss << "}";
    return ss.str();
}

/*
 * parseSimpleValue - Basic JSON value extraction
 * 
 * This is a simple parser for extracting values from JSON.
 * For complex JSON parsing, use nlohmann/json library.
 */
string JsonHelper::parseSimpleValue(const string& json, const string& key) {
    string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    
    if (keyPos == string::npos) {
        return "";
    }
    
    // Find the colon after the key
    size_t colonPos = json.find(':', keyPos + searchKey.length());
    if (colonPos == string::npos) {
        return "";
    }
    
    // Skip whitespace
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && isspace(json[valueStart])) {
        valueStart++;
    }
    
    if (valueStart >= json.length()) {
        return "";
    }
    
    // Determine value type and extract
    if (json[valueStart] == '"') {
        // String value
        size_t valueEnd = json.find('"', valueStart + 1);
        if (valueEnd != string::npos) {
            return json.substr(valueStart + 1, valueEnd - valueStart - 1);
        }
    } else if (json[valueStart] == '{' || json[valueStart] == '[') {
        // Object or array - find matching bracket
        char openBracket = json[valueStart];
        char closeBracket = (openBracket == '{') ? '}' : ']';
        int depth = 1;
        size_t valueEnd = valueStart + 1;
        
        while (valueEnd < json.length() && depth > 0) {
            if (json[valueEnd] == openBracket) depth++;
            else if (json[valueEnd] == closeBracket) depth--;
            valueEnd++;
        }
        
        return json.substr(valueStart, valueEnd - valueStart);
    } else {
        // Number, boolean, or null
        size_t valueEnd = json.find_first_of(",}]", valueStart);
        if (valueEnd != string::npos) {
            string value = json.substr(valueStart, valueEnd - valueStart);
            // Trim whitespace
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            return value;
        }
    }
    
    return "";
}

/*
 * successResponse - Creates a standard success response
 */
string JsonHelper::successResponse(const string& message) {
    vector<pair<string, string>> pairs = {
        {"success", makeBool(true)},
        {"message", makeString(message)}
    };
    return makeObject(pairs);
}

/*
 * errorResponse - Creates a standard error response
 */
string JsonHelper::errorResponse(const string& message) {
    vector<pair<string, string>> pairs = {
        {"success", makeBool(false)},
        {"message", makeString(message)}
    };
    return makeObject(pairs);
}

/*
 * dataResponse - Creates a response with data
 */
string JsonHelper::dataResponse(const string& data) {
    vector<pair<string, string>> pairs = {
        {"success", makeBool(true)},
        {"data", data}
    };
    return makeObject(pairs);
}
