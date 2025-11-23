// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__XML_READ_H
#define HDDAQ__XML_READ_H

#include <map>
#include <string>

#include "xml_helper.hh"

#include "Uncopyable.hh"


namespace hddaq
{
  
  namespace xml
  {
    
    class XMLRead
      : private hddaq::Uncopyable<XMLRead>
    {
      
    public:
      typedef hddaq::xml::DOMParser      DOMParser;
      typedef hddaq::xml::DOMDocument    DOMDocument;
      typedef hddaq::xml::DOMElement     DOMElement;
      typedef hddaq::xml::DOMNode        DOMNode;
      typedef hddaq::xml::DOMText        DOMText;
      typedef hddaq::xml::DOMElementList DOMElementList;
      typedef hddaq::xml::DOMAttrList    DOMAttrList;

    protected:
      DOMElement*                   m_e;
      std::map<std::string, double> m_variables;
      int                           m_loop_depth;

    public:
      explicit XMLRead(DOMElement* e);
      virtual ~XMLRead();

    protected:
      virtual void hoge(const std::string& arg="") const;
      virtual void generate_content(DOMElement* e);
      virtual void read(DOMElement* e);
      virtual void read_loop(DOMElement* e);
      virtual void replace_loop_variables(std::string& var);




    };

  }

}
#endif
