// -*- C++ -*-

// Author: Tomonori Takahashi

#include "filesystem_util.hh"

#include <iostream>
#include <sstream>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <cstdlib>

namespace hddaq
{

//______________________________________________________________________________
std::string
realpath(const std::string& path)
{
  std::vector<char> resolved(PATH_MAX);
  char* ret = ::realpath(path.c_str(), &(resolved[0]));
  if (!ret)
    {
      std::ostringstream msg;
      msg << "\n during hddaq::realpath() \n"
	  << " cannot resolve: " << path
	  << "\n no sush file" ;
      throw FilesystemException(msg.str());
    }
  return ret;
}

//______________________________________________________________________________
std::string
dirname(const std::string& path)
{

  try
    {
      std::string real = realpath(path);
      //  const std::string::size_type first_slash = real.find("/");
      const std::string::size_type last_slash = real.find_last_of("/");
      struct ::stat s;
      if (::stat(real.c_str(), &s)!=-1)
	{
	  switch (s.st_mode & S_IFMT)
	    {
	    case S_IFDIR:
	      return real;
	      break;
	    case S_IFREG:
	      return real.substr(0, last_slash);
	      break;
	    default:
	      std::cout << "#D path = " << path
			<< "is niether directory nor regular file" << std::endl;
	      break;
	    }
	}
    }
  catch (const std::exception& e)
    {
      std::ostringstream msg;
      msg << e.what();
      msg << " during hddaq::dirname()";
      throw FilesystemException(msg.str());
    }
  return "";
}

//______________________________________________________________________________
std::string
basename(const std::string& path)
{
  try
    {
      std::string real = realpath(path);
      std::string::size_type last_slash = real.find_last_of("/");
      struct ::stat s;

      if (::stat(real.c_str(), &s)!=-1)
	{
	  switch (s.st_mode & S_IFMT)
	    {
	    case S_IFREG:
	      if (last_slash==std::string::npos)
		return real.substr(0);
	      else
		return real.substr(last_slash+1);
	      break;
	    default:
	      std::cout << "#D path = " << path
			<< "is not regular file" << std::endl;
	      break;
	    }
	}
    }
  catch (const std::exception& e)
    {
      std::ostringstream msg;
      msg << e.what();
      msg << " during hddaq::basename()";
      throw FilesystemException(msg.str());
    }

  return "";
}

//______________________________________________________________________________
std::string
readlink(const std::string& path)
{
  char buf[PATH_MAX];
  ssize_t len = ::readlink(path.c_str(), buf, sizeof(buf)-1);
  if ( len==-1 )
    {
      std::ostringstream msg;
      msg << "\n during hddaq::readlink() \n"
	  << " cannot resolve " << path
	  << "\n no such file";
      throw FilesystemException(msg.str());
    }

  buf[len] = '\0';
  return std::string(buf);
}

//______________________________________________________________________________
std::string
selfpath()
{
  try
    {
      return readlink("/proc/self/exe");
    }
  catch(const std::exception& e)
    {
      std::ostringstream msg;
      msg << e.what() << " during hddaq::selfpath()";
      throw FilesystemException(msg.str());
    }
}

//______________________________________________________________________________
FilesystemException::FilesystemException(const std::string& msg)
  : m_msg()
{
  m_msg = "hddaq::FilesystemException:: " + msg;
}

//______________________________________________________________________________
FilesystemException::~FilesystemException() throw()
{
}

//______________________________________________________________________________
void
FilesystemException::hoge(const std::string& arg) const
{
  std::cout << "#D FilesystemException::hoge(" << arg << ")" << std::endl;
  return;
}

//______________________________________________________________________________
const char*
FilesystemException::what() const throw()
{
  return m_msg.c_str();
}

}
