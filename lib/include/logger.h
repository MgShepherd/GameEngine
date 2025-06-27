#ifndef M_LOGGER_H
#define M_LOGGER_H

enum M_LogLevel { M_VERBOSE, M_INFO, M_WARN, M_ERROR, M_NONE };

/**
 * @brief Initalises Logger
 *
 * This function performs all setup for enabling logging within an application
 * and also creates error callback functions for dependencies such as GLFW
 *
 * @param level Determines which is the lowest log level that will be shown
 */
void m_logger_init(enum M_LogLevel level);

/**
 * @brief Displays a message to stdout with log level VERBOSE
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void m_logger_verbose(const char *message, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Displays a message to stdout with log level INFO
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void m_logger_info(const char *message, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Display a message to stderror with log level ERROR
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void m_logger_error(const char *message, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Display a message to stderror with log level WARN
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void m_logger_warn(const char *message, ...) __attribute__((format(printf, 1, 2)));

#endif
