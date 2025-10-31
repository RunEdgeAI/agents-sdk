/**
 * @file http_client.h
 * @brief Thin HTTP Client Wrapper
 * @version 0.1
 * @date 2025-10-11
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 */
#pragma once

#include <functional>
#include <httplib.h>
#include <map>
#include <memory>
#include <random>
#include <string>

namespace agents {

/**
 * @brief Thin HTTP Client wrapper for internal use.
 *
 * Usage contract:
 * - Callers receive an `HTTPClient::Response` and should inspect
 *   `response.error` and `response.status_code` to decide success.
 * - For streaming responses provide a WriteCallback to `post()` to receive
 *   incremental body chunks.
 */
class HTTPClient {
public:
    /**
     * @brief Callback type for streaming response body data.
     *
     * The callback receives a string_view containing the next chunk of
     * response body data. It should return true to continue receiving
     * data or false to abort the transfer.
     */
    using WriteCallback = std::function<bool(const std::string_view&)>;

    /**
     * @brief Normalized response returned by the wrapper functions.
     *
     * - `status_code` contains the HTTP status or -1 on network/connect failure.
     * - `text` contains the full response body when no streaming callback was
     *    provided. For streaming calls the body will be empty.
     * - `error` is true when a transport or library error occurred.
     * - `error_message` contains an explanatory message for transport errors.
     */
    struct Response {
        int status_code;           /**< HTTP status code or -1 on error */
        std::string text;         /**< Response body (when available) */
        bool error;               /**< True when there was a transport error */
        std::string error_message;/**< Transport or parsing error message */
        httplib::Headers headers;  /**< Response headers */
    };

    /**
     * @brief Perform an HTTP POST to `url`.
     *
     * If `write_cb` is provided the function will stream response body
     * chunks to the callback instead of collecting the full body into
     * the returned `Response::text` field.
     *
     * @param url Full request URL
     * @param headers Map of headers to send
     * @param body Request body
     * @param timeout_ms Request timeout in milliseconds
     * @param write_cb Optional callback that receives incremental body data
     * @param multipart Optional vector of multipart form data parts
     * @return Response Normalized response object
     */
    static Response post(const std::string& url,
                        const std::map<std::string, std::string>& headers,
                        const std::string& body,
                        int timeout_ms = 30000,
                        WriteCallback write_cb = nullptr,
                        const std::vector<httplib::MultipartFormData>& multipart = {}) {
        Response result;

        try {
            auto session = createSession(url);
            setSessionOptions(session, headers, body, timeout_ms, write_cb);

            // Make request. If a streaming write callback is provided, use the
            // Post overload that accepts a ResponseHandler and a ContentReceiver
            // (content receiver is called for streaming body chunks).
            // Handle redirects similarly afterwards.
            httplib::Client* client = &session->client;
            std::shared_ptr<httplib::Client> redirect_client;

            // Build a Request so we can attach response/content callbacks when needed
            httplib::Request req;
            req.method = "POST";
            req.path = getPath(url);
            req.headers = session->headers;

            std::string multipart_boundary;
            std::string multipart_body;
            // If multipart is provided, encode multipart form; else use body
            if (!multipart.empty()) {
                // Build multipart/form-data body by hand
                multipart_boundary = make_boundary();
                multipart_body     = build_multipart_body(multipart, multipart_boundary);

                req.set_header("Content-Type",
                            "multipart/form-data; boundary=" + multipart_boundary);
                req.set_header("Content-Length", std::to_string(multipart_body.size()));
                req.body = std::move(multipart_body);
            } else {
                req.body = session->body;
            }

            if (write_cb) {
                // Attach a response handler (no-op but required by some overloads)
                req.response_handler = [](const httplib::Response &) { return true; };

                // Request::content_receiver expects ContentReceiverWithProgress
                req.content_receiver = [write_cb](const char *data, size_t len,
                                                  uint64_t /*offset*/, uint64_t /*total*/) {
                    return write_cb(std::string_view(data, len));
                };
            }

            // Send the request and get a Result (returned by value)
            auto res = client->send(req);

            // Handle redirects (similar to CPR's behavior)
            if (res && session->follow_redirects &&
                (res->status == 301 || res->status == 302 || res->status == 303 || res->status == 307 || res->status == 308)) {

                auto location = res->get_header_value("Location");
                if (!location.empty()) {
                    redirect_client = std::make_shared<httplib::Client>(getBaseUrl(location));
                    redirect_client->set_connection_timeout(session->timeout_ms / 1000);
                    redirect_client->set_read_timeout(session->timeout_ms / 1000);
                    redirect_client->set_write_timeout(session->timeout_ms / 1000);

                    client = redirect_client.get();
                    res = client->Get(getPath(location).c_str(), session->headers);
                }
            }

            if (res) {
                result.status_code = res->status;
                result.headers = res->headers;
                // Only set body text if no streaming callback
                if (!write_cb) {
                    result.text = res->body;
                }
                result.error = false;
                result.error_message = res->reason;
            } else {
                result.error = true;
                result.error_message = "Failed to connect";
                result.status_code = -1;
            }
        } catch (const std::exception& e) {
            result.error = true;
            result.error_message = e.what();
            result.status_code = -1;
        }

        return result;
    }

