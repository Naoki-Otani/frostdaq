// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__FILESYSTEM_UTIL_H
#define HDDAQ__FILESYSTEM_UTIL_H

#include <string>
#include <exception>

namespace hddaq
{

  std::string realpath(const std::string& path);
  std::string dirname(const std::string& path);
  std::string basename(const std::string& path);
  std::string readlink(const std::string& path);
  std::string selfpath();

  class FilesystemException
    : public std::exception
  {

  private:
    std::string m_msg;

  public:
    explicit FilesystemException(const std::string& msg);
    virtual ~FilesystemException() throw();

    virtual void hoge(const std::string& arg="") const;
    virtual const char* what() const throw();

  };


}
#endif
