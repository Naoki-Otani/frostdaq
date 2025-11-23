// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerXMLErrorHandler.hh"

#include <iostream>

#include <cstdlib>

#include <xercesc/sax/SAXParseException.hpp>

#include "std_ostream.hh"
#include "xml_helper.hh"

namespace hddaq
{
  namespace unpacker
  {

//______________________________________________________________________________
UnpackerXMLErrorHandler::UnpackerXMLErrorHandler(const std::string& name,
						 bool saw_errors)
  : xercesc::ErrorHandler(),
    m_name(),
    m_saw_errors(saw_errors)
{}

//______________________________________________________________________________
UnpackerXMLErrorHandler::~UnpackerXMLErrorHandler()
{
}

//______________________________________________________________________________
void
UnpackerXMLErrorHandler::hoge() const
{
  cout << "#D " << m_name << "::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
void
UnpackerXMLErrorHandler::error(const xercesc::SAXParseException& to_catch)
{
  if (!m_saw_errors)
    return;

  cerr << "\n#E : error at file \"" 
       << xml::xstring(to_catch.getSystemId())
       << "\"" 
       << "\n     line: " << to_catch.getLineNumber()
       << "\n     column: " << to_catch.getColumnNumber()
       << "\n   Message: " << xml::xstring(to_catch.getMessage())
       << std::endl;

//   for (;;)
//     {
//       std::string answer;
//       std::cout << "\n exit ? [y/n]" << std::flush;
//       std::getline(std::cin, answer);
//       if (amswer.find("y")==0 || answer.find("Y")==0)
// 	std::exit(0);
//       else
// 	std::cout << "\n unknown input: " << answer << std::endl;
//     }

  std::exit(1);

  return;
}


//______________________________________________________________________________
void
UnpackerXMLErrorHandler::fatalError(const xercesc::SAXParseException& to_catch)
{
  if (!m_saw_errors)
    return;

  cerr << "\n#E : fatal error at file \"" 
       << xml::xstring(to_catch.getSystemId())
       << "\"" 
       << "\n     line: " << to_catch.getLineNumber()
       << "\n     column: " << to_catch.getColumnNumber()
       << "\n   Message: " << xml::xstring(to_catch.getMessage())
       << "\n\n exit" 
       << std::endl;
  std::exit(1);
  return;
}

//______________________________________________________________________________
bool
UnpackerXMLErrorHandler::get_saw_errors() const
{
  return m_saw_errors;
}

//______________________________________________________________________________
void
UnpackerXMLErrorHandler::resetErrors()
{
  m_saw_errors = false;
  return;
}

//______________________________________________________________________________
void
UnpackerXMLErrorHandler::warning(const xercesc::SAXParseException& to_catch)
{
  if (!m_saw_errors)
    return;

  cout << "W : validation warning : " << xml::xstring(to_catch.getMessage())
       << " at line: " << to_catch.getLineNumber() << std::endl;
  return;
}

  }
}
