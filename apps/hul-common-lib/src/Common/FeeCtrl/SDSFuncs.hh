#ifndef SDSFUNCS_HH
#define SDSFUNCS_HH

namespace HUL{
  class FPGAModule;
};

namespace XilinxSEM
{
  // --------------------------------------------------------------------
  // Core function
  // --------------------------------------------------------------------
  void SetErrorInjectionAddress(HUL::FPGAModule& fpga_module,
				uint32_t command,
				uint32_t addr_linear_frame,
				uint32_t addr_word,
				uint32_t addr_bit
				);


  // --------------------------------------------------------------------
  // Controll functions
  // --------------------------------------------------------------------
  void ResetSEMCore(HUL::FPGAModule& fpga_module);
  void InjectError(HUL::FPGAModule& fpga_module,
		   uint32_t addr_linear_frame,
		   uint32_t addr_word,
		   uint32_t addr_bit
		   );
  void ReadStatus(HUL::FPGAModule& fpga_module);
}

#endif
