/**
 * @file live_client_interface.h
 * @brief Abstract interface for live real-time AI clients
 * @details This interface provides a common abstraction for different real-time
 * AI providers and transport protocols (WebSocket, WebRTC, etc.).
 *
 * @version 1.0
 * @date 2025-07-20
 */

#pragma once

#include <agents-cpp/types.h>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace agents {

/**
 * @brief Transport protocol types for live AI communication
 *
 * Different protocols offer different trade-offs in terms of latency,
 * reliability, and browser compatibility.
 */
enum class TransportProtocol {
    WEBSOCKET,    ///< WebSocket-based communication (high compatibility, medium latency)
    WEBRTC,       ///< WebRTC-based communication (low latency, requires ICE servers)
    HTTP_STREAM,  ///< HTTP streaming (fallback option, high latency)
    CUSTOM        ///< Custom protocol implementation
};

/**
 * @brief Audio format specification for live AI sessions
 *
 * Defines the audio format for both input and output streams.
 * All live AI providers support PCM16 mono as the primary format.
 */
struct AudioFormat {
    int sample_rate_hz = 16000;     ///< Sample rate in Hz (typically 16000 or 24000)
    int channels = 1;               ///< Number of channels (1 = mono, 2 = stereo)
    int bits_per_sample = 16;       ///< Bits per sample (16, 24, 32)
    bool is_float = false;          ///< true for float samples, false for integer
    bool is_signed = true;          ///< true for signed samples, false for unsigned
    bool is_little_endian = true;   ///< Byte order (true = little endian)
};

/**
 * @brief Configuration for live AI sessions
 *
 * This is a minimal common configuration that works across all providers.
 * Provider-specific implementations may extend this with additional options
 * through the provider_options map.
 */
struct LiveSessionConfig {
    std::string model;  ///< Model name (provider-specific format)

    std::string system_instruction = "You are a helpful assistant and answer in a friendly tone.";  ///< System instruction to steer the assistant

    TransportProtocol preferred_transport = TransportProtocol::WEBSOCKET;  ///< Preferred transport protocol

    AudioFormat input_audio_format;   ///< Input audio format specification
    AudioFormat output_audio_format;  ///< Output audio format specification

    int connection_timeout_ms = 30000;    ///< Connection timeout in milliseconds
    int keepalive_interval_ms = 30000;    ///< Keepalive interval in milliseconds
    bool auto_reconnect = true;           ///< Auto-reconnect on disconnect

    std::map<std::string, std::string> provider_options;  ///< Provider-specific options (key-value pairs)
};

/**
 * @brief Abstract interface for live real-time AI clients
 *
 * This interface provides a common abstraction for different real-time
 * AI providers and transport protocols. It focuses on core functionality
 * that is truly common across all implementations.
 *
 * @note Provider-specific features are available through concrete implementations
 *       and can be accessed via dynamic_cast when needed.
 */
class ILiveClient {
public:
    /**
     * @brief Callbacks for live client events
     *
     * Not all callbacks may be supported by all providers or protocols.
     * Check provider documentation for specific support.
     */
    struct Callbacks {
        std::function<void(const void* samples, size_t num_samples, const AudioFormat& format)> on_audio_frame;  ///< Audio output callback
        std::function<void(const std::string& text)> on_text;  ///< Text output callback (transcription, responses)
        std::function<void()> on_turn_complete;  ///< Turn completion callback
        std::function<void()> on_connected;  ///< Connection established callback
        std::function<void()> on_disconnected;  ///< Connection lost callback
        std::function<void(const std::string& reason)> on_connection_failed;  ///< Connection failed callback
        std::function<void(const std::string& event_type, const std::string& data)> on_session_event;  ///< Session events callback
        std::function<void(const std::string& function_name, const std::string& arguments)> on_function_call;  ///< Function calls callback
        std::function<void(const std::string& error)> on_error;  ///< Error callback
    };

    /**
     * @brief Protocol selection strategies
     *
     * These strategies help choose the best transport protocol
     * based on the intended use case.
     */
    enum class ProtocolStrategy {
        PERFORMANCE,    ///< Prioritize low latency (WebRTC preferred)
        COMPATIBILITY,  ///< Prioritize compatibility (WebSocket preferred)
        RELIABILITY,    ///< Prioritize reliability (WebSocket with fallback)
        CUSTOM         ///< Use custom selection logic
    };

    virtual ~ILiveClient() = default;

    // ============================================================================
    // Factory Methods (Static)
    // ============================================================================

