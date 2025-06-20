#include "app.hh"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <switch.h>

extern "C" {
char *wpp_input_handler() {
  Result ret = 0;

  SwkbdConfig swkbd;
  char *input_string = new char[256];
  memset(input_string, 0, 256);

  if (R_FAILED(ret = swkbdCreate(std::addressof(swkbd), 0))) {
    swkbdClose(std::addressof(swkbd));
    return input_string;
  }

  swkbdConfigMakePresetDefault(std::addressof(swkbd));
  /*
if (!guide_text.empty())
swkbdConfigSetGuideText(std::addressof(swkbd), guide_text.c_str());
          */

  /*
if (!initial_text.empty())
swkbdConfigSetInitialText(std::addressof(swkbd), initial_text.c_str());
          */

  if (R_FAILED(ret = swkbdShow(std::addressof(swkbd), input_string, 256))) {
    swkbdClose(std::addressof(swkbd));
    return input_string;
  }

  swkbdClose(std::addressof(swkbd));
  return input_string;
}

void wpp_printf_handler(const char *format, ...) {
  va_list args;
  va_start(args, format);

  // First, determine the size needed for the formatted string
  int size = std::vsnprintf(nullptr, 0, format, args);
  if (size < 0) {
    // Handle error in formatting
    va_end(args);
    return;
  }

  // Allocate a buffer of the required size (+1 for null terminator)
  std::unique_ptr<char[]> buffer(new char[size + 1]);

  // Format the string into the buffer
  std::vsnprintf(buffer.get(), size + 1, format, args);

  // Append the formatted string to the std::string
  output.append(buffer.get());

  // Clean up the variable argument list
  va_end(args);
}
}
