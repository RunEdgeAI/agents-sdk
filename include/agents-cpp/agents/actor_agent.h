/**
 * @file actor_agent.h
 * @brief Actor Agent Definition
 * @version 0.1
 * @date 2025-07-20
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#pragma once

#include <agents-cpp/agent.h>
#include <agents-cpp/context.h>
#include <agents-cpp/coroutine_utils.h>
#include <chrono>
#include <memory>
#include <mutex>

namespace agents {

/**
 * @brief Actor-based agent implementation
 *
 * @details This class implements a flexible agent that can operate autonomously,
 * use tools, and achieve complex tasks.
 */
class ActorAgent : public Agent {
public:
    /**
     * @brief Constructor with agent context
     * @param context The agent context
     */
    ActorAgent(std::shared_ptr<Context> context);

    /**
     * @brief Destructor
     */
    virtual ~ActorAgent();

    /**
     * @brief Initialize the agent
     */
    void init() override;

    /**
     * @brief Run the agent with a task using coroutines
     * @param task The task to run
     * @return The result of the task
     */
    Task<JsonObject> run(const String& task) override;

    /**
     * @brief Stop the agent
     */
    void stop() override;

    /**
     * @brief Provide human feedback
     * @param feedback The feedback to provide
     */
    void provideFeedback(const String& feedback) override;

    /**
     * @brief Set the agent prompt for the agent
     * @param agent_prompt The agent prompt to set
     */
    void setAgentPrompt(const String& agent_prompt);

    /**
     * @brief Get the current agent prompt
     * @return The current agent prompt
     */
    String getAgentPrompt() const;

    /**
     * @brief Wait for feedback using coroutines
     * @param message The message to wait for feedback
     * @param context The context to wait for feedback
     * @return The feedback
     */
    Task<String> waitForFeedback(const String& message, const JsonObject& context) override;

    /**
     * @brief Run the agent asynchronously with callback
     * @param task The task to run
     * @param callback Callback function to call with result
     */
    void runAsync(const String& task, std::function<void(const JsonObject&)> callback);


/*! @cond PRIVATE */
protected:
    /**
     * @brief Agent prompt
     */
    String agent_prompt_;

    /**
     * @brief Feedback promise for simple feedback mechanism
     */
    std::promise<String> feedback_promise_;

    /**
     * @brief Mutex for feedback promise
     */
    std::mutex feedback_mutex_;

    /**
     * @brief Callback for when a tool is used
     * @param tool_name The name of the tool used
     * @param params The parameters of the tool used
     * @param result The result of the tool used
     */
    virtual void onToolUsed(const String& tool_name, const JsonObject& params, const ToolResult& result);

    /**
     * @brief Callback for when the agent generates a response
     * @param response The response from the agent
     */
    virtual void onResponse(const String& response);

    /**
     * @brief Callback for when the agent errors
     * @param error The error message
     */
    virtual void onError(const String& error);

    /**
     * @brief Create the agent prompt with available tools
     * @return The agent prompt
     */
    virtual String createAgentPrompt() const;

    /**
     * @brief Process a message with coroutines
     * @param message The message to process
     * @return The processed message
     */
    Task<String> processMessage(const String& message);

/*! @endcond */
};

} // namespace agents
