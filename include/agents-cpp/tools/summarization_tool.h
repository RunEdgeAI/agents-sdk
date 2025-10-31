/**
 * @file summarization_tool.h
 * @brief Summarization Tool Header
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#pragma once

#include <agents-cpp/llm_interface.h>
#include <agents-cpp/tool.h>

namespace agents {
namespace tools {

/**
 * @brief Summarization tool that provides text summarization capabilities using LLM
 */
class SummarizationTool : public Tool {
public:
    /**
     * @brief Construct a new Summarization Tool object
     * @param llm The LLM interface to use
     */
    explicit SummarizationTool(std::shared_ptr<LLMInterface> llm);

    /**
     * @brief Execute the summarization tool
     * @param params The parameters for the summarization tool
     * @return ToolResult The result of the summarization tool
     */
    ToolResult execute(const JsonObject& params) const override;

/*! @cond PRIVATE */
protected:
    /**
     * @brief Setup the parameters for the summarization tool
     */
    void setupParameters();

    /**
     * @brief Validate the text parameter
     * @param text The text to validate
     * @return true if the text is valid, false otherwise
     */
    bool validateText(const std::string& text) const;

    /**
     * @brief Validate the max_length parameter
     * @param max_length The max length to validate
     * @return true if the max length is valid, false otherwise
     */
    bool validateMaxLength(int max_length) const;

    /**
     * @brief Generate the summarization prompt
     * @param text The text to summarize
     * @param max_length The maximum length of the summary
     * @return The formatted prompt
     */
    std::string generatePrompt(const std::string& text, int max_length) const;

    /**
     * @brief Format the summarization result
     * @param original_text The original text that was summarized
     * @param summary The generated summary
     * @param max_length The maximum length that was requested
     * @return ToolResult The formatted result of the summarization
     */
    ToolResult formatSummarizationResult(const std::string& original_text, const std::string& summary, int max_length) const;

    /**
     * @brief The LLM interface to use
     */
    std::shared_ptr<LLMInterface> llm_;
/*! @endcond */
};

} // namespace tools
} // namespace agents
