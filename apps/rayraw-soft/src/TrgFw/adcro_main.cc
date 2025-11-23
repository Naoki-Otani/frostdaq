#include <iostream>
#include <unistd.h>

#include "RegisterMap.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "MhTdcFuncs.hh"
#include "AdcFuncs.hh"
#include "DaqFuncs.hh"

enum kArgIndex{kBin, kIp, kRunNo, kNumEvent, kWinMax, kWinMin};

using namespace LBUS;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "adcro [IP address]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip   = argv[kIp];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  // Release AdcRo reset
  if(1 == fpga_module.ReadModule(ADC::kAddrAdcRoReset)){
    fpga_module.WriteModule(ADC::kAddrAdcRoReset, 0);
  }

  // Set trigger path //
  uint32_t reg_trg = TRM::kRegL1Ext;
  fpga_module.WriteModule(TRM::kAddrSelectTrigger, reg_trg);

  // Set NIM-IN //
  fpga_module.WriteModule(IOM::kAddrExtL1, IOM::kReg_i_Nimin1);

  // Enable TDC block //
  uint32_t en_block = TDC::kEnLeading | TDC::kEnTrailing;
  fpga_module.WriteModule(TDC::kAddrEnableBlock, en_block);

  // Resest event counter //
  fpga_module.WriteModule(DCT::kAddrResetEvb, 0);

  // AdcRo initialize status
  std::cout.setf(std::ios::unitbuf);
  std::cout << "#D: AdcRo IsReady status: " << std::hex << fpga_module.ReadModule(ADC::kAddrAdcRoIsReady) << std::dec << std::endl;

  // Event Read Cycle //
  //HUL::DAQ::DoTrgDaq(board_ip, run_no, num_event, DCT::kAddrDaqGate);
  
  return 0;

}// main
