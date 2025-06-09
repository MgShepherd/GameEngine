#include "logger.h"
#include <GLFW/glfw3.h>
#include <_time.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

const size_t TIME_DISPLAY_LEN = 10;

enum M_LogLevel g_level = M_INFO;

void glfw_error_callback(int error, const char *description) {
  m_logger_error("GLFW Error %d: %s", error, description);
}

const char *get_log_level_str(enum M_LogLevel level) {
  switch (level) {
  case M_INFO:
    return "INFO";
  case M_ERROR:
    return "ERROR";
  case M_WARN:
    return "WARN";
  default:
    return "";
  }
}

void display_formatted_message(enum M_LogLevel level, FILE *file, const char *message, va_list args) {
  time_t now = time(NULL);
  struct tm *local = localtime(&now);

  char output_buffer[TIME_DISPLAY_LEN];
  strftime(output_buffer, TIME_DISPLAY_LEN, "%H:%M:%S", local);

  fprintf(file, "[%s -> %s]: ", output_buffer, get_log_level_str(level));
  vfprintf(file, message, args);
  fprintf(file, "\n");
}

void m_logger_init(enum M_LogLevel level) {
  g_level = level;
  glfwSetErrorCallback(glfw_error_callback);
  m_logger_info("Logger successfully initialised");
}

void m_logger_info(const char *message, ...) {
  if (g_level > M_INFO)
    return;

  va_list args;
  va_start(args, message);

  display_formatted_message(M_INFO, stdout, message, args);

  va_end(args);
}

void m_logger_error(const char *message, ...) {
  if (g_level > M_ERROR)
    return;

  va_list args;
  va_start(args, message);

  fprintf(stderr, "\033[31m");
  display_formatted_message(M_ERROR, stderr, message, args);
  fprintf(stderr, "\033[0m");

  va_end(args);
}

void m_logger_warn(const char *message, ...) {
  if (g_level > M_WARN)
    return;

  va_list args;
  va_start(args, message);

  display_formatted_message(M_WARN, stdout, message, args);

  va_end(args);
}
