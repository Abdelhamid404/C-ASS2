/*
 * ============================================
 * JSONHELPER.H - JSON Utilities
 * ============================================
 * Helper functions for JSON serialization/deserialization.
 * Works with WebView to communicate with the frontend.
 */

#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

/*
 * JsonHelper - Utility class for JSON operations
 * 
 * This class provides simple methods for creating JSON strings
 * without external dependencies. For production, consider using
 * nlohmann/json library.
 */
class JsonHelper {
public:
    /*
     * escapeString - Escapes special characters in JSON strings
     * 
     * Handles: quotes, backslashes, newlines, tabs
     */
    static std::string escapeString(const std::string& str);

    /*
     * makeString - Creates a JSON string value
     * 
     * Example: makeString("hello") returns "\"hello\""
     */
    static std::string makeString(const std::string& value);

    /*
     * makeNumber - Creates a JSON number value
     */
    static std::string makeNumber(int value);
    static std::string makeNumber(double value);

    /*
     * makeBool - Creates a JSON boolean value
     */
    static std::string makeBool(bool value);

    /*
     * makeNull - Creates a JSON null value
     */
    static std::string makeNull();

    /*
     * makeArray - Creates a JSON array from vector of strings
     * 
     * Each string should already be valid JSON (object or value)
     */
    static std::string makeArray(const std::vector<std::string>& items);

    /*
     * makeObject - Creates a JSON object from key-value pairs
     * 
     * Values should already be valid JSON
     */
    static std::string makeObject(const std::vector<std::pair<std::string, std::string>>& pairs);

    /*
     * parseSimpleValue - Extracts a simple value from JSON
     * 
     * WARNING: This is a very basic parser. For complex JSON,
     * use nlohmann/json library.
     */
    static std::string parseSimpleValue(const std::string& json, const std::string& key);

    /*
     * Response helpers - Create standard response objects
     */
    static std::string successResponse(const std::string& message);
    static std::string errorResponse(const std::string& message);
    static std::string dataResponse(const std::string& data);
};

#endif // JSONHELPER_H
