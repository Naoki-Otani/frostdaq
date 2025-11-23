// -*- C++ -*-

#ifndef HDDAQ__FONT_COLOR_H
#define HDDAQ__FONT_COLOR_H

#include <string>

namespace hddaq
{
  namespace unpacker
  {

    namespace esc {

      const std::string k_beep          = "\007";
      const std::string k_black         = "\033[0;30m";
      const std::string k_blue          = "\033[0;34m";
      const std::string k_green         = "\033[0;32m";
      const std::string k_cyan          = "\033[0;36m";
      const std::string k_red           = "\033[0;31m";
      const std::string k_purple        = "\033[0;35m";
      const std::string k_brown         = "\033[0;33m";
      const std::string k_light_gray    = "\033[0;37m";
      const std::string k_dark_gray     = "\033[1;30m";
      const std::string k_light_blue    = "\033[1;34m";
      const std::string k_light_green   = "\033[1;32m";
      const std::string k_light_cyan    = "\033[1;36m";
      const std::string k_light_red     = "\033[1;31m";
      const std::string k_light_purple  = "\033[1;35m";
      const std::string k_yellow        = "\033[1;33m";
      const std::string k_white         = "\033[1;37m";
      const std::string k_default_color = "\033[0m";
    }
    
  }
};
#endif
