#include <iostream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "FlashMemoryProgrammer.hh"

using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "This program converts the ASCII MCS file to the downloadable binary file.\n";
    std::cout << "Usage\n";
    std::cout << "mcs_converter [In file] [Out file]"<< std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  const std::string board_ip    = "0.0.0.0";
  const std::string input_file  = argv[1];
  const std::string output_file = argv[2];
  
  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);
			  
  HUL::FlashMemoryProgrammer fmp(fpga_module);

  fmp.ReadMCSFile(input_file);
  fmp.SaveBinaryMCSFile(output_file);

  return 0;

}// main
