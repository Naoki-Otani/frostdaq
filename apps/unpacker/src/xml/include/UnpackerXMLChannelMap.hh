// -*- C++ -*-

// AUthor: Tomonori Takahashi

#ifndef HDDAQ__UNAPCKER_XML_CHANNEL_MAP_H
#define HDDAQ__UNPACKER_XML_CHANNEL_MAP_H

#include <map>
#include <string>
#include <list>

#include "xml_helper.hh"

#include "XMLRead.hh"
#include "Uncopyable.hh"
#include "defines.hh"

namespace hddaq
{

  namespace unpacker
  {
    
    class Unpacker;

    class UnpackerXMLChannelMap
      : public xml::XMLRead,
	private Uncopyable<UnpackerXMLChannelMap>
     {

     public:
       // map : id -> name
       typedef std::map<int, std::string> NameList;
       
       // map : name -> id
       typedef std::map<std::string, int> IndexList;
       
     private:
       std::list<Unpacker*>  m_unpacker_list;
       DigitList& m_digit_list;

       NameList  m_felist_id2name;
       IndexList m_felist_name2id;

     public:
       UnpackerXMLChannelMap(DOMElement* e,
			     DigitList& digit_list);
       ~UnpackerXMLChannelMap();
       
       int                get_fe_id(const std::string& name) const;
       const std::string& get_fe_name(int id) const;

     protected:
       virtual void hoge(const std::string& arg) const;
       virtual void generate_content(DOMElement* e);
       virtual void read(DOMElement* e);
       virtual void read_front_end(DOMElement* e);
      
    };

  }
}
#endif
