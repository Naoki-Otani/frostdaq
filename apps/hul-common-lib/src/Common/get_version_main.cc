#include <iostream>
#include <iomanip>
#include <ios>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "FWMagicMap.hh"
#include "Utility.hh"

using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "get_version [IP address]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[1];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);
  uint32_t bct_version = fpga_module.ReadModule(LBUS::BCT::kAddrVersion, 4);
  uint32_t major_version = (bct_version >> 8) & 0xff;
  uint32_t minor_version = (bct_version) & 0xff;

  std::cout << "#D: FEE Firmware" << std::endl;
  std::cout << std::hex;
  std::cout << std::setfill('0') << std::right 
	    << "FW ID      : 0x" << std::setw(4) << ((bct_version >> 16) & 0xffff) << std::endl;
  std::cout << std::setfill(' ') << std::dec;  
  std::cout << "FW version : " << major_version << "."
	    << minor_version
	    << std::endl;

  uint32_t fw_magic = ((bct_version >> 16) & 0xffff);
  const auto itr = HUL::g_fw_map.find(fw_magic);
  if(itr == HUL::g_fw_map.end()){
    Utility::PrintError("", "Unknown firmware");
  }else{
    Utility::PrintNormal("", itr->second);
  }

  return 0;

}// main