    /**
     * @brief Perform a simple HTTP GET and return the full body.
     *
     * This variant accepts headers but no query params. For callers that
     * wish to supply query parameters, use the overload that accepts
     * `httplib::Params` which delegates to cpp-httplib to perform
     * percent-encoding.
     *
     * @param url Full request URL
     * @param headers Map of headers to send
     * @param timeout_ms Request timeout in milliseconds
     * @return Response Normalized response object
     */
    static Response get(const std::string& url,
                       const std::map<std::string, std::string>& headers,
                       int timeout_ms = 30000) {
        Response result;

        try {
            httplib::Client cli(getBaseUrl(url));
            cli.set_connection_timeout(timeout_ms / 1000);

            // Set headers
            httplib::Headers header_map;
            for (const auto& [key, value] : headers) {
                header_map.emplace(key, value);
            }

            // Make request
            auto res = cli.Get(getPath(url).c_str(), header_map);

            if (res) {
                result.status_code = res->status;
                result.headers = res->headers;
                result.text = res->body;
                result.error = false;
            } else {
                result.error = true;
                result.error_message = "Request failed";
                result.status_code = -1;
            }
        } catch (const std::exception& e) {
            result.error = true;
            result.error_message = e.what();
            result.status_code = -1;
        }

        return result;
    }

