#include <iostream>
#include <iomanip>
#include <ios>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"

// Main ___________________________________________________________________________
enum argIndex{kBin, kIp, kDac};
using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc || 2 == argc){
    std::cout << "Usage\n";
    std::cout << "set_max1932 [IP address] [DAC code (dec)]" << std::endl;
    std::cout << std::endl;
    std::cout << " DAC code can be set between 1 to 255" << std::endl;
    std::cout << " -   1 : Maximum voltage" << std::endl;
    std::cout << " - 255 : Minimum voltage (Note that min. voltage is not 0V but around 30V)" << std::endl;
    std::cout << " -   0 : Off (almost 0V)" << std::endl;
    
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[kIp];
  int set_parameter    = atoi(argv[kDac]);
  
  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  fpga_module.WriteModule(LBUS::BIAS::kAddrTxd, set_parameter);
  fpga_module.WriteModule(LBUS::BIAS::kAddrExecWrite, 0);

  return 0;

}// main
