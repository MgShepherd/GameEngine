#ifndef LOGGER_H
#define LOGGER_H

enum LoggerLevel {
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_NONE
};

/**
 * @brief Initalises Logger
 *
 * This function performs all setup for enabling logging within an application
 * and also creates error callback functions for dependencies such as GLFW
 *
 * @param level Determines which is the lowest log level that will be shown
 */
void logger_init(enum LoggerLevel level);

/**
 * @brief Displays a message to stdout with log level INFO
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void logger_info(const char *message, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Display a message to stderror with log level ERROR
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void logger_error(const char *message, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Display a message to stderror with log level WARN
 *
 * @param message The format string to be displayed
 * @param ... 		Additional arguments corresponding to the format string
 */
void logger_warn(const char *message, ...) __attribute__((format(printf, 1, 2)));

#endif
