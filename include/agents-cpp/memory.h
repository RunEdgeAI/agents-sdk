/**
 * @file memory.h
 * @brief Memory Definition
 * @version 0.1
 * @date 2025-07-20
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#pragma once

#include <agents-cpp/types.h>
#include <memory>
#include <optional>
#include <vector>

namespace agents {

/**
 * @brief Interface for agent memory storage
 *
 * Memory stores information that can be retrieved by the agent,
 * such as conversation history, retrieved documents, or
 * intermediate results.
 */
class Memory {
public:
    virtual ~Memory() = default;

    /**
     * @brief Add a memory entry
     * @param key The key to add
     * @param value The value to add
     * @param type The type of memory to add
     */
    virtual void add(const String& key, const JsonObject& value, MemoryType type = MemoryType::SHORT_TERM) = 0;

    /**
     * @brief Get a memory entry by key
     * @param key The key to get
     * @param type The type of memory to get
     * @return The memory entry
     */
    virtual std::optional<JsonObject> get(const String& key, MemoryType type = MemoryType::SHORT_TERM) const = 0;

    /**
     * @brief Check if a memory entry exists
     * @param key The key to check
     * @param type The type of memory to check
     * @return true if exists, otherwise false
     */
    virtual bool has(const String& key, MemoryType type = MemoryType::SHORT_TERM) const = 0;

    /**
     * @brief Remove a memory entry
     * @param key The key to remove
     * @param type The type of memory to remove
     */
    virtual void remove(const String& key, MemoryType type = MemoryType::SHORT_TERM) = 0;

    /**
     * @brief Clear all memory of a specific type
     * @param type The type of memory to clear
     */
    virtual void clear(MemoryType type = MemoryType::SHORT_TERM) = 0;

    /**
     * @brief Add a conversation message to memory
     * @param message The message to add
     */
    virtual void addMessage(const Message& message) = 0;

    /**
     * @brief Get all conversation messages
     * @return The conversation messages
     */
    virtual std::vector<Message> getMessages() const = 0;

    /**
     * @brief Get conversation summary as a string
     * @param max_length The maximum length of the summary
     * @return The conversation summary
     */
    virtual String getConversationSummary(int max_length = 0) const = 0;

    /**
     * @brief Get conversation history as a JSON object
     * @return The conversation history
     */
    virtual JsonObject getConversationHistory() const = 0;

    /**
     * @brief Semantic search in memory
     * @param query The query to search for
     * @param type The type of memory to search
     * @param max_results The maximum number of results to return
     * @return The search results
     */
    virtual std::vector<std::pair<JsonObject, float>> search(
        const String& query,
        MemoryType type = MemoryType::LONG_TERM,
        int max_results = 5
    ) const = 0;
};

/**
 * @brief Create a Memory instance
 * @return The Memory instance
 */
std::shared_ptr<Memory> createMemory();

} // namespace agents