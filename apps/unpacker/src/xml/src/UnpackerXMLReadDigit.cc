// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerXMLReadDigit.hh"

#include <algorithm>
#include <map>
#include <cstdlib>

#include "std_ostream.hh"
#include "replace_string.hh"
#include "lexical_cast.hh"
#include "func_name.hh"

namespace hddaq
{
namespace unpacker
{

namespace
{
const std::string ks_class_name("UnpackerXMLReadDigit");

}

//_____________________________________________________________________________
UnpackerXMLReadDigit::UnpackerXMLReadDigit(DOMElement* e,
					   DigitList& digit_list)
  : xml::XMLRead(e),
    m_tag(),
    m_digit_list(digit_list),
    m_device_ref(),
    m_plane_ref(),
    m_segment_ref(),
    m_ch_ref(),
    m_data_ref(),
    m_device_names(),
    m_plane_names(),
    m_segment_names(),
    m_ch_names(),
    m_data_names(),
    m_device_id(0),
    m_plane_id(0),
    m_segment_id(0),
    m_ch_id(0),
    m_null_device_id(-1)
{
  m_tag = xml::get_tag_name(e);
  cout << "#D " << ks_class_name << "::" << ks_class_name << "()\n   "
       << " reading <" << m_tag << "> " << std::endl;
  try
  {
    read(e);
  }
  catch (const std::bad_alloc& e)
  {
    cerr << "\n#E " << ks_class_name << "::" << ks_class_name << "()\n   "
         << " what = " << e.what()
         << " while reading <" << m_tag << "> : \n"
         << std::endl;
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::" << ks_class_name << "()\n   "
         << " what = " << e.what()
         << " while reading <" << m_tag << "> : \n"
         << std::endl;
  }


  // add default container when plane, segment, ch, or data are omitted
  for (DataRef::iterator i = m_data_ref.begin(); i != m_data_ref.end(); ++i)
  {
    if (i->empty())
      i->resize(1);
    for (DataRef::value_type::iterator j = i->begin(); j != i->end(); ++j)
    {
      if (j->empty())
        j->resize(1);
      for (DataRef::value_type::value_type::iterator k = j->begin(); k != j->end(); ++k)
      {
        if (k->empty())
          k->resize(1);
        for (DataRef::value_type::value_type::value_type::iterator l = k->begin(); l != k->end(); ++l)
        {
          if (l->empty())
            l->insert(std::make_pair<std::string, int>("0", 0));
        }
      }
    }
  }



  cout << "#D resize DigitList\n";

  const int n_device = get_n_device();
  m_digit_list.resize(n_device);

  for (int device_id=0; device_id<n_device; ++device_id)
  {
    if (device_id%1000==0)
    {
      cout << ".";
      cout.flush();
    }

//       cout << "#D device id = " << device_id << " / " << n_device;
    Device& device = m_digit_list[device_id];
    const int n_plane = get_n_plane(device_id);
    device.resize(n_plane);

    for (int plane_id=0; plane_id<n_plane; ++plane_id)
    {
// 	  cout << "\n  plane id = " << plane_id << " / " << n_plane;
      Plane& plane = device[plane_id];
      const int n_segment = get_n_segment(device_id, plane_id);
      plane.resize(n_segment);


      for (int segment_id=0; segment_id<n_segment; ++segment_id)
      {
// 	      cout << "\n  segment id = " << segment_id << " / " << n_segment;
        Segment& segment = plane[segment_id];
        const int n_ch = get_n_ch(device_id, plane_id, segment_id);
        segment.resize(n_ch);



        for (int ch_id=0; ch_id<n_ch; ++ch_id)
        {
// 	      cout << "\n  ch id = " << ch_id << " / " << n_ch;
          Ch& ch = segment[ch_id];

          std::map<std::string, int>& data_ref
            = m_data_ref[device_id][plane_id][segment_id][ch_id];
          if (data_ref.empty())
            data_ref.insert(std::make_pair("0",0));

          const int n_data
            = get_n_data(device_id, plane_id, segment_id, ch_id);
// 		  cout << "\n n data = " << n_data << std::endl;
          ch.resize(n_data);
        }
      }
    }
  }

  cout << " ->  finished" << std::endl;

  cout << "#D build digit name map" << std::endl;
  m_device_names .resize(n_device);
  m_plane_names  .resize(n_device);
  m_segment_names.resize(n_device);
  m_ch_names     .resize(n_device);
  m_data_names   .resize(n_device);

  for (Ref::iterator itr_dev = m_device_ref.begin();
       itr_dev!=m_device_ref.end(); ++itr_dev)
  {
    const std::string& dev_name = itr_dev->first;
    //      const std::string& dev_id   = i2a(itr_dev->second);
//       if (dev_id==dev_name)
// 	continue;
//       std::cout << dev_name << " " << dev_id << std::endl;

    const unsigned int idev = itr_dev->second;
    if(m_device_names[idev] != ""){
      cout << "#E " << ks_class_name << "::" << ks_class_name << "()\n";
      cout << " Multiple declaration of Device ID\n";
      cout << " ID : " << idev << ", Name :" << dev_name << std::endl;
      std::exit(-1);
    }
    m_device_names[idev] = dev_name;

    const int n_plane = get_n_plane(idev);
    m_plane_names  [idev].resize(n_plane);
    m_segment_names[idev].resize(n_plane);
    m_ch_names     [idev].resize(n_plane);
    m_data_names   [idev].resize(n_plane);
  }
//   std::cout << "#D   device name map finishded" << std::endl;

  const int n_dev = get_n_device();
  for (int idev=0; idev<n_dev; ++idev)
  {
    if (idev==m_null_device_id)
      continue;
    if (m_device_names[idev].empty())
      continue;
    //      const int n_plane = get_n_plane(idev);
    for (Ref::iterator itr_plane = m_plane_ref[idev].begin();
         itr_plane!=m_plane_ref[idev].end(); ++itr_plane)
    {
      const std::string& plane_name = itr_plane->first;
// 	  const std::string& plane_id   = i2a(itr_plane->second);
// 	  if (plane_id==plane_name)
// 	    continue;
      const unsigned int iplane = itr_plane->second;
//  	  std::cout << plane_name << " " << iplane << std::endl;
      m_plane_names[idev][iplane] = plane_name;
    }
  }
//   std::cout << "#D   plane name map finishded" << std::endl;

  for (int idev=0; idev<n_dev; ++idev)
  {
    if (idev==m_null_device_id)
      continue;
    if (m_device_names[idev].empty())
      continue;
    const int n_plane = get_n_plane(idev);
    for (int iplane=0; iplane<n_plane; ++iplane)
    {
      const int n_segment = get_n_segment(idev, iplane);
// 	  std::cout << " n seg = " << n_segment << std::endl;
      m_segment_names[idev];
      m_segment_names[idev][iplane];
      m_segment_names[idev][iplane].resize(n_segment);
      m_ch_names     [idev][iplane].resize(n_segment);
      m_data_names   [idev][iplane].resize(n_segment);
      Ref& ref = m_segment_ref[idev][iplane];
      if (ref.empty())
        continue;

      for (Ref::iterator itr = ref.begin(); itr!=ref.end(); ++itr)
      {
        const std::string& seg_name = itr->first;
// 	      const std::string& seg_id   = i2a(itr->second);
// 	      if (seg_id==seg_name)
// 		continue;
// 	      std::cout << seg_name << std::endl;
        const unsigned int iseg = itr->second;
        m_segment_names[idev][iplane][iseg] = seg_name;

      }
    }
  }
//   std::cout << "#D   segment name map finished" << std::endl;

  for (int idev=0; idev<n_dev; ++idev)
  {
    if (idev==m_null_device_id)
      continue;
    if (m_device_names[idev].empty())
      continue;
    const int n_plane = get_n_plane(idev);
//       std::cout << "#D device = " << m_device_names[idev] << std::endl;
    for (int iplane=0; iplane<n_plane; ++iplane)
    {
// 	  std::cout << "#D plane = " << m_plane_names[idev][iplane] << std::endl;
      const int n_segment = get_n_segment(idev, iplane);
      for (int iseg=0; iseg<n_segment; ++iseg)
      {
// 	      std::cout << "#D segment = " << m_segment_names[idev][iplane][iseg] << std::endl;
        const int n_ch = get_n_ch(idev, iplane, iseg);
// 	      std::cout << " n ch = " << n_ch << std::endl;
        m_ch_names  [idev][iplane][iseg].resize(n_ch);
        m_data_names[idev][iplane][iseg].resize(n_ch);
        Ref& ref=m_ch_ref[idev][iplane][iseg];
        for (Ref::iterator itr = ref.begin(); itr!=ref.end(); ++itr)
        {
          const std::string& ch_name = itr->first;
          if (ch_name.empty())
            continue;
          //		  const std::string& ch_id   = i2a(itr->second);

// 		  if (ch_id==ch_name)
// 		    continue;
//  		  std::cout << ch_name << " " << ch_id << std::endl;
          const unsigned int ich = itr->second;
          m_ch_names[idev][iplane][iseg][ich] = ch_name;
        }
      }
    }
  }
//   std::cout << "#D   ch name map finished" << std::endl;

  for (int idev=0; idev<n_dev; ++idev)
  {
    if (idev==m_null_device_id)
      continue;
    if (m_device_names[idev].empty())
      continue;
    const int n_plane = get_n_plane(idev);
    for (int iplane=0; iplane<n_plane; ++iplane)
    {
      const int n_segment = get_n_segment(idev, iplane);
      for (int iseg=0; iseg<n_segment; ++iseg)
      {
        const int n_ch = get_n_ch(idev, iplane, iseg);
        for (int ich=0; ich<n_ch; ++ich)
        {
          const int n_data = get_n_data(idev, iplane, iseg, ich);
          m_data_names[idev][iplane][iseg][ich].resize(n_data);

          for (Ref::iterator itr_data
                 = m_data_ref[idev][iplane][iseg][ich].begin();
               itr_data!=m_data_ref[idev][iplane][iseg][ich].end();
               ++itr_data)
          {
// 		      std::cout << m_device_names[idev] << " "
// 				<< m_plane_names[idev][iplane] << " "
// 				<< m_segment_names[idev][iplane][iseg] << " "
// 				<< m_ch_names[idev][iplane][iseg][ich] << std::endl;
            const std::string& data_name = itr_data->first;
            //		      const std::string& data_id   = i2a(itr_data->second);

            // 		      if (data_id==data_name)
            // 			continue;

            const unsigned int idata = itr_data->second;
            m_data_names[idev][iplane][iseg][ich][idata] = data_name;
          }
        }

      }

    }

  }
//   std::cout << "#D   data name map finished" << std::endl;


  cout << " ->  finished" << std::endl;
}

//_____________________________________________________________________________
UnpackerXMLReadDigit::~UnpackerXMLReadDigit()
{
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::hoge(const std::string& arg) const
{
  cout << "#D UnpackerXMLReadDigit::hoge(" << arg << ")" << std::endl;
  return;
}

//_____________________________________________________________________________
int
UnpackerXMLReadDigit::get_ch_id(int device_id,
				int plane_id,
				int segment_id,
				const std::string& name) const
{
  try
  {
    const std::map<std::string, int>& ref
      = m_ch_ref.at(device_id).at(plane_id).at(segment_id);
    const std::map<std::string, int>::const_iterator i = ref.find(name);
    if (i==ref.end())
    {
      cerr << "\n#E UnpackerXMLReadDigit::get_ch_id()\n"
           << "   cannot find [" << device_id
           << "][" << plane_id << "]["
           << segment_id << "]" << std::endl;
      std::exit(1);
    }
    return i->second;
  }
  catch (const std::exception& e)
  {
    cerr << "\nE UnpackerXMLReadDigit::get_ch_id()"
         << " device id = " <<  device_id
         << " plane id = " << plane_id
         << " segment id = " << segment_id
         << std::endl;
    throw;
  }
  return -1;
}

//_____________________________________________________________________________
int
UnpackerXMLReadDigit::get_data_id(int device_id,
				  int plane_id,
				  int segment_id,
				  int ch_id,
				  const std::string& name) const
{
  try
  {
    const std::map<std::string, int>& ref
      = m_data_ref.at(device_id).at(plane_id).at(segment_id).at(ch_id);
    std::map<std::string, int>::const_iterator i = ref.find(name);
    if (i==ref.end())
    {
      cerr << "\n#E UnpackerXMLReadDigit::get_data_id()\n"
           << "   cannot find [" << device_id
           << "][" << plane_id
           << "][" << segment_id
           << "][" << name << "]"
           << " in <" << m_tag << "> XML DOM tree" << std::endl;
      std::exit(1);
    }
    return i->second;
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_data_id()\n"
         << " device id = " << device_id << " / "
         << m_data_ref.size() <<"\n"
         << " plane id  = " << plane_id << " / "
         << m_data_ref.at(device_id).size() << "\n"
         << " segment id = " << segment_id << " / "
         << m_data_ref.at(device_id).at(plane_id).size() << "\n"
         << " ch id  = " << ch_id << " / "
         << m_data_ref.at(device_id).at(plane_id).at(segment_id).size()
         << "\n"
         << " " << e.what() << std::endl;

    throw;
  }
  return -1;
}

//_____________________________________________________________________________
int
UnpackerXMLReadDigit::get_device_id(const std::string& name) const
{
  std::map<std::string, int>::const_iterator i = m_device_ref.find(name);
  if (i==m_device_ref.end())
  {
    cerr << "\n#E UnpackerXMLReadDigit::get_device_id()\n"
         << " cannot find " << name << " in <"
         << m_tag << "> XML DOM tree"
         << std::endl;
    std::exit(1);
  }

  return i->second;
}

//_____________________________________________________________________________
const std::string&
UnpackerXMLReadDigit::get_device_type(int device_id) const
{
  return m_device_types.at(device_id);
}

//_____________________________________________________________________________
const std::string&
UnpackerXMLReadDigit::get_device_type(const std::string& name) const
{
  return get_device_type(get_device_id(name));
}

//_____________________________________________________________________________
const UnpackerXMLReadDigit::NameList&
UnpackerXMLReadDigit::get_name_list() const
{
  return m_device_names;
}

//_____________________________________________________________________________
const UnpackerXMLReadDigit::NameList&
UnpackerXMLReadDigit::get_name_list(int device_id) const
{
  try
  {
    return m_plane_names.at(device_id);
  }
  catch (std::exception& e)
  {
    std::cout << "\n#E " << ks_class_name
              << "::get_name_list( plane name list )\n"
              << " what = " << e.what() << "\n"
              << " device_id = " << device_id
              << std::endl;
    throw;
  }
}

//_____________________________________________________________________________
const UnpackerXMLReadDigit::NameList&
UnpackerXMLReadDigit::get_name_list(int device_id,
				    int plane_id) const
{
  try
  {
    return m_segment_names.at(device_id).at(plane_id);
  }
  catch (std::exception& e)
  {
    std::cout << "\n#E " << ks_class_name
              << "::get_name_list( segment name list)\n"
              << " what = " << e.what() << "\n"
              << " device_id = " << device_id
              << ", plane_id = " << plane_id
              << std::endl;
    throw;
  }
}

//_____________________________________________________________________________
const UnpackerXMLReadDigit::NameList&
UnpackerXMLReadDigit::get_name_list(int device_id,
				    int plane_id,
				    int segment_id) const
{
  try
  {
    return m_ch_names.at(device_id).at(plane_id).at(segment_id);
  }
  catch (std::exception& e)
  {
    std::cout << "\n#E " << ks_class_name
              << "::get_name_list( ch name list)\n"
              << " what = " << e.what() << "\n"
              << " device_id = "   << device_id
              << ", plane_id = "   << plane_id
              << ", segment_id = " << segment_id
              << std::endl;
    throw;
  }
}

//_____________________________________________________________________________
const UnpackerXMLReadDigit::NameList&
UnpackerXMLReadDigit::get_name_list(int device_id,
				    int plane_id,
				    int segment_id,
				    int ch_id) const
{
  try
  {
    return m_data_names.at(device_id).at(plane_id).at(segment_id).at(ch_id);
  }
  catch (std::exception& e)
  {
    std::cout << "\n#E " << ks_class_name
              << "::get_name_list( data name list)\n"
              << " what = " << e.what() << "\n"
              << " device_id = "   << device_id
              << ", plane_id = "   << plane_id
              << ", segment_id = " << segment_id
              << ", ch_id = "      << ch_id
              << std::endl;
    throw;
  }
}

//_____________________________________________________________________________
unsigned int
UnpackerXMLReadDigit::get_n_ch(int device_id,
			       int plane_id,
			       int segment_id) const
{
  try
  {
//       return m_ch_ref.at(device_id).at(plane_id).at(segment_id).size();
    return m_data_ref.at(device_id).at(plane_id).at(segment_id).size();
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_n_ch()\n"
         << " what = " << e.what() << "\n"
         << " device_id = " << device_id
         << ", plane_id = " << plane_id
         << ", segment_id = " << segment_id
         << std::endl;
    throw;
  }
//   return -1;

}

//_____________________________________________________________________________
unsigned int
UnpackerXMLReadDigit::get_n_data(int device_id,
				 int plane_id,
				 int segment_id,
				 int ch_id) const
{
  try
  {
    return m_data_ref
      .at(device_id)
      .at(plane_id)
      .at(segment_id)
      .at(ch_id)
      .size();
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_n_data()\n"
         << " what = " << e.what() << "\n"
         << " device_id = " << device_id
         << ", plane_id = " << plane_id
         << ", segment_id = " << segment_id
         << ", ch_id = " << ch_id
         << std::endl;
    throw;
  }
//   return -1;
}

//_____________________________________________________________________________
unsigned int
UnpackerXMLReadDigit::get_n_device() const
{
//   return m_device_ref.size();
  return m_data_ref.size();
}

//_____________________________________________________________________________
unsigned int
UnpackerXMLReadDigit::get_n_plane(int device_id) const
{
  try
  {
//       return m_plane_ref.at(device_id).size();
    return m_data_ref.at(device_id).size();
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_n_plane()\n"
         << " what = " << e.what() << "\n"
         << " device_id = " << device_id
         << std::endl;
    throw;
  }
//   return -1;
}


//_____________________________________________________________________________
unsigned int
UnpackerXMLReadDigit::get_n_segment(int device_id,
				    int plane_id) const
{
  try
  {
//       return m_segment_ref.at(device_id).at(plane_id).size();
    return m_data_ref.at(device_id).at(plane_id).size();
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_n_plane()\n"
         << " what = " << e.what() << "\n"
         << " device_id = " << device_id
         << ", plane_id = " << plane_id
         << std::endl;
    throw;
  }
//   return -1;
}

//_____________________________________________________________________________
int
UnpackerXMLReadDigit::get_null_device_id() const
{
  return m_null_device_id;
}

//_____________________________________________________________________________
int
UnpackerXMLReadDigit::get_plane_id(int device_id,
				   const std::string& name) const
{
  try
  {
    const std::map<std::string, int>& ref =  m_plane_ref.at(device_id);
    std::map<std::string, int>::const_iterator i = ref.find(name);
    if (i==ref.end())
    {
      cerr << "\n#E UnpackerXMLReadDigit::get_plane_id()\n"
           << "   cannot find [" << device_id << "][" << name << "]"
           << std::endl;
      std::exit(1);
      return -1;
    }
    return i->second;
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_plane_id()\n"
         << " device_id = " << device_id << "\n"
         << " plane name = " << name << "\n"
         << " " << e.what() << std::endl;
    throw;
  }
  return -1;
}

//_____________________________________________________________________________
int
UnpackerXMLReadDigit::get_segment_id(int device_id,
				     int plane_id,
				     const std::string& name) const
{
  try
  {
    const std::map<std::string, int>& ref
      =  m_segment_ref.at(device_id).at(plane_id);
    std::map<std::string, int>::const_iterator i = ref.find(name);
    if (i==ref.end())
    {
      cerr << "\n#E UnpackerXMLReadDigit::get_segment_id()\n"
           << "   cannot find [" << device_id << "]["
           << plane_id << "]["
           << name << "]"
           << std::endl;
      std::exit(1);
      return -1;
    }
    return i->second;
  }
  catch (const std::exception& e)
  {
    cerr << "\n#E " << ks_class_name << "::get_segment_id()\n"
         << " device_id = " << device_id << "\n"
         << " plane_id = " << plane_id << "\n"
         << " segment name = " << name
         << " " << e.what() << std::endl;
    throw;
  }
  return -1;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::print(const std::string& arg) const
{
  static const std::string ks_func_name
    = make_func_name(__func__, ks_class_name);
  cout << "#D " << ks_class_name << "::print( " << arg << " )\n"
       << " m_device_id = " << m_device_id << "\n"
       << " m_plane_id  = " << m_plane_id << "\n"
       << " m_segment_id = " << m_segment_id << "\n"
       << " m_ch_id = " << m_ch_id << "\n"
       << std::endl;
  return;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::read(DOMElement* e)
{
  static const std::string ks_func_name
    = make_func_name(__func__, ks_class_name);
  DOMElementList children;
  xml::get_children(e, children);
//   print(ks_func_name);
  for (DOMElementList::iterator i=children.begin();i!=children.end();++i)
  {
    DOMElement* child = *i;

    const std::string& tag = xml::get_tag_name(child);
#ifdef DIGIT_READ
    cout << "#D " << ks_func_name << tag << std::endl;
#endif

    if (tag=="for")
      read_loop(child);
    else if (tag=="device")
      read_device_digit(child);
    else if (tag=="plane")
      read_plane_digit(child);
    else if (tag=="segment")
      read_segment_digit(child);
    else if (tag=="ch")
      read_ch_digit(child);
    else if (tag=="data")
      read_data_digit(child);
    else
    {
      cerr << "#E UnpackerXMLReadDigit::read()\n"
           << " unknown element = " << tag << " found" << std::endl;
    }
  }
  return;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::read_ch_digit(DOMElement* e)
{
  static const std::string ks_func_name
    = make_func_name(__func__, ks_class_name);
  m_ch_id = 0;
#ifdef DIGIG_CH
  cout << "#D " << ks_func_name << std::endl;
#endif


  DOMAttrList attr;
  xml::get_attributes(e, attr);
  std::string& id = attr["id"];

  replace_loop_variables(id);
  m_ch_id = a2i(id);


  std::string& ref = attr["name"];
  if (ref.empty())
    ref = id;

#ifdef DIGIT_CH
  print(ks_func_name);
  cout << "ch_id = " << id
       << ", m_ch_id = " << m_ch_id
       << ", ref = " << ref
       << std::endl;
#endif

//   PlaneRef::reference   pref = m_plane_ref[m_device_id];
//   SegmentRef::reference sref = m_segment_ref[m_device_id];
  ChRef::reference      cref = m_ch_ref[m_device_id];
  DataRef::reference    dref = m_data_ref[m_device_id];
//   // case : plane == specified, segment == not specified
//   if (m_segment_id==0 && cref[m_plane_id].empty())
//     {
//       cref[m_plane_id].resize(1);
//       dref[m_plane_id].resize(1);
//       if (sref[m_plane_id].empty())
// 	    sref[m_plane_id]["0"] = 0;
// //       cout << "#W " << ks_class_name << "::read_ch_digit()\n"
// // 	   << " automatically add segment = " << m_segment_id
// // 	   << " to device = " << m_device_id
// // 	   << ", plane = " << m_plane_id << std::endl;
//     }
//   hoge();

  // case : plane == not specified, segment == not specified
//   if (m_plane_id==0 && cref.empty())
//     {
//       cref.resize(1);
//       dref.resize(1);
//       if (pref.empty())
// 	   pref["0"] = 0;
//       if (m_segment_ref[m_device_id].empty())
// 	{
// 	  sref.resize(1);
// 	  sref[m_plane_id]["0"]=0;
// // 	  cout << "#W " << ks_class_name << "::read_ch_digit()\n"
// // 	       << " automatically add plane = " << m_plane_id
// // 	       << ", segment = " << 0
// // 	       << " to device = " << m_device_id << std::endl;

// 	}
//     }


  if (cref.size()<=m_plane_id)
  {
    const int n = m_plane_id + 1;
    cref.resize(n);

  }

  ChRef::value_type::reference ccref = cref[m_plane_id];
  if (ccref.size()<=m_segment_id)
  {
    const int n = m_segment_id + 1;
    ccref.resize(n);

  }


  ChRef::value_type::value_type::reference cccref = ccref[m_segment_id];


  cccref.insert(std::make_pair(ref, m_ch_id));

  if (dref.size()<=m_plane_id)
  {
    const int n = m_plane_id + 1;
    dref.resize(n);

  }
  DataRef::value_type::reference ddref = dref[m_plane_id];

  if (ddref.size()<=m_segment_id)
  {
    const int n = m_segment_id + 1;
    ddref.resize(n);

  }

  unsigned int n_ch = get_n_ch(m_device_id, m_plane_id, m_segment_id);
//   hoge();
  if (m_ch_id>=n_ch)
  {
    int n = m_ch_id + 1;
    ddref[m_segment_id].resize(n);

  }

#ifdef DIGIT_CH
  cout << " ch_id = " << m_ch_id
       << " , ref = " << ref << std::endl;

#endif

  read(e);

  return;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::read_data_digit(DOMElement* e)
{
  static const std::string ks_func_name
    = make_func_name(__func__, ks_class_name);
//  cout << "#D hoge" <<std::endl;
  DOMAttrList attr;
  xml::get_attributes(e, attr);
  std::string id = attr["id"];
  replace_loop_variables(id);
  int data_id = a2i(id);
  std::string& ref = attr["name"];

#ifdef DIGIT_DATA
  print(ks_func_name);
  cout << "#D0  data_id = " << id
       << " , ref = " << ref << std::endl;
#endif
  if (ref.empty())
    ref = id;

  if (m_data_ref.size()<=m_device_id)
    m_data_ref.resize(m_device_id+1);

  DataRef::reference dref = m_data_ref[m_device_id];
  if (dref.size()<=m_plane_id)
    dref.resize(m_plane_id+1);

  DataRef::value_type::reference ddref = dref[m_plane_id];
  if (ddref.size()<=m_segment_id)
    ddref.resize(m_segment_id+1);

  DataRef::value_type::value_type::reference dddref = ddref[m_segment_id];
  if (dddref.size()<=m_ch_id)
    dddref.resize(m_ch_id+1);

  Ref& ddddref = dddref[m_ch_id];

  ddddref.insert(std::make_pair(ref, data_id));

#ifdef DIGIT_DATA
  cout << " data_id = " << id
       << " , ref = " << ref << std::endl;

  cout << "#D " << ks_func_name << "\n"
       << " device_id = " << m_device_id
       << ", plane_id = " << m_plane_id
       << ", segment_id = " << m_segment_id
       << ", ch_id = " << m_ch_id
       << "(" << m_ch_ref[m_device_id][m_plane_id][m_segment_id].size() << ")"
       << ", data_id = " << id
       << " , ref = " << ref << std::endl;
#endif
  return;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::read_device_digit(DOMElement* e)
{
  static const std::string ks_func_name
    = ks_class_name + "::raed_device_digit()";

  m_device_id  = 0;
  m_plane_id   = 0;
  m_segment_id = 0;
  m_ch_id      = 0;

  DOMAttrList attr;
  xml::get_attributes(e, attr);

  std::string& id = attr["id"];
  replace_loop_variables(id);
  m_device_id = a2i(id);
  std::string& ref = attr["name"];
  if (ref=="null" || ref=="NULL" || ref=="Null")
    m_null_device_id = a2i(id);

  if (ref.empty()){
    ref = id;
  }

  std::pair<Ref::iterator, bool> insert_result
    = m_device_ref.insert(std::make_pair(ref, m_device_id));
  if(!insert_result.second){
    cout << "#E " << ks_class_name << "::read_device_digit()\n ";
    cout << " Multiple declaration of Device Name\n";
    cout << " Name : " << id << std::endl;
    std::exit(-1);
  }

  const unsigned int n_device = get_n_device();
  if (m_device_id>=n_device)
  {
    const int n = m_device_id + 1;
    m_plane_ref.resize(n);
    m_segment_ref.resize(n);
    m_ch_ref.resize(n);
    m_data_ref.resize(n);

  }


  std::string& type = attr["type"];
  m_device_types[m_device_id] = type;

#ifdef DIGIT_DEVICE
  cout << "#D " << ks_class_name << "::read_device_digit()\n ";
#endif

  cout << " found device id = " << std::setw(8) << m_device_id

#ifdef DIGIT_DEVICE
       << " ( .size() = " << n_device << ")"
#endif
       << " ( .size() = " << std::setw(3) << n_device << ")"
       << ", ref = " << std::setw(12) << ref
       << ", type = " << type << std::endl;

  read(e);

  return;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::read_plane_digit(DOMElement* e)
{
  static const std::string ks_func_name
    = make_func_name(__func__, ks_class_name);
  m_plane_id   = 0;
  m_segment_id = 0;
  m_ch_id      = 0;

  DOMAttrList attr;
  xml::get_attributes(e, attr);
  std::string& ref = attr["name"];
  std::string& id  = attr["id"];
  replace_loop_variables(id);

  m_plane_id = a2i(id);

  if (ref.empty())
    ref = id;
  PlaneRef::reference   pref = m_plane_ref[m_device_id];
  SegmentRef::reference sref = m_segment_ref[m_device_id];
  ChRef::reference      cref = m_ch_ref[m_device_id];
  DataRef::reference    dref = m_data_ref[m_device_id];

  pref.insert(std::make_pair(ref, m_plane_id));

  const unsigned int n_plane = get_n_plane(m_device_id);
  if (m_plane_id>=n_plane)
  {
    const int n = m_plane_id + 1;
    sref.resize(n);
    cref.resize(n);
    dref.resize(n);

  }

#ifdef DIGIT_PLANE
  cout << "#D " << ks_func_name << "  ______________\n "
       << " plane id = " << m_plane_id
       << ", ref = " << ref << std::endl;
#endif
  read(e);

  return;
}

//_____________________________________________________________________________
void
UnpackerXMLReadDigit::read_segment_digit(DOMElement* e)
{
  static const std::string ks_func_name
    = make_func_name(__func__, ks_class_name);
  m_segment_id = 0;
  m_ch_id      = 0;

  DOMAttrList attr;
  xml::get_attributes(e, attr);
  std::string& ref = attr["name"];
  std::string& id  = attr["id"];
  replace_loop_variables(id);

  m_segment_id = a2i(id);

  if (ref.empty())
    ref = id;

  PlaneRef::reference   pref = m_plane_ref[m_device_id];
  SegmentRef::reference sref = m_segment_ref[m_device_id];
  ChRef::reference      cref = m_ch_ref[m_device_id];
  DataRef::reference    dref = m_data_ref[m_device_id];
  if (m_plane_id==0 && sref.empty())
  {
    if (m_plane_ref[m_device_id].empty())
      pref.insert(std::make_pair("0", 0));
//       cout << "#W " << ks_class_name
// 	   << "::read_segment_digit()\n"
// 	   << " automatically add plane = " << m_plane_id << " to "
// 	   << "device = " << m_device_id << std::endl;
  }

  if (sref.size()<=m_plane_id)
  {
    int n = m_plane_id+1;
    sref.resize(n);
    cref.resize(n);
    dref.resize(n);

  }

  SegmentRef::value_type::reference ssref = sref[m_plane_id];
  ssref.insert(std::make_pair(ref, m_segment_id));

  const unsigned int n_segment = get_n_segment(m_device_id, m_plane_id);
  if (m_segment_id>=n_segment)
  {
    const int n = m_segment_id + 1;
    cref[m_plane_id].resize(n);
    dref[m_plane_id].resize(n);

  }

#ifdef DIGIT_SEGMENT
  cout << "#D " << ks_func_name << " ______________\n "
       << " segment id = " << m_segment_id
       << ", ref = " << ref << std::endl;
#endif
  read(e);

  return;
}

}

}
