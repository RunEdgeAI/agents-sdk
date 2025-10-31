/**
 * @file utils.h
 * @brief Utility functions for the project
 * @version 0.1
 * @date 2025-10-25
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 */

#pragma once

#include <agents-cpp/types.h>

namespace agents {

/**
 * @brief Utility class providing static helper functions
 */
class Utils {
public:
    /**
     * @brief Change a key in a JSON object
     *
     * @param object The JSON object
     * @param old_key The old key name
     * @param new_key The new key name
     */
    static void changeKey(JsonObject& object, const std::string& old_key, const std::string& new_key) {
        // get iterator to old key; TODO: error handling if key is not present
        auto it = object.find(old_key);
        if (it == object.end()) {
            throw std::runtime_error("Key not found: " + old_key);
        }
        // create null value for new key and swap value from old key
        std::swap(object[new_key], it.value());
        // delete value at old key (cheap, because the value is null after swap)
        object.erase(it);
    }

    /**
     * @brief Convert a string to lowercase
     *
     * @param s The input string
     * @return The lowercase string
     */
    static inline std::string toLower(const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        return result;
    }
};

} // namespace agents