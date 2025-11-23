#include<ios>
#include<iomanip>
#include<iostream>
#include<cstdio>
#include<fstream>
#include<cstdlib>
#include<unistd.h>

#include"Utility.hh"
#include"RegisterMapCommon.hh"
#include"FPGAModule.hh"
#include"CDCE62002Funcs.hh"
#include"BitDump.hh"

namespace CDCE62002{

  namespace{
    const std::string class_name = "CDCE62002";
  };
  
  // ------------------------------------------------------------------
  // Reg0
  // ------------------------------------------------------------------
  
  // INBUFSET ________________________________________________
  uint32_t
  SetINBUFSEL(uint32_t reg, const INBUF_SEL val)
  {
    const uint32_t mask  = 0xFFFFFFCF;
    const uint32_t shift = kAddressShift;

    uint32_t set = static_cast<const uint32_t>(val);
      
    reg = (reg & mask) | (set << shift);
    return reg;
  }


  // SmartMUX ________________________________________________
  uint32_t
  SetSMARTMUX(uint32_t reg, const SMART_MUX val)
  {
    const uint32_t mask  = 0xFFFFFF3F;
    const uint32_t shift = kAddressShift + 2;

    uint32_t set = static_cast<const uint32_t>(val);
      
    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // ACDCSEL__________________________________________________
  uint32_t
  SetACDCSEL(uint32_t reg, const ACDC_SEL val)
  {
    const uint32_t mask  = 0xFFFFFEFF;
    const uint32_t shift = kAddressShift + 4;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // TERMSEL__________________________________________________
  uint32_t
  SetTERMSEL(uint32_t reg, const TERM_SEL val)
  {
    const uint32_t mask  = 0xFFFFFDFF;
    const uint32_t shift = kAddressShift + 5;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

    
  // REFDIVIDE________________________________________________
  uint32_t
  SetREFDIVIDE(uint32_t reg, const REF_DIVIDE val)
  {
    const uint32_t mask  = 0xFFFFC3FF;
    const uint32_t shift = kAddressShift + 6;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // LOCKW____________________________________________________
  uint32_t
  SetLOCKW(uint32_t reg, const LOCKW val)
  {
    const uint32_t mask  = 0xFFF9FFFF;
    const uint32_t shift = kAddressShift + 13;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

    
  // OUTDIVIDE________________________________________________
  uint32_t
  SetOUTDIVIDE0(uint32_t reg, const OUT_DIVIDE val)
  {
    const uint32_t mask  = 0xFF87FFFF;
    const uint32_t shift = kAddressShift + 15;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  uint32_t
  SetOUTDIVIDE1(uint32_t reg, const OUT_DIVIDE val)
  {
    const uint32_t mask  = 0xF87FFFFF;
    const uint32_t shift = kAddressShift + 19;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // OUTBUFSEL________________________________________________
  uint32_t
  SetOUTBUFSEL0(uint32_t reg, const OUTBUF_SEL val)
  {
    const uint32_t mask  = 0xCFFFFFFF;
    const uint32_t shift = kAddressShift + 24;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  uint32_t
  SetOUTBUFSEL1(uint32_t reg, const OUTBUF_SEL val)
  {
    const uint32_t mask  = 0x3FFFFFFF;
    const uint32_t shift = kAddressShift + 26;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // ------------------------------------------------------------------
  // Reg1
  // ------------------------------------------------------------------
  
  // SELVCO __________________________________________________
  uint32_t
  SetSELVCO(uint32_t reg, const VCO_MODE val)
  {
    const uint32_t mask  = 0xFFFFFFEF;
    const uint32_t shift = kAddressShift;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // SELINDIV ________________________________________________
  uint32_t
  SetSELINDIV(uint32_t reg, const uint32_t divide_ratio)
  {
    if( !(1 <= divide_ratio && divide_ratio <= 256)){
      std::cerr << "#E : CDCE62002::Reg1::SetSELINDIV()" << std::endl;
      std::cerr << "     Arg2 exceeds the range of INDIV (1 - 256)." << std::endl;
      return reg;
    }

    uint32_t val = divide_ratio -1;
      
    const uint32_t mask  = 0xFFFFE01F;
    const uint32_t shift = kAddressShift + 1;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // PRESCALER _______________________________________________
  uint32_t
  SetPRESCALER(uint32_t reg, const PRE_SCALER val)
  {
    const uint32_t mask  = 0xFFFF9FFF;
    const uint32_t shift = kAddressShift + 9;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // SELFBDIV ________________________________________________
  uint32_t
  SetSELFBDIV(uint32_t reg, const FB_DIVIDE val)
  {
    const uint32_t mask  = 0xFF807FFF;
    const uint32_t shift = kAddressShift + 11;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // SELBPDIV  _______________________________________________
  uint32_t
  SetSELBPDIV(uint32_t reg, const BYPASS_DIVIDE val)
  {
    const uint32_t mask  = 0xFC7FFFFF;
    const uint32_t shift = kAddressShift + 19;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // LFRCSEL _________________________________________________
  uint32_t
  SetLFRCSEL(uint32_t reg, const LFRC_SEL val)
  {
    const uint32_t mask  = 0xC3FFFFFF;
    const uint32_t shift = kAddressShift + 22;

    uint32_t set = static_cast<const uint32_t>(val);

    reg = (reg & mask) | (set << shift);
    return reg;
  }

  // ------------------------------------------------------------------
  // Control functions
  // ------------------------------------------------------------------

  // GenSpiTxdReg0 _________________________________________________
  uint32_t
  GenSpiTxdReg0(const Register0& reg_struct)
  {
    static const std::string func_name {class_name + "::" + __func__ + "()"};
    
    uint32_t spi_txd = 0; // Address 0
      
    spi_txd = SetINBUFSEL(spi_txd,   reg_struct.inbuf_sel);	  
    spi_txd = SetSMARTMUX(spi_txd,   reg_struct.smart_mux);	  
    spi_txd = SetACDCSEL(spi_txd,    reg_struct.acdc_sel);	  
    spi_txd = SetTERMSEL(spi_txd,    reg_struct.term_sel);	  
    spi_txd = SetREFDIVIDE(spi_txd,  reg_struct.ref_divide);  
    spi_txd = SetLOCKW(spi_txd,      reg_struct.lockw);	  
    spi_txd = SetOUTDIVIDE0(spi_txd, reg_struct.out0_divide); 
    spi_txd = SetOUTDIVIDE1(spi_txd, reg_struct.out1_divide); 
    spi_txd = SetOUTBUFSEL0(spi_txd, reg_struct.out0_buf_sel);
    spi_txd = SetOUTBUFSEL1(spi_txd, reg_struct.out1_buf_sel);

    Utility::PrintNormal(func_name, "");
    std::cout << std::setfill('0') << std::right;
    std::cout << std::hex << "     0x" << std::setw(8) << spi_txd << "\n";
    std::cout << std::dec;
    std::cout << std::setfill(' ');

    return Utility::BitReverse(spi_txd);
  }

  // GenSpiTxdReg1 _________________________________________________
  uint32_t
  GenSpiTxdReg1(const Register1& reg_struct)
  {
    static const std::string func_name {class_name + "::" + __func__ + "()"};

    uint32_t spi_txd = 0x80000001; // Address 1
      
    spi_txd = SetSELVCO(spi_txd,     reg_struct.vco_mode_sel);	
    spi_txd = SetSELINDIV(spi_txd,   reg_struct.input_divider);	
    spi_txd = SetPRESCALER(spi_txd,  reg_struct.pre_scaler);	
    spi_txd = SetSELFBDIV(spi_txd,   reg_struct.fb_divider);	
    spi_txd = SetSELBPDIV(spi_txd,   reg_struct.bypass_divider);	
    spi_txd = SetLFRCSEL(spi_txd,    reg_struct.lfrc_sel);       

    Utility::PrintNormal(func_name, "");
    std::cout << std::setfill('0') << std::right;
    std::cout << std::hex << "     0x" << std::setw(8) << spi_txd << "\n";
    std::cout << std::dec;
    std::cout << std::setfill(' ');

    return Utility::BitReverse(spi_txd);
  }

  // GenSpiTxdReg2 _________________________________________________
  uint32_t
  GenSpiTxdReg2(const uint32_t bit_mask)
  {
    static const std::string func_name {class_name + "::" + __func__ + "()"};
    
    uint32_t spi_txd = 0x61003CF2; // Address 2
    spi_txd = spi_txd & bit_mask;
      
    Utility::PrintNormal(func_name, "");
    std::cout << std::setfill('0') << std::right;
    std::cout << std::hex << "     0x" << std::setw(8) << spi_txd << "\n";
    std::cout << std::dec;
    std::cout << std::setfill(' ');

    return Utility::BitReverse(spi_txd);
  }
  
  // GenSpiRxd _____________________________________________________
  uint32_t
  GenSpiRxd(uint32_t address)
  {
    const uint32_t kInstruction  = 0xE;  
    address = address & kAddressMask;

    uint32_t reg = 0;
    reg = (address << kAddressShift) | kInstruction;
    return Utility::BitReverse(reg);
  }
  
  // WriteReg ______________________________________________________
  void
  WriteReg(HUL::FPGAModule& fpga_module, uint32_t reg)
  {
    fpga_module.WriteModule(LBUS::C6C::kAddrTxd, reg, 4);
    fpga_module.WriteModule(LBUS::C6C::kAddrExecWrite, 0, 1);
  }
  
  // ReadReg _______________________________________________________
  uint32_t
  ReadReg(HUL::FPGAModule& fpga_module, uint32_t address)
  {
    fpga_module.WriteModule(LBUS::C6C::kAddrTxd, GenSpiRxd(address), 4);
    fpga_module.ReadModule(LBUS::C6C::kAddrExecRead, 1);
    uint32_t ret = fpga_module.ReadModule(LBUS::C6C::kAddrRxd, 4);
    ret = Utility::BitReverse(ret);
  
    std::cout << "#D : CDCE62002::ReadReg() \n";
    std::cout << std::setfill('0') << std::right;
    std::cout << std::hex << "     0x" << std::setw(8) << ret << "\n";
    std::cout << std::dec;
    std::cout << std::setfill(' ');
  
    hddaq::BitDump bdump;
    bdump(ret);

    return ret;
  }

  // CopyToEEPROM __________________________________________________
  void
  CopyToEEPROM(HUL::FPGAModule& fpga_module)
  {
    const uint32_t reg = 0x1F;
    
    fpga_module.WriteModule(LBUS::C6C::kAddrTxd, Utility::BitReverse(reg), 4);
    fpga_module.WriteModule(LBUS::C6C::kAddrExecWrite, 0, 1);
  }

};
