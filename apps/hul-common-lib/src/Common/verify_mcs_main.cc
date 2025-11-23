#include <iostream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "FlashMemoryProgrammer.hh"

void ParseOption(const std::vector<std::string> arg,
		 std::string& ip_address,
		 std::string& mcs_file,
		 bool& is_binary_mode
		 );

using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Verify MCS\n";
    std::cout << "Usage\n";
    std::cout << "verify_mcs [IP address] [MCS file]" << std::endl;
    std::cout << " Options\n";
    std::cout << " -b: Use binary MCS file." << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::vector<std::string> in_arg(argc-1);
  std::copy(argv+1, argv+argc, in_arg.begin());

  std::string ip_address;
  std::string mcs_file_name;
  bool is_binary_mode = false;
  ParseOption(in_arg, ip_address, mcs_file_name, is_binary_mode);
  
  RBCP::UDPRBCP udp_rbcp(ip_address, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);
  HUL::FlashMemoryProgrammer fmp(fpga_module);

#ifdef DEBUG
  fmp.EnableDebugMode();
#endif

  if(is_binary_mode){
    fmp.ReadBinaryMCSFile(mcs_file_name);
  }else{
    fmp.ReadMCSFile(mcs_file_name);
  }

  fmp.InitializeRegister();
  fmp.SendDummyClock();
  const std::string device_name = fmp.CheckSpiDevice();

  if(false
     || device_name == "s25fl128s"
     || device_name == "s25fl256s"
     || device_name == "mt25ql512"
     ){
    fmp.EnableExAddrMode();
  }

  fmp.VerifyMCS();

  return 0;

}// main

void
ParseOption(const std::vector<std::string> arg,
	    std::string& ip_address,
	    std::string& mcs_file,
	    bool& is_binary_mode
	    )
{
  enum Index{kIp, kFile};

  for(int i = 0, opt = 0, n = arg.size(); i<n; ++i){
    if(arg.at(i) == "-b"){
      is_binary_mode = true;
      continue;
    }

    if(opt == static_cast<int>(kIp))   ip_address = arg.at(i);
    if(opt == static_cast<int>(kFile)) mcs_file   = arg.at(i);
    ++opt;
  }// for(i)
}
