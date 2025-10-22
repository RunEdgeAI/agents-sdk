/**
 * @file autonomous_agent.h
 * @brief Autonomous Agent Definition
 * @version 0.1
 * @date 2025-07-20
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#pragma once

#include <agents-cpp/agent.h>
#include <agents-cpp/coroutine_utils.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace agents {

/**
 * @brief An agent that operates autonomously to complete a task
 *
 * Autonomous agents start with a task, plan steps to accomplish it,
 * and use tools to execute those steps. They can be configured with
 * various strategies and human-in-the-loop options.
 */
class AutonomousAgent : public Agent {
public:
    /**
     * @brief Step in the agent's execution
     */
    struct Step {
        /**
         * @brief The description of the step
         */
        String description;
        /**
         * @brief The status of the step
         */
        String status;
        /**
         * @brief The result of the step
         */
        JsonObject result;
        /**
         * @brief Whether the step was successful
         */
        bool success;
    };

    /**
     * @brief Planning strategy for the agent
     */
    enum class PlanningStrategy {
        /**
         * @brief Generate actions without explicit planning
         */
        ZERO_SHOT,
        /**
         * @brief Generate multiple reasoning paths
         */
        TREE_OF_THOUGHT,
        /**
         * @brief Generate a plan then execute it
         */
        PLAN_AND_EXECUTE,
        /**
         * @brief Reflect on past steps for improvement
         */
        REFLEXION,
        /**
         * @brief Reasoning and acting
         */
        REACT
    };

    /**
     * @brief Constructor
     * @param context The agent context
     */
    AutonomousAgent(std::shared_ptr<Context> context);

    /**
     * @brief Destructor
     */
    ~AutonomousAgent() override = default;

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
     * @param feedback The feedback
     */
    void provideFeedback(const String& feedback) override;

    /**
     * @brief Set the agent prompt
     * @param agent_prompt The agent prompt
     */
    void setAgentPrompt(const String& agent_prompt);

    /**
     * @brief Set the planning strategy
     * @param strategy The planning strategy
     */
    void setPlanningStrategy(PlanningStrategy strategy);

    /**
     * @brief Get the steps executed so far
     * @return The steps executed so far
     */
    std::vector<Step> getSteps() const;

    /**
     * @brief Set a callback for when a step is completed
     * @param callback The callback
     */
    void setStepCallback(std::function<void(const Step&)> callback);

    /**
     * @brief Wait for feedback using coroutines
     * @param message The message to wait for feedback
     * @param context The context
     * @return The feedback
     */
    Task<String> waitForFeedback(const String& message, const JsonObject& context) override;

private:
    String agent_prompt_;
    PlanningStrategy planning_strategy_ = PlanningStrategy::REACT;
    std::vector<Step> steps_;
    std::function<void(const Step&)> step_callback_;

    // Execution state
    std::atomic<bool> should_stop_{false};

    /**
     * @brief Promise for coroutine-based feedback
     */
    std::promise<String> feedback_promise_;

    /**
     * @brief Execute the agent's task using coroutines
     * @param task The task to execute
     * @return The result of the task
     */
    Task<JsonObject> executeTask(const String& task);

    /**
     * @brief Execute a step using coroutines
     * @param step_description The description of the step
     * @param context The context
     * @return The result of the step
     */
    Task<Step> executeStep(String& step_description, JsonObject& context);

    /**
     * @brief Record a completed step
     * @param step The step to record
     */
    void recordStep(const Step& step);

    /**
     * @brief Get tool descriptions for prompts
     * @return The tool descriptions
     */
    String getToolDescriptions() const;

    /**
     * @brief Plan using Zero Shot
     * @param task The task to plan
     * @return The plan
     */
    Task<JsonObject> planZeroShot(const String& task);

    /**
     * @brief Plan using Tree of Thought
     * @param task The task to plan
     * @return The plan
     */
    Task<JsonObject> planTreeOfThought(const String& task);

    /**
     * @brief Plan and execute
     * @param task The task to plan
     * @return The plan
     */
    Task<JsonObject> planAndExecute(const String& task);

    /**
     * @brief Plan using Reflexion
     * @param task The task to plan
     * @return The plan
     */
    Task<JsonObject> planReflexion(const String& task);

    /**
     * @brief Plan using React
     * @param task The task to plan
     * @return The plan
     */
    Task<JsonObject> planReact(const String& task);

    /**
     * @brief Split a string by newlines
     * @param text The text to split
     * @return The lines
     */
    std::vector<std::string> split_by_newline(const std::string& text) {
        std::vector<std::string> lines;
        std::istringstream iss(text);
        std::string line;

        // Read lines from the stringstream until the end, delimited by '\n'
        while (std::getline(iss, line, '\n')) {
            lines.push_back(line);
        }
        return lines;
    }
};

} // namespace agents
