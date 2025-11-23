#include <iostream>
#include <iomanip>
#include <ios>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "Utility.hh"
#include "BitDump.hh"
#include "CDCE62002Funcs.hh"

// Main ___________________________________________________________________________
enum argIndex{kBin, kIp, kSetting};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "set_cdce62002 [IP address] [Pre-defined setting]" << std::endl;
    std::cout << std::endl;
    std::cout << "### Pre-defined settings.###" << std::endl;
    std::cout << "(in_XXX_out_YYY_ZZZ : Input XXX MHz, Out0 YYY MHZ, Out1 ZZZ MHz)" << std::endl;
    for_each(CDCE62002::g_map_reg.begin(), CDCE62002::g_map_reg.end(),
	     [](const auto& itr){std::cout << " - " << itr.first << std::endl;}
	     );
    
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[kIp];
  std::string setting  = argv[kSetting];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  const auto itr = CDCE62002::g_map_reg.find(setting);
  if(itr == CDCE62002::g_map_reg.end()){
    std::string message("No such setting: " + setting);
    Utility::PrintError("main()", message);
    return -1;
  }
  
  const auto& [register0, register1] = itr->second;
  
  //  CDCE62002::WriteReg(fpga_module, CDCE62002::Reg2::GenSpiTxd());
  CDCE62002::WriteReg(fpga_module, CDCE62002::GenSpiTxdReg0(register0));
  CDCE62002::WriteReg(fpga_module, CDCE62002::GenSpiTxdReg1(register1));

  CDCE62002::ReadReg(fpga_module, 0);
  CDCE62002::ReadReg(fpga_module, 1);
  //  CDCE62002::ReadReg(fpga_module, 2);
  
  CDCE62002::CopyToEEPROM(fpga_module);

  return 0;

}// main
