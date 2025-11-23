// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_CONFIG_H
#define HDDAQ__UNPACKER_CONFIG_H

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include "xml_helper.hh"

#include "Uncopyable.hh"
#include "Singleton.hh"
#include "defines.hh"

namespace hddaq
{
  namespace unpacker
  {

    class UnpackerXMLReadDigit;
    class UnpackerXMLChannelMap;

    class UnpackerConfig
      : private Uncopyable<UnpackerConfig>
    {

    public:
      typedef defines::unpacker_type unpacker_type;
      typedef UnpackerXMLReadDigit  DigitInfo;
      typedef UnpackerXMLChannelMap ChannelMap;

    private:
      friend class Singleton<UnpackerConfig>;
      xml::DOMParser* m_parser;
      DigitInfo*      m_digit;
      ChannelMap*     m_channel_map;

      std::vector<std::pair<int, int> >                      m_run_range;
      std::map<std::string, std::string>                     m_control;

    public:
      ~UnpackerConfig();

      void hoge(const std::string& arg="") const;
      const std::string&
           get_control_param(const std::string& key) const;
      const DigitInfo&  get_digit_info() const;
      const ChannelMap& get_channel_map() const;
      const std::vector<std::pair<int, int> >&
           get_run_range() const;
      void initialize(DigitList& device_list,
		      const std::string& config_file,
		      const std::string& digit_file="",
		      const std::string& channel_map_file="");

    private:
      UnpackerConfig();
      void read_control(xml::DOMElement* control_root);
      void read_elec(const std::string& filename);

    };

//______________________________________________________________________________
  typedef Singleton<UnpackerConfig> GConfig;


  }

}
#endif
