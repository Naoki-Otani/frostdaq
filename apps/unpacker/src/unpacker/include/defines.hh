// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__DEFINES_H
#define HDDAQ__DEFINES_H

#include <string>
#include <bitset>
#include <vector>
#include <deque>
#include <stdint.h>

namespace hddaq
{
  namespace unpacker
  {

    namespace defines
    {
      const int k_unassigned                = -0xffff;
      const uint32_t k_unassignedU          = 0xffffffffU;
      const uint64_t k_unassignedULL        = 0xffffffffffffffffULL;
      const std::string k_unassigned_string = "unassigned";

      const uint32_t k_event_type_normal    = 0U;
      const uint32_t k_event_type_null      = 1U;
      const uint32_t k_event_type_dummy     = 2U;

      typedef std::string unpacker_type;
      const unpacker_type k_Unknown = "Unknown";
      const unpacker_type k_Null    = "Null";
      
      enum e_error_state
	{
	  // unpacker status is good
	  k_good_bit,
	  // ---- Data format error -------------------------------------
	  // n_word in header is not consistent 
	  // with the distance between itrator begin and end of data
	  k_data_size_bit,
	  // Header is probably broken
	  k_header_bit,
	  // Trailer is probably broken
	  k_trailer_bit,
	  // ---- Event Tag error (current event) -----------------------
	  // At least, local event # in one unpacker is different others
	  k_local_tag_bit,
	  // At least, global event # in one unpacker is different others
	  k_event_tag_bit,
	  // At least, global spill # in one unpacker is different others
	  k_spill_tag_bit,
	  // ---- Event Tag error (comp. current and prev. events) ------
	  // LocalEvent#(current) != LocalEvent#(prev)+1
	  k_local_slip_bit,
	  // GlobalEvent#(current) != GlobalEvent#(prev)+1
	  k_event_slip_bit,
	  // GlobalSpill#(current) != GlobalSpill#(prev)+1
	  k_spill_slip_bit,
	  // LocalEvent#(current) == LocalEvent#(prev)
	  k_local_unchanged_bit,
	  // GlobalEvent#(current) == GlobalEvent#(prev)
	  k_event_unchanged_bit,
	  // Something wrong (dump status and data)
	  k_bad_bit,
	  k_n_error_state
	};

      enum e_check_mode
	{
// 	  k_no_check, // don't check
	  k_format,   // check data format (header, trailer, word count, ...)
	  k_slip1,    // check tag slip
	  k_slip2,    // check tag slip
	  k_local,    // check local tag mismatch between children and itself
	  k_event,    // check event tag mismatch 
	  k_spill,    // check spill tag mismatch 
	  k_show_all,    // all unpackers' tag or only those of front-end nodes'
	  k_show_always,    // show the summary of check even if there is no error
	  k_n_check_mode
	};

      
      enum e_dump_mode
	{
	  k_no_dump,
	  k_binary,
	  k_dec,
	  k_hex,
	  k_n_dump_mode
	};
      
      typedef std::bitset<k_n_error_state> error_state_t;
      typedef std::bitset<k_n_check_mode>  check_mode_t;
      typedef std::bitset<k_n_dump_mode>   dump_mode_t;

    }

// #ifdef DEQUE_BUFFER  
//     typedef std::deque<unsigned long long> Data;
// #else
    typedef std::vector<unsigned long long> Data;
// #endif
    typedef std::deque<Data>                Ch;
    typedef std::deque<Ch>                  Segment;
    typedef std::deque<Segment>             Plane;
    typedef std::deque<Plane>               Device;
    typedef std::deque<Data*>               FECh;
    typedef std::deque<FECh>                FrontEndData;

    typedef std::deque<Device>              DigitList;
    typedef std::deque<FrontEndData>        FEDataList;

    
  }
}
#endif
