// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerXMLChannelMap.hh"

#include <algorithm>
#include <vector>
#include <stdexcept>

#include <cstdlib>

#include "std_ostream.hh"
#include "lexical_cast.hh"
#include "Evaluator.hh"
#include "UnpackerManager.hh"
#include "UnpackerConfig.hh"
#include "UnpackerXMLReadDigit.hh"
#include "Unpacker.hh"
#include "replace_string.hh"

namespace hddaq
{
  namespace unpacker
  {
    namespace
    {
      enum e_ref
	{
	  k_device,
	  k_plane,
	  k_segment,
	  k_ch,
	  k_data,
	  k_n_ref
	};
//______________________________________________________________________________
void
print(int idevice,
      int iplane,
      int isegment,
      int ich,
      int idata,
      int fe_ch,
      int fe_data,
      const std::string& name="",
      const std::string& device="",
      const std::string& plane="",
      const std::string& segment="",
      const std::string& ch="",
      const std::string& data="")
{
  cout << "\n#D UnpackerXMLChannelMap::generate_content()\n"
       << name << "\n"
       << " FE ch_id = " << fe_ch << ", data_id =  " << fe_data << "\n"
       << " D device_id = " << idevice
       << ", plane_id = " << iplane
       << ", segmend_id =  " << isegment
       << ", ch_id = " << ich
       << ", data_id = " << idata
       << "\n Dref " << device
       << ", " << plane
       << ", " << segment
       << ", " << ch
       << ", " << data
       << std::endl;
  return;
}

    } // end of anonymous namespace
//______________________________________________________________________________
UnpackerXMLChannelMap::UnpackerXMLChannelMap(DOMElement* e,
					     DigitList& digit_list)
  : XMLRead(e),
    m_unpacker_list(),
    m_digit_list(digit_list)
{
  UnpackerConfig& g_config = GConfig::get_instance();
  const UnpackerConfig::DigitInfo& digit_info = g_config.get_digit_info();
  Unpacker::set_null_device_id(digit_info.get_null_device_id());

  const std::string& value    = xml::get_attribute(e, "dump");
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  const std::string& tag = xml::get_tag_name(e);
  if (false
      || value.find("y")==0
      || value.find("Y")==0
      || value.find("h")==0
      || value.find("H")==0
      || value.find("x")!=std::string::npos
      || value.find("X")!=std::string::npos)
    g_unpacker.set_dump_mode(defines::k_hex);
  else if (false
	   || value.find("d")==0
	   || value.find("D")==0)
    g_unpacker.set_dump_mode(defines::k_dec);
  else if (false
	   || value.find("b")==0
	   || value.find("B")==0)
    g_unpacker.set_dump_mode(defines::k_binary);

  cout << "#D UnpackerXMLChannelMap::UnpackerXMLChannelMap()" << std::endl;
  try
    {
      cout << " reading <" << tag <<  ">" << std::endl;
      read(e);
    }
  catch (const std::exception& e)
    {
      cerr << "\n#E UnpackerXMLChannelMap::UnpackerXMLChannelMap()\n"
	   << " what = " << e.what()
	   << std::endl;
    }

  cout << "\n#D UnpackerXMLChannelMap constructor  finished" << std::endl;
}

//______________________________________________________________________________
UnpackerXMLChannelMap::~UnpackerXMLChannelMap()
{
}

//______________________________________________________________________________
int
UnpackerXMLChannelMap::get_fe_id(const std::string& name) const
{
  std::map<std::string, int>::const_iterator itr = m_felist_name2id.find(name);
  if( itr == m_felist_name2id.end()){
    cerr << "\n#E UnpackerXMLChannelMap::get_fe_id()\n"
	 << " cannot find " << name
	 << std::endl;
    std::exit(1);
  }

  return itr->second;
}

//______________________________________________________________________________
const std::string&
UnpackerXMLChannelMap::get_fe_name(int id) const
{
  std::map<int, std::string>::const_iterator itr = m_felist_id2name.find(id);
  if( itr == m_felist_id2name.end()){
    cerr << "\n#E UnpackerXMLChannelMap::get_fe_name()\n"
	 << " cannot find " << id
	 << std::endl;
    std::exit(1);
  }

  return itr->second;
}

//______________________________________________________________________________
void
UnpackerXMLChannelMap::hoge(const std::string& arg) const
{
  cout << "#D UnpackerXMLChannelMap:hoge()  " << arg << std::endl;
  return;
}

//______________________________________________________________________________
void
UnpackerXMLChannelMap::generate_content(DOMElement* e)
{
  UnpackerConfig& g_config = GConfig::get_instance();
  const UnpackerConfig::DigitInfo& digit_info = g_config.get_digit_info();
  Unpacker* u = m_unpacker_list.back();
//   const std::string tag = xml::get_tag_name(e);
//   if (tag!="FE")
//     return;
  //  read attributes and replace loop variables(characters) with integers
  static int s_n=0;
  if (s_n%100==0)
    {
      //    cout << "#D " << s_n << std::endl;
      if (s_n%8000==0) cout << std::endl;
      cout << ".";
      cout.flush();
    }
  ++s_n;

  std::vector<std::string> names;
  std::vector<std::string> values;
  xml::get_attributes(e, names, values);
  const int n_attr_fe = names.size();

  DOMElement* d=0;
  for (DOMNode* n=e->getFirstChild(); n!=0;n=n->getNextSibling())
    {
      if (DOMNode::ELEMENT_NODE!=n->getNodeType()) continue;
      d = dynamic_cast<DOMElement*>(n);
      break;
    }
  std::vector<std::string> d_names;
  std::vector<std::string> d_values;
  xml::get_attributes(d, d_names, d_values);
  const int n_attr_device = d_names.size();
  if (m_loop_depth!=0)
    {
      //       cout << "\n replace fe" << std::endl;
      for (int i=0; i<n_attr_fe; ++i) replace_loop_variables(values[i]);
//       for (int i=0; i<n_attr_fe; ++i)
// 	{
// 	  cout << " attr_fe name[" << i << "] = " << names[i]
// 	       << ", val = " << values[i] << std::endl;
// 	}

//       cout << "\n replace device" << std::endl;
      for (int i=0; i<n_attr_device; ++i) replace_loop_variables(d_values[i]);
//       for (int i=0; i<n_attr_device; ++i)
// 	{
// 	  cout << " attr_device name[" << i << "] = " << d_names[i]
// 	       << ", val = " << d_values[i] << std::endl;
// 	}

//       cout << "\n eval fe" << std::endl;
      for (int i=0; i<n_attr_fe; ++i) xml::evaluate_attribute(values[i]);
      //      for (int i=0; i<n_attr_fe; ++i)
      //       	{
      //       	  cout << " attr_fe name[" << i << "] = " << names[i]
      //       	       << ", val = " << values[i] << std::endl;
      //       	}

      //       cout << "\n eval device" << std::endl;
      for (int i=0; i<n_attr_device; ++i) xml::evaluate_attribute(d_values[i]);
      //             for (int i=0; i<n_attr_device; ++i)
      //       	{
      //       	  cout << " attr_device name[" << i << "] = " << d_names[i]
      //       	       << ", val = " << d_values[i] << std::endl;
      //      }
    }

  int fe_ch   = 0;
  int fe_data = 0;

for (int i=0; i<n_attr_fe; ++i)
    if  (names[i]=="ch") fe_ch = a2i(values[i]);
    else if (false
	     || names[i]=="Data"
	     || names[i]=="dataref"
	     || names[i]=="data_ref"
	     || names[i]=="data_type"
	     || names[i]=="dataname"
	     || names[i]=="data_name"
	     )
      {
	fe_data = u->get_data_id(values[i]);
      }
    else if (false
	     || names[i]=="data"
	     || names[i]=="data_id"
	     )
      {
	fe_data = a2i(values[i]);
      }

  std::bitset<k_n_ref> use_ref;
  std::string device;
  std::string plane;
  std::string segment;
  std::string ch;
  std::string data;

  int idevice  = 0;
  int iplane   = 0;
  int isegment = 0;
  int ich      = 0;
  int idata    = 0;

  for (int i=0; i<n_attr_device; ++i)
    {
      if      (false
	       || d_names[i]=="Device"
	       || d_names[i]=="ref"
	       || d_names[i]=="deviceref"
	       || d_names[i]=="device_ref"
	       || d_names[i]=="name"
	       || d_names[i]=="devicename"
	       || d_names[i]=="device_name")
	{
	  use_ref[k_device] = true;
	  device = d_values[i];
	}
      else if (false
	       || d_names[i]=="device"
	       || d_names[i]=="id"
	       || d_names[i]=="deviceid"
	       || d_names[i]=="device_id")
	{
	  idevice = a2i(d_values[i]);
	}
      else if (false
	       || d_names[i]=="Plane"
	       || d_names[i]=="planeref"
	       || d_names[i]=="plane_ref"
	       || d_names[i]=="planename"
	       || d_names[i]=="plane_name")
        {
	  use_ref[k_plane] = true;
	  plane = d_values[i];
        }
      else if (false
	       || d_names[i]=="plane"
	       || d_names[i]=="planeid"
	       || d_names[i]=="plane_id"
	       || d_names[i]=="planeid")
        {
	  iplane = a2i(d_values[i]);
        }
      else if (d_names[i]=="Segment"
	       || d_names[i]=="segmentref"
	       || d_names[i]=="segment_ref"
	       || d_names[i]=="segmentname"
	       || d_names[i]=="segment_name")
        {
	  use_ref[k_segment] = true;
	  segment = d_values[i];
        }
      else if (false
	       || d_names[i]=="segment"
	       || d_names[i]=="segmentid"
	       || d_names[i]=="segment_id")
        {
	  isegment = a2i(d_values[i]);
        }
      else if (false
	       || d_names[i]=="Ch"
	       || d_names[i]=="chref"
	       || d_names[i]=="ch_ref"
	       || d_names[i]=="chname"
	       || d_names[i]=="ch_name")
        {
	  use_ref[k_ch] = true;
	  ch = d_values[i];
        }
      else if (false
	       || d_names[i]=="ch"
	       || d_names[i]=="chid"
	       || d_names[i]=="ch_id")
        {
	  ich = a2i(d_values[i]);
        }
      else if (false
	       || d_names[i]=="Data"
	       || d_names[i]=="dataref"
	       || d_names[i]=="data_ref"
	       || d_names[i]=="dataname"
	       || d_names[i]=="data_name"
	       || d_names[i]=="data_type")
        {
	  use_ref[k_data] = true;
	  data = d_values[i];
        }
      else if (false
	       || d_names[i]=="data"
	       || d_names[i]=="dataid"
	       || d_names[i]=="data_id")
        {
	  idata = a2i(d_values[i]);
        }
    }

  try
    {
      if (use_ref[k_device]){
	idevice  = digit_info.get_device_id(device);
      }

      if (use_ref[k_plane]){
	iplane = digit_info.get_plane_id(idevice, plane);
      }

      if (use_ref[k_segment]){
	isegment = digit_info.get_segment_id(idevice, iplane, segment);
      }

      if (use_ref[k_ch]){
	ich = digit_info.get_ch_id(idevice, iplane, isegment, ch);
      }
      if (use_ref[k_data]){
	idata = digit_info.get_data_id(idevice, iplane, isegment, ich, data);
      }
    }
  catch (const std::exception& e)
    {
      cerr << "\n#E UnpackerXMLChannelMap::generate_content()\n "
	   << " while resolving parameter names ( string -> number )\n"
	   << " what = " << e.what() << "\n " << std::endl;
      print(idevice, iplane, isegment, ich, idata, fe_ch, fe_data,
	    u->get_name(), device, plane, segment, ch, data);
      std::exit(1);
    }

  // ###########################################################################
#ifdef CHANNEL_MAP
  print(idevice, iplane, isegment, ich, idata, fe_ch, fe_data,
	u->get_name(), device, plane, segment, ch, data);
#endif
  // ###########################################################################


  try
    {
      u->add_channel_map(fe_ch, fe_data,
			 m_digit_list,
			 idevice,
			 iplane,
			 isegment,
			 ich,
			 idata);
    }
  catch (const std::out_of_range& e)
    {
      cerr << "\n#E UnpackerXMLChannleMap::generate_content()\n what = "
	   << e.what() << "\n"
	   << " failed to call Unpacker::add_channel_map() of "
	   << u->get_type() << " (name = "
	   << u->get_name() << ")\n"
	   << " FE ch_id = " << fe_ch << ", data_id = " << fe_data
	   << std::endl;
      cerr << " D device_id = " << idevice
	   << " / " << m_digit_list.size()
	   << std::endl;
      cerr << " plane_id = " << iplane
	   << " / " << m_digit_list.at(idevice).size()
	   << std::endl;
      cerr << " segment_id = " << isegment
	   << " / " << m_digit_list.at(idevice).at(iplane).size()
	   << std::endl;
      cerr << " ch_id = " << ich
	   << " / " << m_digit_list.at(idevice).at(iplane).at(isegment).size()
	   << std::endl;
      cerr << " data_id = " << idata
	   << " / "
	   << m_digit_list.at(idevice).at(iplane).at(isegment).at(ich).size()
	   << std::endl;
      cerr << "\n Dref " << device
	   << " " << plane
	   << " " << segment
	   << " " << ch
	   << " " << data
	   << "\n some indeces exceed the range of digit list\n"
	   << "     check <front_end> or <digit_data> in xml files\n"
	   << std::endl;
      std::exit(1);
    }
  catch (const std::exception& e)
    {
      cerr << "\n#E UnpackerXMLChannleMap::generate_content()\n what = "
	   << e.what() << "\n"
	   << " failed to call Unpacker::add_channel_map() of "
	   << u->get_type() << std::endl;
      print(idevice, iplane, isegment, ich, idata, fe_ch, fe_data,
	    u->get_name(), device, plane, segment, ch, data);
      std::exit(1);
    }

  return;
}

//______________________________________________________________________________
void
UnpackerXMLChannelMap::read(DOMElement* e)
{
  DOMElementList children;
  xml::get_children(e, children);

  for (DOMElementList::iterator i=children.begin(); i!=children.end(); ++i)
    {
      DOMElement* child = *i;
      const std::string& tag = xml::get_tag_name(child);
      //cout << "#D UnpackerXMLChannelMap::read() tag = "
      // 	     << tag << std::endl;
      if (tag=="for") read_loop(child);
      else if (tag=="FE") generate_content(child);
      else read_front_end(child);
    }
  return;
}

//______________________________________________________________________________
void
UnpackerXMLChannelMap::read_front_end(DOMElement* e)
{
  const std::string& tag = xml::get_tag_name(e);
  UnpackerManager& g_unpacker = GUnpacker::get_instance();
  Unpacker* parent = 0;
  if (!m_unpacker_list.empty()) parent = m_unpacker_list.back();
//   if (parent){
//     cout << "#D parant = " << parent->get_name() << std::endl;
//}

  std::vector<std::string> names;
  std::vector<std::string> values;
  xml::get_attributes(e, names, values);
  const int n = names.size();

  //cout << "tag : " << tag << "\n";

  Unpacker* u = g_unpacker.create(tag);

  if (0<m_loop_depth)
    {
      for (int i=0; i<n; ++i) replace_loop_variables(values[i]);
      for (int i=0; i<n; ++i) xml::evaluate_attribute(values[i]);
    }

  for (int i=0; i<n; ++i)
    {
      const std::string& name  = names[i];
      const std::string& value = values[i];
      //cout << " name = " << name
      //<< ", value = " << value  << std::endl;
      if (false
	  || name=="node_id" || name=="NODE_ID"
	  || name=="ID" || name=="id"
//		  || name=="IP" || name=="ip"
	  // for COPPER
	  || name=="slot" || name=="SLOT"
	  // for VME
	  || name=="address"
	  // for TKO
	  || name=="MA" || name=="ma"
	  // for CAMAC
	  || name=="CRATE" || name=="crate" || name=="C" || name=="c"
	  || name=="STATION"||name=="station"||name=="N"||name=="n")
	{
	  uint64_t id = a2ull(value);
//  		  cout << " set id = "
// 		       << std::showbase << std::hex << id
// 		       << std::noshowbase << std::dec << std::endl;
	  u->set_id(id);
	  u->set_parent(parent);
	}
      else if (name=="name")
	{
	  u->set_name(value);
	}
      else if (name=="dump")
	{
	  if (false
	      || (value.find("y")==0)
	      || (value.find("Y")==0)
	      || (value.find("h")==0)
	      || (value.find("H")==0)
	      || (value.find("x")!=std::string::npos)
	      || (value.find("X")!=std::string::npos))
	    u->set_dump_mode(defines::k_hex);
	  if (false
	      || (value.find("d")==0)
	      || (value.find("D")==0))
	    {
	      u->set_dump_mode(defines::k_dec);
	    }
	  if (false
	      || (value.find("b")==0)
	      || (value.find("B")==0))
	    {
	      u->set_dump_mode(defines::k_binary);
	    }
	}
      else if (name=="decode"
	       && ((value.find("n")==0) || (value.find("N")==0)))
	{
	  u->set_decode_mode(false);
	}
      else if (name=="unpack"
	       && ((value.find("n")==0) || (value.find("N")==0)))
	{
	  u->set_unpack_mode(false);
	}
    }

  m_felist_id2name[u->get_id()] = u->get_name();
  if (u->get_name().empty())
    {
      u->set_name();
    }
  else
    {
      m_felist_name2id[u->get_name()] = u->get_id();
    }

  u->resize_fe_data();
  m_unpacker_list.push_back(u);
//   std::cout << "#D create unpacker  " << u->get_name() << std::endl;
  read(e);
  m_unpacker_list.pop_back();
  return;
}

  }
}
