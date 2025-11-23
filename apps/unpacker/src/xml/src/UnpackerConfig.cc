// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerConfig.hh"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>

#include "std_ostream.hh"
#include "UnpackerXMLErrorHandler.hh"
#include "UnpackerXMLReadDigit.hh"
#include "UnpackerXMLChannelMap.hh"
#include "lexical_cast.hh"
#include "UnpackerManager.hh"
#include "defines.hh"

namespace hddaq
{
  namespace unpacker
  {
    typedef defines::check_mode_t check_mode_t;


//______________________________________________________________________________
UnpackerConfig::UnpackerConfig()
  : m_parser(0),
    m_digit(0),
    m_channel_map(0),
    m_run_range(),
    m_control()
{
  xml::initialize_xml("UnpackerConfig");
}

//______________________________________________________________________________
UnpackerConfig::~UnpackerConfig()
{
  if (m_parser)
    delete m_parser;
  m_parser = 0;
  
//   cout << "#D UnpackerConfig::~UnpackerConfig()" << std::endl;
  if (m_digit)
    delete m_digit;
  m_digit = 0;
  
  if (m_channel_map)
    delete m_channel_map;
  m_channel_map = 0;

  xml::terminate_xml();
}

//______________________________________________________________________________
void
UnpackerConfig::hoge(const std::string& arg) const
{
  cout << "#D UnpackerConfig::hoge()  " << arg << std::endl;
  return;
}

//______________________________________________________________________________
const std::string&
UnpackerConfig::get_control_param(const std::string& key) const
{
  std::map<std::string, std::string>::const_iterator i = m_control.find(key);
  if (i==m_control.end())
    {
//       cout << "\n#W UnpackerConfig::get_control_param()\n"
// 	   << "  cannot find " << key << std::endl;
      return key;
    }
  return i->second;
}

//______________________________________________________________________________
const UnpackerConfig::DigitInfo&
UnpackerConfig::get_digit_info() const
{
  return *(m_digit);
}

//______________________________________________________________________________
const UnpackerConfig::ChannelMap&
UnpackerConfig::get_channel_map() const
{
  return *(m_channel_map);
}

//______________________________________________________________________________
const std::vector<std::pair<int, int> >&
UnpackerConfig::get_run_range() const
{
  return m_run_range;
}

//______________________________________________________________________________
void
UnpackerConfig::initialize(DigitList& digit_list,
			   const std::string& config_file,
			   const std::string& digit_file,
			   const std::string& channel_map_file)
{
  m_parser = new xml::DOMParser;
  UnpackerXMLErrorHandler err_handler;
  const bool is_validation_required = true;
  xml::DOMElement* root 
    =  xml::initialize_parser(*m_parser, err_handler, config_file,
			      "UnpackerConfig::initialize("+config_file+")",
			      is_validation_required);

  xml::DOMElementList children;
  xml::get_children(root, children);

  std::string cfile = channel_map_file;
  std::string dfile = digit_file;

  int n = children.size();
  std::cout << "#D children size = " << n << std::endl;
  for (int i=0; i<n; ++i)
    {
      xml::DOMElement* const e = children[i];
      const std::string& tag = xml::get_tag_name(e);
       cout << "#D  parsing XML DOM Element " << tag << std::endl;
      std::string filename;
//       if (tag=="xi:include")
// 	filename = xml::get_attribute(e, "href");
      if (filename.empty())
	filename = xml::get_attribute(e, "file");
      cout << filename << std::endl;

      if      (tag=="control")
	read_control(e);
      else if (tag=="digit_data")
//       else if ((tag=="digit_data")
// 	       || (filename.find("digit")!=std::string::npos))
	{
	  if (filename.empty())
	    m_digit       = new DigitInfo(e, digit_list);
	  else
	    dfile = filename;
	}
      else if ((tag=="front_end") || (tag=="channel_map"))
//       else if ((tag=="front_end") || (tag=="channel_map")
// 	       || (filename.find("front_end")!=std::string::npos)
// 	       || (filename.find("channle_map")!=std::string::npos))
	{
	  if (filename.empty())
	    m_channel_map = new ChannelMap(e, digit_list);
	  else
	    cfile = filename;
	}

    }
  
  if (!m_digit && !dfile.empty())
    {
      xml::DOMParser parser;
      xml::DOMElement* e
	= xml::initialize_parser(parser, err_handler, dfile,
				 "UnpackerConfig::initialize("+dfile+")",
				 is_validation_required);
      //      cout << " init digit" << std::endl;
      if (!m_digit){
	m_digit = new DigitInfo(e, digit_list);      
      }
    }

  if (!m_channel_map && !cfile.empty())
    {
      xml::DOMParser parser;
      xml::DOMElement* e
	= xml::initialize_parser(parser, err_handler, cfile,
				 "UnpackerConfig::initialize(" + cfile +")",
				 is_validation_required);

      //      cout << " init frontend" << std::endl;
      if (!m_channel_map){
	m_channel_map = new ChannelMap(e,  digit_list);
      }
    }
  return;
}

//______________________________________________________________________________
void
UnpackerConfig::read_control(xml::DOMElement* control_root)
{
  const std::string& root_tag = xml::get_tag_name(control_root);
  cout << "#D UnpackerConfig::read_control()\n" 
       << " reading <" << root_tag << ">\n";
  

  xml::DOMElementList children;
  xml::get_children(control_root, children);
  
  cout << "#D children size = " << children.size() << "\n";
  for (unsigned int i=0; i<children.size(); ++i)
    {
      xml::DOMElement* e = children[i];
      const std::string& tag = xml::get_tag_name(e);
      cout << "#D " <<  tag  << "\n";
      if (tag=="event")
	{
	  xml::DOMAttrList attr;
	  xml::get_attributes(e, attr);
	  for (xml::DOMAttrList::iterator i = attr.begin(); i!=attr.end(); ++i)
	    m_control[i->first] = i->second;
	}
      else if (tag=="run")
	{
	  int from = a2i(xml::get_attribute(e, "from"));
	  int to   = a2i(xml::get_attribute(e, "to"));
	  m_run_range.push_back(std::make_pair(from, to));
	}
      else if (tag=="error_check")
	{
	  check_mode_t mode;
	  xml::DOMElementList errList;
	  xml::get_children(e, errList);
	  for (unsigned int j=0; j<errList.size(); ++j)
	    {
	      xml::DOMElement* err_elem = errList[j];
	      const std::string& etag = xml::get_tag_name(err_elem);
	      if (etag=="format")
		{
		  const std::string is_check
		    = xml::get_attribute(err_elem, "check");
		  if (is_check=="true" || is_check=="1")
		    {
		      cout << "   check : data format " << "\n";
		      mode.set(defines::k_format);
		    }
		}
	      if (etag=="slip")
		{
		  const std::string is_check
		    = xml::get_attribute(err_elem, "check");
		  if (is_check=="true" || is_check=="1")
		    {
		      cout << "   check : tag slip" << "\n";
		      mode.set(defines::k_slip1);
		    }
		}
	      if (etag=="disagreement")
		{
		  const std::string local
		    = xml::get_attribute(err_elem, "local");
		  if (local=="true" || local=="1")
		    {
		      cout << "   check : local tag disagreement" << "\n";
		      mode.set(defines::k_local);
		    }
		  const std::string event
		    = xml::get_attribute(err_elem, "event");
		  if (event=="true" || event=="1")
		    {
		      cout << "   check : event tag disagreement" << "\n";
		      mode.set(defines::k_event);
		    }
		  const std::string spill
		    = xml::get_attribute(err_elem, "spill");
		  if (spill=="true" || spill=="1")
		    {
		      cout << "   check : spill tag disagreement" << "\n";
		      mode.set(defines::k_spill);
		    }
		}
	      if (etag=="show")
		{
		  m_control["char_true"]
		    = xml::get_attribute(err_elem, "char_true");
		  m_control["char_false"]
		    = xml::get_attribute(err_elem, "char_false");
		  const std::string all_module
		    = xml::get_attribute(err_elem, "all");
		  if (all_module=="true" || all_module=="1")
		    {
		      cout << "   show all module" << "\n";
		      mode.set(defines::k_show_all);
		    }
		    
		  const std::string always
		    = xml::get_attribute(err_elem, "always");
		  if (always=="true" || always=="1")
		    {
		      cout << "   show always" << "\n";
		      mode.set(defines::k_show_always);
		    }
		}
	    }
	  m_control["error_check"] = mode.to_string();
	}
      else
	{
	  std::istringstream ss(xml::get_text_data(e));
	  std::string val;
	  ss >> val;
	  cout << "   " << tag << ": " << val << "\n";
	  m_control[tag] = val;
	}
    }

  const std::string& cout_name = m_control["cout"];
  set_cout(cout_name);
  cout << "   cout = " << cout_name << "\n";
  const std::string& cerr_name = m_control["cerr"];
  cout << "   cerr = " << cerr_name << "\n";
  set_cerr(cerr_name);

  std::string& tag_summary_name = m_control["tag_summary"];
  if (tag_summary_name.empty())
    tag_summary_name = m_control["tout"];
  if (tag_summary_name.empty())
    tag_summary_name = m_control["tag"];

  cout << "   tag_summary = " << tag_summary_name << "\n";
  set_tag_summary(tag_summary_name);

  cout << std::endl;

//   if (m_run_range.empty())
//     cerr << "#Warning UnpackerConfig::initialize()\n"
// 	 << " ranges of run number are not specified" << std::endl;
  return;
}

}
}