    /**
     * @brief Perform an HTTP GET using `params` as query parameters.
     *
     * This overload uses cpp-httplib's `Get(path, params, headers)` so the
     * library handles percent-encoding and parameter ordering.
     *
     * @param url Full request URL
     * @param params Query parameters as an httplib::Params map
     * @param headers Map of headers to send
     * @param timeout_ms Request timeout in milliseconds
     * @return Response Normalized response object
     */
    static Response get(const std::string& url,
                       const httplib::Params& params,
                       const std::map<std::string, std::string>& headers,
                       int timeout_ms = 30000) {
        Response result;

        try {
            httplib::Client cli(getBaseUrl(url));
            cli.set_connection_timeout(timeout_ms / 1000);

            // Set headers
            httplib::Headers header_map;
            for (const auto& [key, value] : headers) {
                header_map.emplace(key, value);
            }

            // Use cpp-httplib's built-in params overload so it handles encoding
            auto res = cli.Get(getPath(url).c_str(), params, header_map);

            if (res) {
                result.status_code = res->status;
                result.headers = res->headers;
                result.text = res->body;
                result.error = false;
            } else {
                result.error = true;
                result.error_message = "Request failed";
                result.status_code = -1;
            }
        } catch (const std::exception& e) {
            result.error = true;
            result.error_message = e.what();
            result.status_code = -1;
        }

        return result;
    }

private:
    static std::string make_boundary() {
        static const char alphanum[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<size_t> dist(0, sizeof(alphanum) - 2);

        std::string s = "----cpp-httplib-boundary-";
        for (int i = 0; i < 24; ++i) s += alphanum[dist(rng)];
        return s;
    }

    static std::string build_multipart_body(const std::vector<httplib::MultipartFormData>& parts,
                                            const std::string& boundary) {
        std::string body;
        body.reserve(1024); // small pre-reserve; grows as needed

        for (const auto& p : parts) {
            body += "--" + boundary + "\r\n";
            body += "Content-Disposition: form-data; name=\"" + p.name + "\"";
            if (!p.filename.empty()) {
                body += "; filename=\"" + p.filename + "\"";
            }
            body += "\r\n";

            if (!p.content_type.empty()) {
                body += "Content-Type: " + p.content_type + "\r\n";
            }
            body += "\r\n";
            body += p.content;               // raw bytes are fine in std::string
            body += "\r\n";
        }

        body += "--" + boundary + "--\r\n";
        return body;
    }

    /**
     * @brief Per-request session/configuration object.
     *
     * Create with `createSession(url)` so the internal httplib::Client is
     * initialized with the request base URL. Call `setSessionOptions` to
     * configure headers/timeouts/body before issuing a request with the
     * session.
     */
    struct Session {
        httplib::Client client;    /**< Underlying cpp-httplib client */
        httplib::Headers headers;  /**< Headers to send with the request */
        std::string body;          /**< Request body for POST */
        WriteCallback write_cb;    /**< Optional streaming write callback */
        int timeout_ms;            /**< Read/write timeout in milliseconds */
        int connection_timeout_ms; /**< Connection timeout in milliseconds */
        bool follow_redirects;     /**< Follow 3xx redirects when true */
        bool verify_ssl;           /**< Verify TLS certificates (when supported) */
        std::string ca_cert_path;  /**< Optional CA certificate path */

        explicit Session(const std::string& base_url)
            : client(base_url),
              timeout_ms(30000),
              connection_timeout_ms(30000),
              follow_redirects(true),
              verify_ssl(true) {}
    };

    /**
     * @brief Create a Session initialized with the base URL extracted from `url`.
     *
     * The returned Session contains a configured `httplib::Client` whose base
     * URL is the scheme+host portion of `url`. Call `setSessionOptions` to
     * configure headers, body and timeouts before using the session.
     *
     * @param url Full request URL (scheme://host[/path...])
     * @return std::shared_ptr<Session> New session object
     */
    static std::shared_ptr<Session> createSession(const std::string& url) {
        auto session = std::make_shared<Session>(getBaseUrl(url));
        return session;
    }

    /**
     * @brief Configure a Session with headers, body and timeouts.
     *
     * This helper sets client timeouts, SSL options, CA path and populates
     * the session headers/body/write callback used by subsequent requests.
     *
     * @param session Session to configure (must be non-null)
     * @param headers Map of header key/value pairs to send
     * @param body Request body for POST requests
     * @param timeout_ms Read/write timeout in milliseconds
     * @param write_cb Optional streaming write callback used for streaming POSTs
     */
    static void setSessionOptions(std::shared_ptr<Session> session,
                                const std::map<std::string, std::string>& headers,
                                const std::string& body,
                                int timeout_ms = 30000,
                                WriteCallback write_cb = nullptr) {
        // Set timeouts
        session->client.set_connection_timeout(timeout_ms / 1000);
        session->client.set_read_timeout(timeout_ms / 1000);
        session->client.set_write_timeout(timeout_ms / 1000);

        // SSL verification
        #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        if (!session->verify_ssl) {
            session->client.enable_server_certificate_verification(false);
        }
        #endif

        // CA certificate if provided
        if (!session->ca_cert_path.empty()) {
            session->client.set_ca_cert_path(session->ca_cert_path.c_str());
        }

        // Set headers
        for (const auto& [key, value] : headers) {
            session->headers.emplace(key, value);
        }

        session->body = body;
        session->write_cb = write_cb;
        session->timeout_ms = timeout_ms;
    }

    /**
     * @brief Extract the base URL (scheme + host [+ port]) from a full URL.
     *
     * Example: `https://en.wikipedia.org/w/api.php?q=foo` ->
     * `https://en.wikipedia.org`
     *
     * @param url Full URL
     * @return std::string Base URL portion
     */
    static std::string getBaseUrl(const std::string& url) {
        size_t proto_end = url.find("://");
        if (proto_end == std::string::npos) return url;

        size_t path_start = url.find("/", proto_end + 3);
        if (path_start == std::string::npos) return url;

        return url.substr(0, path_start);
    }

    /**
     * @brief Extract the path (including leading '/') from a full URL.
     *
     * Example: `https://example.com/api/search?q=x` -> `/api/search?q=x`
     *
     * @param url Full URL
     * @return std::string Path and query portion
     */
    static std::string getPath(const std::string& url) {
        size_t proto_end = url.find("://");
        if (proto_end == std::string::npos) return "/";

        size_t path_start = url.find("/", proto_end + 3);
        if (path_start == std::string::npos) return "/";

        return url.substr(path_start);
    }
};

} // namespace agents