#include <iostream>
#include <iomanip>
#include <ios>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "Utility.hh"
#include "BitDump.hh"

// Main ___________________________________________________________________________
enum argIndex{kBin, kIp, kState, kAddrOffset};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "set_hbfstate [IP address] [HBF state] [Addr offset (hex) (option)]" << std::endl;
    std::cout << std::endl;
    std::cout << "### HBF state option ###" << std::endl;
    std::cout << " - on" << std::endl;
    std::cout << " - off" << std::endl;
    
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip    = argv[kIp];
  std::string hbf_state   = argv[kState];
  std::string addr_offset = "0x00000000";
  if(argc == kAddrOffset+1){
    addr_offset  = argv[kAddrOffset];
  }

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  uint32_t reg_state = 0;
  if(hbf_state == "on"){
    reg_state = 1;
  }else if(hbf_state == "off"){
    reg_state = 0;
  }else{
    Utility::PrintError("", "No such the hbf option");
    return -1;
  }

  std::istringstream iss_addr(addr_offset);
  uint32_t addr_offset_val;
  iss_addr   >> std::hex >> addr_offset_val;


  fpga_module.WriteModule(LBUS::MUTIL::kAddrHbfState + addr_offset_val, reg_state);
  
  return 0;

}// main
