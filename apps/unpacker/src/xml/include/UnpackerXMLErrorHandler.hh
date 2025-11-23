// -*- C++ -*-

// Author: Tomonori Takahashi
#ifndef HDDAQ__UNPACKER_XML_ERROR_HANDLER_H
#define HDDAQ__UNPACKER_XML_ERROR_HANDLER_H

#include <string>

#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>

namespace hddaq
{
  namespace unpacker
  {
    class UnpackerXMLErrorHandler
      : public xercesc::ErrorHandler
    {

    private:
      std::string m_name;
      bool m_saw_errors;

    public:
      UnpackerXMLErrorHandler(const std::string& name="UnpackerXMLErrorHandler",
			      bool saw_errors = true);
      virtual ~UnpackerXMLErrorHandler();

      void         hoge() const;
      virtual void error(const xercesc::SAXParseException& to_catch);
      virtual void fatalError(const xercesc::SAXParseException& to_catch);
      bool         get_saw_errors() const;
      virtual void resetErrors();
      virtual void warning(const xercesc::SAXParseException& to_catch);
    };
    
  }
}
#endif
