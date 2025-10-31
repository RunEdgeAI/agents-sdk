#ifndef AGENTS_CPP_TOOLS_WEB_SEARCH_TOOL_H
#define AGENTS_CPP_TOOLS_WEB_SEARCH_TOOL_H

#include <agents-cpp/tool.h>
#include <agents-cpp/types.h>

#include <memory>
#include <string>

namespace agents {
namespace tools {

/**
 * @brief Web search tool that provides secure web search capabilities
 */
class WebSearchTool : public Tool {
public:
    /**
     * @brief Construct a new Web Search Tool object
     */
    WebSearchTool();

    /**
     * @brief Execute the Web Search Tool
     * @param params The parameters for the Web Search Tool
     * @return ToolResult The result of the Web Search Tool
     */
    ToolResult execute(const JsonObject& params) const override;

private:
    /**
     * @brief Setup the parameters for the Web Search Tool
     */
    void setupParameters();

    /**
     * @brief Perform a web search
     * @param query The query to search for
     * @return ToolResult The result of the web search
     */
    ToolResult performWebSearch(const std::string& query) const;

    /**
     * @brief Perform a SerpApi search
     * @param query The query to search for
     * @return ToolResult The result of the SerpApi search
     */
    ToolResult performSerpApiSearch(const std::string& query) const;

    // Security validation
    /**
     * @brief Check if the query is dangerous
     * @param query The query to check
     * @return true if the query is dangerous, false otherwise
     */
    bool isDangerousQuery(const std::string& query) const;

    /**
     * @brief Poll for results when receiving a 202 Accepted response
     * @param query The original search query
     * @param initialResponse The initial 202 response JSON
     * @param source The search source (SerpApi)
     * @return ToolResult The final result after polling
     */
    ToolResult pollForResults(const std::string& query, const JsonObject& initialResponse, const std::string& source) const;

    /**
     * @brief Process SerpApi search results after polling
     * @param query The search query
     * @param responseJson The response JSON
     * @param statusCode The HTTP status code
     * @return ToolResult The processed results
     */
    ToolResult processSerpApiResults(const std::string& query, const JsonObject& responseJson, int statusCode) const;
};

} // namespace tools
} // namespace agents

#endif // AGENTS_CPP_TOOLS_WEB_SEARCH_TOOL_H
