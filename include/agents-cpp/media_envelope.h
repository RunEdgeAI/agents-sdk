/**
 * @file media_envelope.h
 * @brief Minimal helpers for a canonical media JSONObject envelope used across tools and providers
 * @version 0.1
 * @date 2025-09-17
 *
 * The canonical envelope shape (all fields optional unless stated):
 * {
 *   "type": "text" | "image" | "audio" | "video" | "document",            // required
 *   "text": "...",                                                        // when type==text
 *   "mime": "image/png" | "audio/wav" | "video/mp4" | "application/pdf",  // required for non-text
 *   "uri":  "http(s)://..." | "file://..." | "data:...",                  // exactly one of uri or data
 *   "data": "<base64-bytes>",                                             // exactly one of uri or data
 *   "meta": {                                                             // optional metadata
 *     "width": 1024, "height": 768, "fps": 30.0, "duration_s": 3.2,
 *     "sample_rate_hz": 16000, "channels": 1
 *   }
 * }
 */

#pragma once

#include <agents-cpp/types.h>

namespace agents {

/**
 * @brief Media envelope namespace
 */
namespace media {

/**
 * @brief Check if a key exists and is a string
 *
 * @param j     The JSON object
 * @param key   The key to check
 * @return bool true if key exists and is a string, else false
 */
bool hasString(const JsonObject& j, const char* key);

/**
 * @brief Check if a key exists and is an object
 *
 * @param j     The JSON object
 * @param key   The key to check
 * @return bool true if key exists and is an object, else false
 */
bool hasObject(const JsonObject& j, const char* key);

/**
 * @brief Check if a key exists and is a specific type
 *
 * @param j     The JSON object
 * @param key   The key to check
 * @param value The type to compare against
 * @return bool true if key exists and is of the specified type, else false
 */
bool eqType(const JsonObject& j, const char* value);

/**
 * @brief Check if the media type is known
 *
 * @param v The media type string
 * @return bool true if type is known, else false
 */
bool isKnownType(const std::string& v);

/**
 * @brief Check if the media type is known
 *
 * @param j The JSON object
 * @return bool true if type is known, else false
 */
bool hasKnownType(const JsonObject& j);

/**
 * @brief Quick probe to see if JSONObject looks like a media envelope (canonical or compatible)
 */
bool isMediaPart(const JsonObject& j);

/**
 * @brief Normalize various accepted shapes into the canonical envelope, validating constraints
 * @throws std::invalid_argument if the input cannot be normalized to a valid envelope
 */
JsonObject normalizeMediaPart(JsonObject j);

/**
 * @brief Extract MIME type (returns empty string if not present)
 */
std::string getMime(const JsonObject& j);

/**
 * @brief Returns true if the envelope carries a URI reference
 */
bool hasUri(const JsonObject& j);

/**
 * @brief Returns true if the envelope carries inline base64 data
 */
bool hasData(const JsonObject& j);

/**
 * @brief Best-effort parse of a Data URL to extract MIME type (empty if not parsable)
 */
std::string mimeFromDataUrl(const std::string& data_url);

/**
 * @brief Parse a string into a media envelope if possible; returns nullopt if not media
 */
std::optional<JsonObject> tryParseEnvelopeFromString(const std::string& content);

/** Media envelope builders **/

/**
 * @brief Create a text media envelope
 * @param s The text content
 * @return The media envelope
 */
inline JsonObject text(const std::string& s) {
    return JsonObject{{"type","text"},{"text",s}};
}

/**
 * @brief Create an image media envelope
 * @param uri The image URI
 * @param mime The image MIME type
 * @param meta The image metadata
 * @return The media envelope
 */
inline JsonObject imageUri(const std::string& uri, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","image"},{"mime",mime},{"uri",uri}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create an image media envelope with base64 data
 * @param base64 The base64 data
 * @param mime The image MIME type
 * @param meta The image metadata
 * @return The media envelope
 */
inline JsonObject imageData(const std::string& base64, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","image"},{"mime",mime},{"data",base64}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create an audio media envelope
 * @param uri The audio URI
 * @param mime The audio MIME type
 * @param meta The audio metadata
 * @return The media envelope
 */
inline JsonObject audioUri(const std::string& uri, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","audio"},{"mime",mime},{"uri",uri}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create an audio media envelope with base64 data
 * @param base64 The base64 data
 * @param mime The audio MIME type
 * @param meta The audio metadata
 * @return The media envelope
 */
inline JsonObject audioData(const std::string& base64, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","audio"},{"mime",mime},{"data",base64}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create a video media envelope
 * @param uri The video URI
 * @param mime The video MIME type
 * @param meta The video metadata
 * @return The media envelope
 */
inline JsonObject videoUri(const std::string& uri, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","video"},{"mime",mime},{"uri",uri}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create a video media envelope with base64 data
 * @param base64 The base64 data
 * @param mime The video MIME type
 * @param meta The video metadata
 * @return The media envelope
 */
inline JsonObject videoData(const std::string& base64, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","video"},{"mime",mime},{"data",base64}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create a document media envelope
 * @param uri The document URI
 * @param mime The document MIME type
 * @param meta The document metadata
 * @return The media envelope
 */
inline JsonObject documentUri(const std::string& uri, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","document"},{"mime",mime},{"uri",uri}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

/**
 * @brief Create a document media envelope with base64 data
 * @param base64 The base64 data
 * @param mime The document MIME type
 * @param meta The document metadata
 * @return The media envelope
 */
inline JsonObject documentData(const std::string& base64, const std::string& mime, JsonObject meta = {}) {
    JsonObject j{{"type","document"},{"mime",mime},{"data",base64}};
    if (!meta.empty()) j["meta"] = std::move(meta);
    return j;
}

} // namespace media
} // namespace agents


