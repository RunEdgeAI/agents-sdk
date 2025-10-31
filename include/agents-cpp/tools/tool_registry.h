/**
 * @file tool_registry.h
 * @brief Tool Registry Definition
 * @version 0.1
 * @date 2025-07-20
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#pragma once

#include <agents-cpp/tool.h>
#include <agents-cpp/tools/media_loader_tool.h>
#include <agents-cpp/tools/summarization_tool.h>
#include <agents-cpp/tools/wiki_tool.h>

#include <map>
#include <memory>
#include <vector>

namespace agents {
/**
 * @brief Tools Namespace
 *
 */
namespace tools {

/**
 * @brief Registry for tools that agents can use
 *
 * The ToolRegistry provides a central place to register, retrieve,
 * and manage tools that agents can use.
 */
class ToolRegistry {
public:
    ToolRegistry() = default;
    ~ToolRegistry() = default;

    /**
     * @brief Register a tool
     * @param tool The tool to register
     */
    void registerTool(std::shared_ptr<Tool> tool);

    /**
     * @brief Get a tool by name
     * @param name The name of the tool
     * @return The tool
     */
    std::shared_ptr<Tool> getTool(const std::string& name) const;

    /**
     * @brief Get all registered tools
     * @return The tools
     */
    std::vector<std::shared_ptr<Tool>> getAllTools() const;

    /**
     * @brief Check if a tool is registered
     * @param name The name of the tool
     * @return True if the tool is registered, false otherwise
     */
    bool hasTool(const std::string& name) const;

    /**
     * @brief Remove a tool
     * @param name The name of the tool
     */
    void removeTool(const std::string& name);

    /**
     * @brief Clear all tools
     */
    void clear();

    /**
     * @brief Get tool schemas as JSON
     * @return The tool schemas
     */
    JsonObject getToolSchemas() const;

    /**
     * @brief Get the global tool registry
     * @return The global tool registry
     */
    static ToolRegistry& global();

private:
    std::map<std::string, std::shared_ptr<Tool>> tools_;
};

/**
 * @brief Create and register standard tools
 *
 * @param registry The tool registry to register tools with
 * @param llm Optional LLM interface for tools that require it
 */
void registerStandardTools(ToolRegistry& registry, std::shared_ptr<LLMInterface> llm = nullptr);

/**
 * @brief Creates a tool for executing shell commands
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createShellCommandTool();

/**
 * @brief Creates a tool for performing web searches
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createWebSearchTool();

/**
 * @brief Creates a tool for retrieving information from Wikipedia
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createWikipediaTool();

/**
 * @brief Creates a tool for running Python code
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createPythonTool();

/**
 * @brief Creates a tool for reading files
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createFileReadTool();

/**
 * @brief Creates a tool for writing files
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createFileWriteTool();

/**
 * @brief Creates a tool for text summarization
 *
 * @param llm The LLM interface to use
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createSummarizationTool(std::shared_ptr<LLMInterface> llm);

/**
 * @brief Creates a tool for loading media from URLs or local files
 * @param llm The LLM interface to use
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createMediaLoaderTool(std::shared_ptr<LLMInterface> llm);

/**
 * @brief Creates a tool for generating safe responses
 *
 * @return Pointer to tool
 */
std::shared_ptr<Tool> createRespondTool();

} // namespace tools
} // namespace agents
