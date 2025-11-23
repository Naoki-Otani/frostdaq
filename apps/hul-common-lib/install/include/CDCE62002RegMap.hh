#ifndef CDCE62002REGMAP_HH
#define CDCE62002REGMAP_HH

#include<stdint.h>
#include<tuple>
#include<map>

namespace CDCE62002{
  // Register 0 definition _______________________________________________
  enum class INBUF_SEL : uint32_t
    {
      kDisableIn = 0,
      kLVDSIn    = 1,
      kLVPECLIn  = 2,
      kLVCMOSIn  = 3
    };

  enum class SMART_MUX : uint32_t
    {
      kReserveMux = 0,
      kRefIn      = 1,
      kAUXIn      = 2,
      kAuto       = 3
    };
    
  enum class ACDC_SEL : uint32_t
    {
      kACIn       = 0,
      kDCIn       = 1
    };

  enum class TERM_SEL : uint32_t
    {
      kEnTerm       = 0,
      kDisEnTerm    = 1
    };

  enum class REF_DIVIDE : uint32_t
    {
      kRefDiv1      = 0,
      kRefDiv2      = 1,
      kRefDiv3      = 2,
      kRefDiv4      = 3,
      kRefDiv5      = 4,
      kRefDiv6      = 5,
      kRefDiv7      = 6,
      kRefDiv8      = 7,
      kRefDiv10     = 12,
      kRefDiv12     = 13,
      kRefDiv14     = 14,
      kRefDiv16     = 15
    };

  enum class LOCKW : uint32_t
    {
      kLock21  = 0,
      kLock46  = 1,
      kLock72  = 2,
      kLock199 = 3
    };

  enum class OUT_DIVIDE : uint32_t
    {
      kDisableOutDiv = 0,
      kOutDiv1    = 1,
      kOutDiv2    = 2,
      kOutDiv3    = 3,
      kOutDiv4    = 4,
      kOutDiv5    = 5,
      kOutDiv6    = 6,
      kOutDiv8    = 8,
      kOutDiv10   = 10,
      kOutDiv20   = 11,
      kOutDiv12   = 12,
      kOutDiv24   = 13,
      kOutDiv16   = 14,
      kOutDiv32   = 15
    };


  enum class OUTBUF_SEL : uint32_t
    {
      kDisableOut = 0,
      kLVDSOut    = 1,
      kLVCMOSOut  = 2,
      kLVPECLOut  = 3
    };

  // Register 0 struct //
  struct Register0
  {
    const INBUF_SEL  inbuf_sel;
    const SMART_MUX  smart_mux;
    const ACDC_SEL   acdc_sel;
    const TERM_SEL   term_sel;
    const REF_DIVIDE ref_divide;
    const LOCKW      lockw;
    const OUT_DIVIDE out0_divide;
    const OUT_DIVIDE out1_divide;
    const OUTBUF_SEL out0_buf_sel;
    const OUTBUF_SEL out1_buf_sel;
  };

  // Register 1 definition _______________________________________________  
  enum class VCO_MODE : uint32_t
    {
      kVCOLow  = 0,
      kVCOHigh = 1
    };

  enum class PRE_SCALER : uint32_t
    {
      kPreScale2 = 3,
      kPreScale3 = 1,
      kPreScale4 = 2,
      kPreScale5 = 0
    };

  enum class FB_DIVIDE : uint32_t
    {
      kFBDIV8   = 0,
      kFBDIV12  = 1,
      kFbDiv16  = 2,
      kFbDiv20  = 3,
      kFbDiv24  = 5,
      kFbDiv32  = 6
    };
  
  enum class BYPASS_DIVIDE : uint32_t
    {
      kBpDiv2   = 0,
      kBpDiv5   = 1,
      kBpDiv8   = 2,
      kBpDiv10  = 3,
      kBpDiv16  = 4,
      kBpDiv20  = 5,
      kBPBypass = 7
    };

  enum class LFRC_SEL : uint32_t
    {
      kRCINT0   = 0, // Internal loop filter 0
      kRCINT1   = 1, // Internal loop filter 1
      kRCINT2   = 2, // Internal loop filter 2
      kRCINT3   = 3, // Internal loop filter 3
      // External loop filter is not optimized. Do not use them.
      kLFRCSEL  = 4
    };

