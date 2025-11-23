// -*- C++ -*-

// Author: Tomonori Takahashi

#include "xml_helper.hh"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include "Evaluator.hh"
#include "lexical_cast.hh"
#include "std_ostream.hh"
#include "func_name.hh"

namespace hddaq
{
  namespace xml
  {
//______________________________________________________________________________
    void
    evaluate_attribute(std::string& value)
    {
// 	cout << "evaluate " << value << std::endl;
      static std::vector<std::string> s_xslt_arithmetic_operators;
      if (s_xslt_arithmetic_operators.empty())
	{
	  s_xslt_arithmetic_operators.push_back("+");
	  s_xslt_arithmetic_operators.push_back(" - ");
	  s_xslt_arithmetic_operators.push_back("*");
	  s_xslt_arithmetic_operators.push_back("div");
	  s_xslt_arithmetic_operators.push_back("mod");
	  s_xslt_arithmetic_operators.push_back("and");
	  s_xslt_arithmetic_operators.push_back("or");
	}
      for (unsigned int i=0U; i<s_xslt_arithmetic_operators.size(); ++i)
	if (value.find(s_xslt_arithmetic_operators[i])
	    !=std::string::npos)
	  {
// 	      if (attr_value.find("<")!=std::string::npos)
// 		cout << "#EEEEEE " << attr_value << "?" << std::endl;
	    Evaluator e;
	    std::string ret = d2a(e.evaluate(value.c_str()));
	    
	    if (e.status()!=Evaluator::OK)
	      e.print_error();
//   	cout << " #D " << value << std::endl;
	    ret.swap(value);
//   	cout << " #DD " << value << std::endl;
	  }
      return;
    }



//______________________________________________________________________________
std::string
get_attribute(const DOMElement* const e,
	      const std::string& attr_name)
{
//   cout << "#D " << attr_name << std::endl;
  if (!e)
    return "";
  const xstring ret = e->getAttribute(xstring(attr_name).c_wstr());
  if (!ret.empty())
    return ret.str();
  return "";
}

//______________________________________________________________________________
void
get_attributes(const DOMElement* const e,
	       std::vector<std::string>& name,
	       std::vector<std::string>& value)
{
  const xercesc::DOMNamedNodeMap* attrs = e->getAttributes();
  const ::XMLSize_t n_attr = attrs->getLength();
  for (::XMLSize_t i=0; i<n_attr; ++i)
    {
      xercesc::DOMNode* const n = attrs->item(i);
      if (xercesc::DOMNode::ATTRIBUTE_NODE != n->getNodeType())
	continue;
      xercesc::DOMAttr* attr = dynamic_cast<xercesc::DOMAttr*>(n);
      name.push_back(xstring(attr->getName()).str());
      value.push_back(xstring(attr->getValue()).str());
    }
  return;
}

//______________________________________________________________________________
void
get_attributes(const DOMElement* const e,
	       DOMAttrList& attr)
{
  const xercesc::DOMNamedNodeMap* attrs = e->getAttributes();
  const ::XMLSize_t n_attr = attrs->getLength();
  for (::XMLSize_t i=0; i<n_attr; ++i)
    {
      xercesc::DOMNode* const n = attrs->item(i);
      if (xercesc::DOMNode::ATTRIBUTE_NODE!=n->getNodeType())
	continue;
      xercesc::DOMAttr* a = dynamic_cast<xercesc::DOMAttr*>(n);
      attr[xstring(a->getName()).str()] = xstring(a->getValue()).str();
    }
  return;
}

//______________________________________________________________________________
void
get_children(const DOMElement* const e,
	     DOMElementList& elements)
{
  for (DOMNode* n=e->getFirstChild(); n!=0; n=n->getNextSibling())
    {
      if (DOMNode::ELEMENT_NODE!=n->getNodeType())
	continue;
      DOMElement* child = dynamic_cast<DOMElement*>(n);
      elements.push_back(child);
    }
  return;
}

//______________________________________________________________________________
void
get_elements_by_tag_name(const DOMElement* const e,
			 const std::string& tag_name,
			 DOMElementList& elements)
{
  xercesc::DOMNodeList* l
    = e->getElementsByTagName(xstring(tag_name).c_wstr());
  const ::XMLSize_t len = l->getLength();
  elements.resize(len);
  for (::XMLSize_t i = 0; i<len; ++i)
    {
      DOMNode* n = l->item(i);
      if (xercesc::DOMNode::ELEMENT_NODE!=n->getNodeType())
	continue;
      xercesc::DOMElement* c = dynamic_cast<xercesc::DOMElement*>(n);
      elements[i] = c;
    }
  return;
}

//______________________________________________________________________________
DOMElement*
get_parent(const DOMElement* const e)
{
  return dynamic_cast<DOMElement*>(e->getParentNode());
}

//______________________________________________________________________________
std::string
get_tag_name(const DOMElement* const e)
{
  const xstring ret = e->getTagName();
  return ret.str();
}

//______________________________________________________________________________
std::string
get_text_data(const DOMElement* const e)
{
  DOMText* t = 0;
  std::string ret;
  for (DOMNode* n = e->getFirstChild(); n!=0; n=n->getNextSibling())
    {
      if (n->getNodeType()!=DOMNode::TEXT_NODE)
	continue;
      t = dynamic_cast<DOMText*>(n);

      ret += get_text_data(t);
    }

  return ret;
}

//______________________________________________________________________________
std::string
get_text_data(const DOMText* const t)
{
  return xstring(t->getData()).str();
}


//______________________________________________________________________________
DOMElement*
initialize_parser(DOMParser& parser,
		  const xercesc::ErrorHandler& err_handler,
		  const std::string& filename,
		  const std::string& s,
		  bool is_validation_required)
{
  static const std::string ks_func_name
    = make_func_name(__func__, "hddaq::xml");
  parser.setDoNamespaces(true);
  parser.setCreateEntityReferenceNodes(false);

  if (is_validation_required)
    {
      parser.setValidationScheme(xercesc::XercesDOMParser::Val_Always);
      parser.setValidationSchemaFullChecking(true);
      parser.setDoSchema(true);
    }
  else
    {
      parser.setDoSchema(false);
      parser.setValidationSchemaFullChecking(false);
    }
  

  parser.setErrorHandler(const_cast<xercesc::ErrorHandler*>(&err_handler));

  try 
    {
      parser.parse(filename.c_str());
    }
  catch (const xercesc::OutOfMemoryException& e)
    {
      std::cerr << "\n#E " << s << "\n"
		<< ks_func_name << "\n"
		<< " xercesc::OutOfMemoryException" << std::endl;
    }
  catch (const xercesc::XMLException& e)
    {
      std::cerr << "\n#E " << s << "\n"
		<< ks_func_name << "\n"
		<< " An error occured during parsing\n Message: "
		<< xstring(e.getMessage()) << std::endl;
    }
  catch (const xercesc::DOMException& e)
    {
      std::cerr << "\n#E " << s << "\n"
		<< ks_func_name << "\n"
		<< " DOM Error during parsing: \"" << filename << "\"\n"
		<< " DOMException code is " << e.code << std::endl;
    }
  catch (...)
    {
      std::cerr << "\n#E " << s << "\n"
		<< ks_func_name << "\n"
		<< " An error occured during parsing" << std::endl;
    }

  xercesc::DOMDocument* doc = parser.getDocument();
  
  if (!doc)
    {
      std::cerr << "\n#E " << s << "\n"
		<< ks_func_name << "\n"
		<< " Unable to open document: " << filename << std::endl;
    }
  xercesc::DOMElement* root = doc->getDocumentElement();
  if (!root)
    {
      std::cerr << "\n#E " << s << "\n"
		<< ks_func_name << "\n"
		<< " empty document" << std::endl;
    }

  return root;
}

//______________________________________________________________________________
void
initialize_xml(const std::string& s)
{
  static const std::string ks_func_name("hddaq::xml::initialize_xml()");
  try
    {
      xercesc::XMLPlatformUtils::Initialize();
    }
  catch (const xercesc::XMLException& e)
    {
      std::cerr << "\n#E initialize_xml(" << s << ")\n" 
		<< ks_func_name << "\n"
		<< xstring(e.getMessage())
		<< std::endl;
    }
  return;
}

//______________________________________________________________________________
void
terminate_xml()
{
  xercesc::XMLPlatformUtils::Terminate();
  return;
}


//______________________________________________________________________________
//______________________________________________________________________________
xstring::xstring()
  : m_local_form(),
    m_form(0)
{
}

//______________________________________________________________________________
xstring::xstring(const xstring& arg)
  : m_local_form(),
    m_form(0)
{
  *this = arg.m_local_form;
}

//______________________________________________________________________________
xstring::xstring(const ::XMLCh* const to_transcode)
  : m_local_form(),
    m_form(0)
{
  char* temp = xercesc::XMLString::transcode(to_transcode);
  m_local_form = temp;
  m_form = xercesc::XMLString::transcode(temp);
  xercesc::XMLString::release(&temp);
}

//______________________________________________________________________________
xstring::xstring(const char* const to_transcode)
  : m_local_form(to_transcode),
    m_form(xercesc::XMLString::transcode(m_local_form.c_str()))
{
}

//______________________________________________________________________________
xstring::xstring(const std::string& to_transcode)
  : m_local_form(to_transcode),
    m_form(xercesc::XMLString::transcode(to_transcode.c_str()))
{
}

//______________________________________________________________________________
xstring::~xstring()
{
  if (m_form)
    xercesc::XMLString::release(&m_form);
}

//______________________________________________________________________________
const char*
xstring::c_str() const
{
  return m_local_form.c_str();
}

//______________________________________________________________________________
const ::XMLCh*
xstring::c_wstr() const
{
    return m_form;
}

//______________________________________________________________________________
bool
xstring::empty() const
{
  return m_local_form.empty();
}

//______________________________________________________________________________
std::string::size_type
xstring::find(const std::string& s) const
{
  return m_local_form.find(s);
}

//______________________________________________________________________________
std::string
xstring::str() const
{
  return m_local_form;
}

//______________________________________________________________________________
void
xstring::operator=(const xstring& arg)
{
  *this = arg.m_local_form;
  return;
}

//______________________________________________________________________________
void
xstring::operator=(const ::XMLCh* const to_transcode)
{
  if (m_form)
    xercesc::XMLString::release(&m_form);
  char* temp = xercesc::XMLString::transcode(to_transcode);
  m_local_form = temp;
  m_form = xercesc::XMLString::transcode(m_local_form.c_str());
  xercesc::XMLString::release(&temp);
  return;
}

//______________________________________________________________________________
void
xstring::operator=(const char* const to_transcode)
{
  if (m_form)
    xercesc::XMLString::release(&m_form);
  m_local_form = to_transcode;
  m_form = xercesc::XMLString::transcode(to_transcode);
  return;
}

//______________________________________________________________________________
void
xstring::operator=(const std::string& to_transcode)
{
  if (m_form)
    xercesc::XMLString::release(&m_form);
  m_local_form = to_transcode;
  m_form = xercesc::XMLString::transcode(to_transcode.c_str());
  return;
}

  }
}