    /**
     * @brief Create a client (single simple factory)
     *
     * Chooses sensible defaults for protocol/transport based on the provider
     * and platform. Advanced selection can be implemented by concrete factories
     * if needed.
     * @param api_key API key for the provider
     * @param config Configuration for the live session
     * @return The created client
     */
    static std::unique_ptr<ILiveClient> create(const std::string& api_key, const LiveSessionConfig& config) {
        // Default implementation - to be overridden by concrete classes
        return nullptr;
    }

    /**
     * @brief Get supported protocols for this provider
     *
     * @return std::vector<TransportProtocol> Supported protocols
     */
    static std::vector<TransportProtocol> getSupportedProtocols() {
        return {};
    }

    /**
     * @brief Check if a protocol is supported by this provider
     *
     * @param protocol Transport protocol
     * @return bool True if supported
     */
    static bool isProtocolSupported(TransportProtocol protocol) {
        auto supported = getSupportedProtocols();
        return std::find(supported.begin(), supported.end(), protocol) != supported.end();
    }

    /**
     * @brief Get the recommended protocol for a given use case
     *
     * @param use_case Use case description ("performance", "compatibility", "reliability")
     * @return TransportProtocol Recommended protocol
     */
    static TransportProtocol getRecommendedProtocol(const std::string& use_case = "general") {
        auto supported = getSupportedProtocols();
        if (supported.empty()) return TransportProtocol::WEBSOCKET;

        // Default recommendation logic - can be overridden
        if (use_case == "performance" || use_case == "low_latency") {
            if (isProtocolSupported(TransportProtocol::WEBRTC)) {
                return TransportProtocol::WEBRTC;
            }
        }

        return supported[0]; // Return first supported protocol
    }

    // ============================================================================
    // Core Interface Methods (Pure Virtual)
    // ============================================================================

    /**
     * @brief Connect to the live AI service
     *
     * Establishes a connection using the configured transport protocol.
     *
     * @return bool True if connection successful
     */
    virtual bool connect() = 0;

    /**
     * @brief Close the connection to the live AI service
     *
     * Gracefully closes the connection and cleans up resources.
     */
    virtual void close() = 0;

    /**
     * @brief Check if currently connected
     *
     * @return bool True if connected
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Get the current transport protocol
     *
     * @return TransportProtocol The protocol being used
     */
    virtual TransportProtocol getTransportProtocol() const = 0;

    /**
     * @brief Send audio data with specified format
     *
     * @param samples Audio samples
     * @param num_samples Number of samples
     * @param format Audio format specification
     * @return bool True if successful
     */
    virtual bool sendAudio(const void* samples, size_t num_samples, const AudioFormat& format) = 0;

    /**
     * @brief Send text input to the AI
     *
     * @param text Text message
     * @return bool True if successful
     */
    virtual bool sendText(const std::string& text) = 0;

    /**
     * @brief Signal completion of the current turn
     *
     * This indicates that the user has finished their input and expects
     * a response from the AI.
     *
     * @return bool True if successful
     */
    virtual bool sendTurnComplete() = 0;

    /**
     * @brief Set event callbacks
     *
     * @param callbacks Callback structure
     */
    virtual void setCallbacks(const Callbacks& callbacks) = 0;

    /**
     * @brief Get the current session configuration
     *
     * @return const LiveSessionConfig& Session configuration
     */
    virtual const LiveSessionConfig& getConfig() const = 0;

    // ============================================================================
    // Convenience Methods (Virtual with Default Implementation)
    // ============================================================================

    /**
     * @brief Send a complete user message (text + turn completion)
     *
     * This is a convenience method that combines sendText() and sendTurnComplete()
     * for common use cases where you want to send a complete message.
     *
     * @param text User message text
     * @return bool True if successful
     */
    virtual bool sendMessage(const std::string& text) {
        if (!sendText(text)) return false;
        return sendTurnComplete();
    }

    /**
     * @brief Send audio data and complete the turn
     *
     * This is a convenience method for sending audio and completing the turn
     * in one call, which is common for voice input scenarios.
     *
     * @param samples Audio samples
     * @param num_samples Number of samples
     * @param sample_rate_hz Sample rate in Hz
     * @return bool True if successful
     */
    virtual bool sendAudioAndComplete(const int16_t* samples, size_t num_samples, int sample_rate_hz) {
        if (!sendAudio(samples, num_samples, AudioFormat{sample_rate_hz, 1, 16, false, true, true})) return false;
        return sendTurnComplete();
    }
};

} // namespace agents
