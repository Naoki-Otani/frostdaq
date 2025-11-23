#include <iostream>
#include <cstdio>

#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "DaqFuncs.hh"

enum argIndex{kBin, kIp, kRunNo};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "daq [IP address] [RunNo.]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[kIp];
  int32_t runno        = atoi(argv[kRunNo]);

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);
  HUL::DAQ::DoStrDaq(board_ip, runno);

  return 0;
  
}// main