  // Register 1 struct _______________________________________________
  struct Register1
  {
    const VCO_MODE      vco_mode_sel;
    const uint32_t      input_divider;
    const PRE_SCALER    pre_scaler;
    const FB_DIVIDE     fb_divider;
    const BYPASS_DIVIDE bypass_divider;
    const LFRC_SEL      lfrc_sel;
  };
    
  // Register Map ____________________________________________________
  using TupleReg = std::tuple<const Register0, const Register1>;
  using MapReg   = std::map<const std::string, const TupleReg>;

  const MapReg g_map_reg =
    {
      // ----------------------------------------------------------
      // Input freq: 100 MHz
      // Output freq
      // - OUT0: 500 MHz
      // - OUT1: 100 MHz
      // ----------------------------------------------------------
      {"in_100_out_500_100", {
	  {// Register 0 __________________________
	    INBUF_SEL::kLVDSIn,
	    SMART_MUX::kRefIn,
	    ACDC_SEL::kDCIn,
	    TERM_SEL::kEnTerm,
	    REF_DIVIDE::kRefDiv1,
	    LOCKW::kLock21,
	    OUT_DIVIDE::kOutDiv1,  // OUT_DIVIDE_0
	    OUT_DIVIDE::kOutDiv5,  // OUT_DIVIDE_1
	    OUTBUF_SEL::kLVDSOut,  // OUT_BUF_0
	    OUTBUF_SEL::kLVDSOut   // OUT_BUF_1
	  },
	  {// Register 1 __________________________
	    VCO_MODE::kVCOLow,
	    4,                     // Input divider ratio
	    PRE_SCALER::kPreScale4,
	    FB_DIVIDE::kFbDiv20,  
	    BYPASS_DIVIDE::kBPBypass,
	    LFRC_SEL::kRCINT3
	  }
	}// End of Tuple
      },// End of Map Content
      
      // ----------------------------------------------------------
      // Input freq: 125 MHz
      // Output freq
      // - OUT0: 500 MHz
      // - OUT1: 125 MHz
      // ----------------------------------------------------------
      {"in_125_out_500_125", {
	  {// Register 0 __________________________
	    INBUF_SEL::kLVDSIn,
	    SMART_MUX::kRefIn,
	    ACDC_SEL::kDCIn,
	    TERM_SEL::kEnTerm,
	    REF_DIVIDE::kRefDiv1,
	    LOCKW::kLock21,
	    OUT_DIVIDE::kOutDiv1,  // OUT_DIVIDE_0
	    OUT_DIVIDE::kOutDiv4,  // OUT_DIVIDE_1
	    OUTBUF_SEL::kLVDSOut,  // OUT_BUF_0
	    OUTBUF_SEL::kLVDSOut   // OUT_BUF_1
	  },
	  {// Register 1 __________________________
	    VCO_MODE::kVCOLow,
	    5,                     // Input divider ratio
	    PRE_SCALER::kPreScale4,
	    FB_DIVIDE::kFbDiv20,  
	    BYPASS_DIVIDE::kBPBypass,
	    LFRC_SEL::kRCINT3
	  }
	}// End of Tuple
      },// End of Map Content
      
      // ----------------------------------------------------------
      // Input freq: 125 MHz
      // Output freq
      // - OUT0: 625 MHz
      // - OUT1: 125 MHz
      // ----------------------------------------------------------
      {"in_125_out_625_125", {
	  {// Register 0 __________________________
	    INBUF_SEL::kLVDSIn,
	    SMART_MUX::kRefIn,
	    ACDC_SEL::kDCIn,
	    TERM_SEL::kEnTerm,
	    REF_DIVIDE::kRefDiv1,
	    LOCKW::kLock21,
	    OUT_DIVIDE::kOutDiv1,  // OUT_DIVIDE_0
	    OUT_DIVIDE::kOutDiv5,  // OUT_DIVIDE_1
	    OUTBUF_SEL::kLVDSOut,  // OUT_BUF_0
	    OUTBUF_SEL::kLVDSOut   // OUT_BUF_1
	  },
	  {// Register 1 __________________________
	    VCO_MODE::kVCOLow,
	    4,                     // Input divider ratio
	    PRE_SCALER::kPreScale3,
	    FB_DIVIDE::kFbDiv20,  
	    BYPASS_DIVIDE::kBPBypass,
	    LFRC_SEL::kRCINT3
	  }
	}// End of Tuple
      },// End of Map Content

      // ----------------------------------------------------------
      // Input freq: 75 MHz
      // Output freq
      // - OUT0: 375 MHz
      // - OUT1: 75 MHz
      // ----------------------------------------------------------
      {"in_75_out_375_75", {
	  {// Register 0 __________________________
	    INBUF_SEL::kLVDSIn,
	    SMART_MUX::kRefIn,
	    ACDC_SEL::kDCIn,
	    TERM_SEL::kEnTerm,
	    REF_DIVIDE::kRefDiv1,
	    LOCKW::kLock21,
	    OUT_DIVIDE::kOutDiv1,  // OUT_DIVIDE_0
	    OUT_DIVIDE::kOutDiv5,  // OUT_DIVIDE_1
	    OUTBUF_SEL::kLVDSOut,  // OUT_BUF_0
	    OUTBUF_SEL::kLVDSOut   // OUT_BUF_1
	  },
	  {// Register 1 __________________________
	    VCO_MODE::kVCOLow,
	    4,                     // Input divider ratio
	    PRE_SCALER::kPreScale5,
	    FB_DIVIDE::kFbDiv20,  
	    BYPASS_DIVIDE::kBPBypass,
	    LFRC_SEL::kRCINT3
	  }
	}// End of Tuple
      },// End of Map Content
      
      // ----------------------------------------------------------
      // Input freq: 50 MHz
      // Output freq
      // - OUT0: 250 MHz
      // - OUT1: 50 MHz
      // ----------------------------------------------------------
      {"in_50_out_250_50", {
	  {// Register 0 __________________________
	    INBUF_SEL::kLVDSIn,
	    SMART_MUX::kRefIn,
	    ACDC_SEL::kDCIn,
	    TERM_SEL::kEnTerm,
	    REF_DIVIDE::kRefDiv1,
	    LOCKW::kLock21,
	    OUT_DIVIDE::kOutDiv2,  // OUT_DIVIDE_0
	    OUT_DIVIDE::kOutDiv10,  // OUT_DIVIDE_1
	    OUTBUF_SEL::kLVDSOut,  // OUT_BUF_0
	    OUTBUF_SEL::kLVDSOut   // OUT_BUF_1
	  },
	  {// Register 1 __________________________
	    VCO_MODE::kVCOLow,
	    2,                     // Input divider ratio
	    PRE_SCALER::kPreScale4,
	    FB_DIVIDE::kFbDiv20,  
	    BYPASS_DIVIDE::kBPBypass,
	    LFRC_SEL::kRCINT3
	  }
	}// End of Tuple
      },// End of Map Content
      
      // ----------------------------------------------------------
      // Input freq: 100 MHz
      // Output freq
      // - OUT0: 250 MHz
      // - OUT1: 50 MHz
      // ----------------------------------------------------------
      {"in_100_out_250_50", {
	  {// Register 0 __________________________
	    INBUF_SEL::kLVDSIn,
	    SMART_MUX::kRefIn,
	    ACDC_SEL::kDCIn,
	    TERM_SEL::kEnTerm,
	    REF_DIVIDE::kRefDiv1,
	    LOCKW::kLock21,
	    OUT_DIVIDE::kOutDiv2,  // OUT_DIVIDE_0
	    OUT_DIVIDE::kOutDiv10,  // OUT_DIVIDE_1
	    OUTBUF_SEL::kLVDSOut,  // OUT_BUF_0
	    OUTBUF_SEL::kLVDSOut   // OUT_BUF_1
	  },
	  {// Register 1 __________________________
	    VCO_MODE::kVCOLow,
	    4,                     // Input divider ratio
	    PRE_SCALER::kPreScale4,
	    FB_DIVIDE::kFbDiv20,  
	    BYPASS_DIVIDE::kBPBypass,
	    LFRC_SEL::kRCINT3
	  }
	}// End of Tuple
      }// End of Map Content

    };
  
}// End of namespace CDCE62002

#endif
