// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__XML_HELPER_H
#define HDDAQ__XML_HELPER_H

#include <map>
#include <string>
#include <vector>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

//class xercesc::ErrorHandler;

namespace hddaq
{

  namespace xml
  {
    typedef xercesc::XercesDOMParser           DOMParser;
    typedef xercesc::DOMDocument               DOMDocument;
    typedef xercesc::DOMElement                DOMElement;
    typedef xercesc::DOMNode                   DOMNode;
    typedef xercesc::DOMText                   DOMText;
    typedef std::vector<DOMElement*>           DOMElementList;
    typedef std::map<std::string, std::string> DOMAttrList;

    void
    evaluate_attribute(std::string& value);
    

    std::string
    get_attribute(const DOMElement* const e,
		  const std::string& attr_name);
    
    void
    get_attributes(const DOMElement* const e,
		   std::vector<std::string>& name,
		   std::vector<std::string>& value);
    
    void
    get_attributes(const DOMElement* const e,
		   DOMAttrList& attr);

    void
    get_children(const DOMElement* const e,
		 DOMElementList& elements);
    void
    get_elements_by_tag_name(const DOMElement* const e,
			     const std::string& tag_name,
			     DOMElementList& elements);
    
    DOMElement*
    get_parent(const DOMElement* const e);

    std::string
    get_tag_name(const DOMElement* const e);
    
    std::string
    get_text_data(const DOMElement* const e);

    std::string
    get_text_data(const DOMText* const t);

    DOMElement*
    initialize_parser(DOMParser& parser,
		      const xercesc::ErrorHandler& err_handler,
		      const std::string& filename,
		      const std::string& s="",
		      bool is_validation_required=false);
    
    void
    initialize_xml(const std::string& s="");

    void
    terminate_xml();

//_____________________________________________________________________________
 class xstring
{

private:
  std::string m_local_form;
  ::XMLCh*    m_form;

public:
  xstring();
  xstring(const ::XMLCh* const to_transcode);
  xstring(const char* const    to_transcode);
  xstring(const std::string& arg);
  xstring(const xstring& arg);
  ~xstring();

  const char*            c_str() const;
  const ::XMLCh*         c_wstr() const;
  bool                   empty() const;
  std::string::size_type find(const std::string& s) const;
  std::string            str() const;

  void operator=(const xstring& arg);
  void operator=(const ::XMLCh* const to_transcode);
  void operator=(const char* const    to_transcode);
  void operator=(const std::string&   to_transcode);
};

//______________________________________________________________________________
inline std::ostream& 
operator<<(std::ostream& target,
	   const xstring& to_dump)
{
  target << to_dump.str();
  return target;
}

// --- operator==()
//______________________________________________________________________________
inline bool
operator==(const xstring& lhs,
	   const xstring& rhs)
{
  return lhs.str()==rhs.str();
}


//______________________________________________________________________________
inline bool
operator==(const std::string& lhs,
	   const xstring& rhs)
{
  return lhs==rhs.str();
}

//______________________________________________________________________________
inline bool
operator==(const xstring& lhs,
	   const std::string& rhs)
{
  return lhs.str()==rhs;
}

//______________________________________________________________________________
inline bool
operator==(const char* lhs,
	   const xstring& rhs)
{
  return lhs==rhs.str();
}

//______________________________________________________________________________
inline bool
operator==(const xstring& lhs,
	   const char* rhs)
{
  return lhs.str()==rhs;
}

// --- operator!=()
//______________________________________________________________________________
inline bool
operator!=(const xstring& lhs,
	   const xstring& rhs)
{
  return lhs.str()!=rhs.str();
}

//______________________________________________________________________________
inline bool
operator!=(const std::string& lhs,
	   const xstring& rhs)
{
  return lhs!=rhs.str();
}

//______________________________________________________________________________
inline bool
operator!=(const xstring& lhs,
	   const std::string& rhs)
{
  return lhs.str()!=rhs;
}

//______________________________________________________________________________
inline bool
operator!=(const char* lhs,
	   const xstring& rhs)
{
  return lhs!=rhs.str();
}

//______________________________________________________________________________
inline bool
operator!=(const xstring& lhs,
	   const char* rhs)
{
  return lhs.str()!=rhs;
}


  }
}
#endif
