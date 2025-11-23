#include<iostream>
#include<stdint.h>

#include"Utility.hh"
#include"SDSFuncs.hh"
#include"FPGAModule.hh"
#include"RegisterMapCommon.hh"

namespace XilinxSEM
{
  namespace{
    const std::string class_name = "XilinxSEM";
  };
  
  using namespace HUL;
  using namespace LBUS;

  // --------------------------------------------------------------------
  // Core function
  // --------------------------------------------------------------------
  void
  SetErrorInjectionAddress(HUL::FPGAModule& fpga_module,
			   uint32_t command,
			   uint32_t addr_linear_frame,
			   uint32_t addr_word,
			   uint32_t addr_bit
			      )
  {
    // command              :  4bit
    // linear frame address : 17bit
    // word address         :  7bit
    // bit address          :  5bit
    const uint32_t mask_linear = 0x1ffff;
    const uint32_t mask_word   = 0x7f;
    const uint32_t mask_bit    = 0x1f;

    const uint32_t shift_word   = 5;
    const uint32_t shift_linear = 7 + shift_word;

    // For command (5th byte)
    const uint32_t mask_command  = 0xf;
    const uint32_t shift_command = 4;

    uint32_t reg_1st_4th = addr_bit & mask_bit;
    reg_1st_4th = reg_1st_4th | ((addr_word & mask_word) << shift_word);
    reg_1st_4th = reg_1st_4th | ((addr_linear_frame & mask_linear) << shift_linear);
    fpga_module.WriteModule(SDS::kAddrSemErrAddr, reg_1st_4th, 4);

    uint32_t reg_5th = (command & mask_command) << shift_command;
    fpga_module.WriteModule(SDS::kAddrSemErrAddr+0x40000, reg_5th, 1);

    return;
  }

  
  // --------------------------------------------------------------------
  // Control function
  // --------------------------------------------------------------------

  // ResetSEMCore ______________________________________________________
  void
  ResetSEMCore(HUL::FPGAModule& fpga_module)
  {
    static const std::string func_name {class_name + "::" + __func__ + "()"};

    Utility::PrintNormal(func_name, "");
    SetErrorInjectionAddress(fpga_module, SDS::kSetIdle, 0, 0, 0);
    fpga_module.WriteModule(SDS::kAddrSemErrStrobe, 1);

    //  sleep(1);
  
    SetErrorInjectionAddress(fpga_module, SDS::kSetReset, 0, 0, 0);
    fpga_module.WriteModule(SDS::kAddrSemErrStrobe, 1);

    return;
  }

  // InjectError ______________________________________________________
  void
  InjectError(HUL::FPGAModule& fpga_module,
	      uint32_t addr_linear_frame,
	      uint32_t addr_word,
	      uint32_t addr_bit
	      )
  {
    static const std::string func_name {class_name + "::" + __func__ + "()"};
    
    Utility::PrintNormal(func_name, "");
    SetErrorInjectionAddress(fpga_module, SDS::kSetIdle, 0, 0, 0);
    fpga_module.WriteModule(SDS::kAddrSemErrStrobe, 1);

    SetErrorInjectionAddress(fpga_module, SDS::kSetError, addr_linear_frame, addr_word, addr_bit);
    fpga_module.WriteModule(SDS::kAddrSemErrStrobe, 1);

    SetErrorInjectionAddress(fpga_module, SDS::kSetObserve, 0, 0, 0);
    fpga_module.WriteModule(SDS::kAddrSemErrStrobe, 1);
  }

  // ReadStatus _______________________________________________________
  void
  ReadStatus(HUL::FPGAModule& fpga_module)
  {
    static const std::string func_name {class_name + "::" + __func__ + "()"};
    
    const uint32_t sem_correction_count
      = fpga_module.ReadModule(SDS::kAddrSemCorCount, 2);

    const uint32_t status
      = fpga_module.ReadModule(SDS::kAddrSdsStatus, 1);

    Utility::PrintNormal(func_name, "");
    std::cout << "Num. correction. : " << sem_correction_count << std::endl;
    std::cout << std::endl;

    std::cout << "SEM error status (0:nomarl, 1:error)" << std::endl;
    std::cout << " - Sem watchdog      : " << ((status & SDS::kSemWatchdogAlarm) >> 4) << std::endl;
    std::cout << " - Sem uncorrectable : " << ((status & SDS::kSemUncorrectableAlarm) >> 5) << std::endl;

    return;
  }
}// End of namespace SDS
