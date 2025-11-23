// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerImpl.hh"

#include <algorithm>
#include <iostream>
#include <iterator>
// #include <numeric>
#include <stdexcept>

#include "std_ostream.hh"
#include "Unpacker.hh"
#include "UnpackerManager.hh"
#include "lexical_cast.hh"
#include "BitDump.hh"
#include "HexDump.hh"

namespace hddaq
{
  namespace unpacker
  {

    bool UnpackerImpl::gm_is_esc_on = false;
    UnpackerImpl::check_mode_t UnpackerImpl::gm_check_mode;
    std::string UnpackerImpl::gm_true_bit;
    std::string UnpackerImpl::gm_false_bit;
    int UnpackerImpl::gm_null_device_id = -1;

  namespace
  {
    
    //__________________________________________________________________________
    bool
    check_diff(const std::vector<int>& tag)
    {
      //   cout << "#D check_mismatch(private)  " << m_name << std::endl;
      bool ret = true;
      const int n = tag.size();
      for (int i=0; i<n; ++i){
	ret &= (tag[0] == tag[i]);
	//	if(!ret)std::cout << tag[0] << " " << tag[i] << std::endl;
      }
      
      return ret;
    }
    
  }


//______________________________________________________________________________
UnpackerImpl::UnpackerImpl(const unpacker_type& type)
  : m_type(type),
    m_id(defines::k_unassignedULL),
    m_name(),
    m_parent(0),
    m_child_list(),
    m_data_size(0),
    m_word_count(0),
    m_data_first(),
    m_data_last(),
    m_first_list(),
    m_last_list(),
    m_digit_id_list(),
    m_fe_data(),
    m_back(0),
    m_null_ch(),
    m_tag(k_n_tag_type),
    m_error_state(),
    m_has_tag(),
    m_tag_status(),
    m_is_node(false),
    m_dump_mode(),
    m_is_decode_mode(true),
    m_is_unpack_mode(true),
    m_run_number(defines::k_unassigned),
    m_sec(),
    m_nsec()

{
  for (int i=0; i<k_n_tag_type; ++i)
//     fill_tag(m_tag[i], defines::k_unassigned);
//     m_tag[i].clear();
    m_tag[i].resize(1);

  
  Tag& origin = m_tag[k_tag_origin].back();
  origin.m_local = 0;
  origin.m_event = k_MTM_EVENT_TAG_ORIGIN;
  origin.m_spill = 0;

  Tag& max = m_tag[k_tag_max].back();
  max.m_local = 0x7fffffff;
  max.m_event = k_MTM_MAX_EVENT_TAG;
  max.m_spill = k_MTM_MAX_SPILL_TAG;
}

//______________________________________________________________________________
UnpackerImpl::~UnpackerImpl()
{
//   cout << "#D ::~UunpackerImpl()" << m_name 
//          << std::endl;
}

//______________________________________________________________________________
void 
UnpackerImpl::hoge(const std::string& arg) const
{
  cout << "#D UnpackerImpl::hoge()" << std::endl;

  cout << "#D "
	 << (m_type.empty() ? "UnpackerImpl" : m_type)
	 << "::hoge() " << m_name << " " << arg << std::endl;
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::check_data_format()
{
//   cout << "#D UnpackerImpl::check_data_format()" << std::endl;
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::check_mismatch()
{
//   cout << "#D check_mismatch()  " << m_name << std::endl;

  if (m_name.find("UNKNOWN")!=std::string::npos 
      || m_name.find("unknown")!=std::string::npos)
    {
      return;
    }

  if (m_child_list.empty())
    {
      return;
    }

  std::vector<int> child_local;
  std::vector<int> child_event;
  std::vector<int> child_spill;

  Tag& tag  = m_tag[k_tag_current].back();
  Tag& max  = m_tag[k_tag_max].back();
  const Tag& orig = m_tag[k_tag_origin].back();
  unsigned int max_local = max.m_local;

  std::vector<Unpacker*> child_with_event_tag;
  std::vector<Unpacker*> child_with_spill_tag;

  ChildList::const_iterator cend = m_child_list.end();
  for (ChildList::const_iterator i = m_child_list.begin(); i!=cend; ++i)
    {
      const Unpacker* const child = i->second;
      if (!child) continue;

      const Tag& cmax = child->get_tag(k_tag_max);
      if (child->has_tag(k_local)){
	// 	max.m_local = std::min(max.m_local, cmax.m_local);
	//	max_local = std::min(max.m_local, cmax.m_local);
	unsigned int tmp_max_local = std::min(max.m_local, cmax.m_local);
	if(max_local > tmp_max_local){
	  max_local = tmp_max_local;
	}
      }

      if (child->has_tag(k_event)){
	max.m_event = std::min(max.m_event, cmax.m_event);
      }

      if (child->has_tag(k_spill)){
	max.m_spill = std::min(max.m_spill, cmax.m_spill);
      }

    }


  for (ChildList::const_iterator i = m_child_list.begin(); i!=cend; ++i)
    {
      Unpacker* child = i->second;
      if (!child) continue;
      const Tag& ctag = child->get_tag();
      const Tag& cori = child->get_tag(k_tag_origin);
      if (child->has_tag(k_local)){
	child_local.push_back((ctag.m_local-cori.m_local)&max_local);
      }
      
      if (child->has_tag(k_event)){
	child_with_event_tag.push_back(child);
	child_event.push_back((ctag.m_event-cori.m_event)&max.m_event);
      }

      if (child->has_tag(k_spill)){
	child_with_spill_tag.push_back(child);
	child_spill.push_back((ctag.m_spill-cori.m_spill)&max.m_spill);
      }
      
      if (!child->is_good()){
	m_error_state.set(defines::k_bad_bit);
      }

    }
  
  if (!child_event.empty())
    {
      Unpacker* child    = child_with_event_tag.front();
      tag.m_event        = child->get_tag().m_event;
//       orig.m_event       = child->get_tag(k_tag_origin).m_event;
      m_has_tag[k_event]            = true;
      m_tag_status[k_event_inherit] = true;
    }
  if (!child_spill.empty())
    {
      Unpacker* child    = child_with_spill_tag.front();
      tag.m_spill        = child->get_tag().m_spill;
//       orig.m_spill       = child->get_tag(k_tag_origin).m_spill;
      m_has_tag[k_spill]            = true;
      m_tag_status[k_spill_inherit] = true;
    }

  // add own tag to the check-list
  child_local.push_back((tag.m_local - orig.m_local) & max_local);
  
  if (gm_check_mode[defines::k_local])
    {
      if (!check_diff(child_local))
	{
	  // 	  cerr << "\n#E " << m_name << " detects disagreement of local tag" 
	  //	       << std::endl;
	  
	  m_tag[k_tag_diff].back().m_local = tag.m_local;
	  m_error_state.set(defines::k_local_tag_bit);
	}
    }

  // ignore the first event
  if (orig.m_local!=tag.m_local)
    {
      if (gm_check_mode[defines::k_event])
	{
	  if (!check_diff(child_event))
	    {
// 	      cerr << "\n#E " << m_name << " detects disagreement of event tag"
// 		     << std::endl;
	      m_tag[k_tag_diff].back().m_event = tag.m_event;
	      m_error_state.set(defines::k_event_tag_bit);
	    }
	}
      if (gm_check_mode[defines::k_spill])
	{
	  if (!check_diff(child_spill))
	    {
// 	      cerr << "\n#E " << m_name << " detects disagreement of spill tag"
// 		     << std::endl;
	      m_tag[k_tag_diff].back().m_spill =  tag.m_spill;
	      m_error_state.set(defines::k_spill_tag_bit);
	    }
	}
    }
  
//   cout << "#D  check_mismatch() " << m_name
//        << std::hex
//        << " , ml = " << max_local
//        << ", me = " << max.m_event
//        << ", ms = " << max.m_spill
//        << std::dec
//        << std::endl;
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::check_slip()
{
//   cout << "#D check_slip()  " << m_name << " " << m_has_tag << std::endl;

  if (false
      || m_name.find("UNKNOWN")!=std::string::npos 
      || m_name.find("unknown")!=std::string::npos
      ){
    return;
  }

  const Tag& tag    = m_tag[k_tag_current].back();
  const Tag& prev   = m_tag[k_tag_prev].back();
  Tag& slip         = m_tag[k_tag_slip].back();
  const Tag& max    = m_tag[k_tag_max].back();
  const Tag& origin = m_tag[k_tag_origin].back();

  if (tag.m_local==origin.m_local){
    return;  
  }


  if (m_has_tag[k_local])
    {
      if (tag.m_local==prev.m_local)
	{
	  slip.m_local = tag.m_local;
	  m_error_state.set(defines::k_local_unchanged_bit);
	}
      else if (tag.m_local!=((prev.m_local+1)&max.m_local))
	{
	  slip.m_local = tag.m_local;
	  m_error_state.set(defines::k_local_slip_bit);
	}
    }
  
  if ((m_has_tag[k_event]))
    {
      if (tag.m_event==prev.m_event) 
	{
// 	  hoge();
// 	  cerr << "#D " << (m_type.empty() ? "UnpackerImpl" : m_type)
// 	       << " : " << m_name << "\n";
// 	  cerr << "#D eslip1 " << tag.m_event << " " << prev.m_event << std::endl;
	  slip.m_event = tag.m_event;
	  m_error_state.set(defines::k_event_unchanged_bit);
	}
      else if ((tag.m_event!=((prev.m_event+1)&max.m_event))
	       && (prev.m_event!=defines::k_unassigned))
	{
// 	  hoge();
// 	  cerr << "#D " << (m_type.empty() ? "UnpackerImpl" : m_type)
// 	       << " : " << m_name << "\n";
// 	  cerr << "#D eslip2 " << tag.m_event 
// 	       << " " << prev.m_event 
// 	       << " " << max.m_event
// 	       << " " << ((prev.m_event+1)&max.m_event)
// 	       << std::endl;
	  slip.m_event = tag.m_event;
	  m_error_state.set(defines::k_event_slip_bit);
	}
    }

  if (m_has_tag[k_spill])
    {
      if (defines::k_unassigned!=prev.m_spill)
	{
	  if (true
	      && (tag.m_spill!=((prev.m_spill+1) & max.m_spill))
	      && (tag.m_spill!=((prev.m_spill)   & max.m_spill))
	      )
	    {
	      slip.m_spill = tag.m_spill;
	      m_error_state.set(defines::k_spill_slip_bit);
	    }
	}
    }

  return;
}

//______________________________________________________________________________
void
UnpackerImpl::clear()
{
  // this method is called at the beginning of 1 event 
  for(int i = 0; i<(int)m_fe_info.size(); ++i){ m_fe_info[i] = 0;}
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::decode()
{
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::dump_in_binary() const
{
  cout << "#D " << m_type << " dump (binary) " << m_name << "\n";
  iterator_list::const_iterator f = m_first_list.begin();
  iterator_list::const_iterator l = m_last_list.begin();
  for (; (f!=m_first_list.end() && l!=m_last_list.end()); ++f, ++l)
    std::for_each(*f, *l, BitDump());

  return;
}

//______________________________________________________________________________
void
UnpackerImpl::dump_in_decimal() const
{
  cout << "#D " << m_type << " dump(dec) " << m_name << "\n";
  cout << " ... not implemented" << std::endl;
//   std::for_each(m_data_first, m_data_last, DecimalDump());
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::dump_in_hexadecimal() const
{
  cout << "#D " << m_type << " dump (hex) " << m_name << "\n";
  iterator_list::const_iterator f = m_first_list.begin();
  iterator_list::const_iterator l = m_last_list.begin();
  for (; (f!=m_first_list.end() && l!=m_last_list.end()); ++f, ++l)
    std::for_each(*f, *l, HexDump());
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::fill(unsigned int ch,
		   unsigned int data_type,
		   unsigned long long data)
{
//   cout << "#D " << m_type << "::fill() " << m_name
//        << "\n " << ch << " " << data_type << " " << data << std::endl;
//   cout << "#D back = " << m_back << std::endl;
  FrontEndData& fe_data = *m_back;
#ifndef RANGE_CHECK
  if (m_null_ch[ch][data_type])
    return;

  fe_data[ch][data_type]->push_back(data);

#else
  try{
    FECh& fch = fe_data.at(ch);
    try{
      if (m_null_ch.at(ch).at(data_type)){
	return;
      }
      
      Data* d = fch.at(data_type);
      if (!d){
	cerr << "\n#E " << m_type << "::fill()\n "
	     << m_name << "["
	     << std::hex << std::showbase << ch 
	     << "][" 
	     << data_type << "] was not assigned" 
	     << std::dec << std::noshowbase
	     << std::endl;
      }else{
	d->push_back(data);
      }
    }catch (const std::out_of_range& e){
      cerr << "\n#E what = " << e.what() << "\n  " << m_type << "::fill()\n"
	   << " invalid access to " << m_name
	   << "[" 
	   << std::hex << std::showbase << ch 
	   << "][" 
	   << data_type 
	   << "]    max size = [" 
	   << fe_data.size() << "][" << fch.size() <<  "]"
	   << "\n    cannot push_back( " << data << " )\n"
	   << std::dec << std::noshowbase
	   << "     check <FE> in <front_end>"
	   << std::endl;
    }
  }catch (const std::out_of_range& e){
    cerr << "\n#E what = " << e.what() << "\n  " << m_type << "::fill()\n"
	 << " invalid access to " << m_name
	 << "[" 
	 << std::hex << std::showbase 
	 << ch << "][" << data << "]"
	 << " max size = [" << fe_data.size() << "][" << "]\n"
	 << std::dec << std::noshowbase
	 << "     check <FE> in <front_end>"
	 << std::endl;
  }catch (const std::exception& e){
    cerr << "\n#E what = " << e.what() << "\n " << m_type << "::fill()\n   " 
	 << m_name << std::endl;
  }catch (...){
    cerr << "\n#E " << m_type << "::fill()\n   " << m_name << std::endl;
  }
#endif

  return;
}

//______________________________________________________________________________
Unpacker*
UnpackerImpl::get_child(uint64_t id) const
{
  ChildList::const_iterator i = m_child_list.find(id);
  if (i!=m_child_list.end())
    return i->second;
  else
    return 0;
//   return m_child_list[id];
}

//______________________________________________________________________________
const std::map<std::string, int>&
UnpackerImpl::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  return s_data_ref;
}

//______________________________________________________________________________
bool
UnpackerImpl::is_ready()
{
  bool ret = true;
  if(m_data_first == m_data_last){ ret = false; }
  return ret;
}

//______________________________________________________________________________
bool
UnpackerImpl::is_related(int device_id, int plane_id)
{
  bool ret = false;
  int n = m_digit_id_list.size();
  for(int i = 0; i<n; ++i){
    if(true
       &&  device_id == (m_digit_id_list[i][0]).m_device
       && (plane_id  == (m_digit_id_list[i][0]).m_plane || plane_id == -1)
       ){
      ret = true; break;
    }
  }
  
  return ret;
}

//______________________________________________________________________________
void
UnpackerImpl::reset()
{
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::resize_fe_data()
{
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::set_data(unsigned int data)
{
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::set_data(const iterator& data_first,
		       const iterator& data_last)
{
  if (m_first_list.empty())
    m_data_first = data_first;
  m_data_last = data_last;

  m_first_list.push_back(data_first);
  m_last_list.push_back(data_last);
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::set_data_size(unsigned int data_size)
{
  m_data_size += data_size;
  return;
}

//______________________________________________________________________________
void
UnpackerImpl::sort_child()
{
  return;
}

//______________________________________________________________________________
bool
UnpackerImpl::unpack()
{
  return true;
}

//______________________________________________________________________________
void
UnpackerImpl::update_tag()
{
  return;
}

  }
}
