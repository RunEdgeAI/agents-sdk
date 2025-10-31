/**
 * @file respond_tool.h
 * @brief Respond Tool Definition
 * @version 0.1
 * @date 2025-10-31
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 */
#pragma once

#include <agents-cpp/tool.h>

namespace agents {
namespace tools {

/**
 * @brief Respond tool that provides safe response generation capabilities
 */
class RespondTool : public Tool {
public:
    /**
     * @brief Construct a new Respond Tool object
     */
    RespondTool();

    /**
     * @brief Execute the Respond tool
     * @param params The parameters for the Respond tool
     * @return ToolResult The result of the Respond tool
     */
    ToolResult execute(const JsonObject& params) const override;
};

} // namespace tools
} // namespace agents
