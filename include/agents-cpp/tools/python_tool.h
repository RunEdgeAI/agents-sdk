/**
 * @file python_tool.h
 * @brief Python Execution Tool Header
 * @version 0.1
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#pragma once

#include <agents-cpp/tool.h>

namespace agents {
namespace tools {

/**
 * @brief Python execution tool that provides safe Python code execution capabilities
 */
class PythonTool : public Tool {
public:
    /**
     * @brief Construct a new Python Tool object
     */
    PythonTool();

    /**
     * @brief Execute the Python tool
     * @param params The parameters for the Python tool
     * @return ToolResult The result of the Python tool
     */
    ToolResult execute(const JsonObject& params) const override;

/*! @cond PRIVATE */
protected:
    /**
     * @brief Setup the parameters for the Python tool
     */
    void setupParameters();

    /**
     * @brief Validate the Python code
     * @param code The Python code to validate
     * @return true if the code is valid, false otherwise
     */
    bool validatePythonCode(const std::string& code) const;

    /**
     * @brief Check if the Python code is dangerous
     * @param code The Python code to check
     * @return true if the code is dangerous, false otherwise
     */
    bool isDangerousPythonCode(const std::string& code) const;

    /**
     * @brief Execute the Python code
     * @param code The Python code to execute
     * @return ToolResult The result of the Python code execution
     */
    ToolResult executePythonCode(const std::string& code) const;

    /**
     * @brief Format the Python result
     * @param code The Python code that was executed
     * @param output The output of the Python code
     * @param success Whether the Python code execution was successful
     * @param error The error message if the Python code execution was not successful
     * @return ToolResult The formatted result of the Python code execution
     */
    ToolResult formatPythonResult(const std::string& code, const std::string& output, bool success, const std::string& error = "") const;
/*! @endcond */
private:
    /**
     * @brief Python interpreter state management
     */
    static bool pythonInitialized;

    /**
     * @brief Initialize the Python interpreter
     */
    static void initializePython();

    /**
     * @brief Finalize the Python interpreter
     */
    static void finalizePython();
};

} // namespace tools
} // namespace agents
