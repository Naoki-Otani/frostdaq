#include <iostream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "SDSFuncs.hh"

using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "reset_sem [IP address]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[1];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  XilinxSEM::ResetSEMCore(fpga_module);

  return 0;

}// main

