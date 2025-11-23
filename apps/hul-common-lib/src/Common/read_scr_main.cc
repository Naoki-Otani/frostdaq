#include <iostream>
#include <iomanip>
#include <fstream>
#include <ios>
#include <cstdio>
#include <unistd.h>

#include "RegisterMapCommon.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"

// Main ___________________________________________________________________________
enum argIndex{kBin, kIp, kFilePath, kGate, kApp};
using namespace HUL;
using namespace LBUS;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "read_scr [IP address] [Output file path] [Gate number] [append (option)]" << std::endl;
    std::cout << " *Gate numbe\n";
    std::cout << " - 0: Scaler count w/o gate\n";
    std::cout << " - 1: Scaler count w gate 1\n";
    std::cout << " - 2: Scaler count w gate 2\n";
    std::cout << std::endl;

    return 0;
  }// usage
  
  // body ------------------------------------------------------
  const std::string kBoardIp  = argv[kIp];
  const std::string kOutPath  = argv[kFilePath];
  const std::string kGateNum  = argv[kGate];
  std::ios_base::openmode ios_flag = std::ios::binary;
  if(argc == kApp+1) ios_flag = ios_flag | std::ios::app;

  uint32_t gate_offset = 0;
  if(kGateNum == "1") gate_offset = 0x10000;
  if(kGateNum == "2") gate_offset = 0x20000;

  std::ofstream ofs(kOutPath.c_str(), ios_flag);
  if(!ofs.is_open()){
    std::cerr << "#E: File cannot be not created (" << kFilePath << ")." << std::endl;
    return 0;
  }

  RBCP::UDPRBCP udp_rbcp(kBoardIp, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  const uint32_t kNumReadCh   = fpga_module.ReadModule(SCR::kAddrNumCh, 1);
  const uint32_t kReadLength  = 4*kNumReadCh;

  std::cout << "#D: # of scaler channel: " << kNumReadCh << std::endl;
  std::cout << "#D: Read length is " << kReadLength << " bytes" << std::endl;

  if(0 == (fpga_module.ReadModule(SCR::kAddrStatus) & SCR::kIndexFifoEmpty)){
    // Previous data remains in FOFO
    // Probably, RBCP time out happended during FIFO readout
    std::cout << "#W: FIFO is not empty. RBCP time out?" << std::endl;
    fpga_module.WriteModule(SCR::kAddrScrReset, SCR::kIndexFifoReset, 1);
    fpga_module.WriteModule(SCR::kAddrScrReset, 0, 1);
  }
  
  while(0 == fpga_module.ReadModule(SCR::kAddrLatchCnt + gate_offset)){};

  const uint32_t kByteHeader    = 2*4;
  const uint32_t kHeaderMagic   = 0xff040000 + std::stoi(kGateNum);
  
  uint8_t fifo_data[kReadLength+kByteHeader];
  const uint32_t kOneCycleLength = 128;
  const uint32_t kNumCycle       = kReadLength/kOneCycleLength;
  const uint32_t kLengthRemainder = kReadLength%kOneCycleLength;

  std::copy((uint8_t*)&kHeaderMagic, (uint8_t*)&kHeaderMagic + 4, fifo_data);
  std::copy((uint8_t*)&kNumReadCh,   (uint8_t*)&kNumReadCh + 4,   fifo_data + 4);
  
  for(uint32_t i = 0; i<kNumCycle; ++i){
    fpga_module.ReadModule_nByte(SCR::kAddrReadFIFO, kOneCycleLength);
    auto itr_begin = fpga_module.GetDataIteratorBegin();
    auto itr_end   = fpga_module.GetDataIteratorEnd();
    std::copy(itr_begin, itr_end, fifo_data + i*kOneCycleLength + kByteHeader);
  }

  if(kLengthRemainder){
    fpga_module.ReadModule_nByte(SCR::kAddrReadFIFO, kLengthRemainder);
    auto itr_begin = fpga_module.GetDataIteratorBegin();
    auto itr_end   = fpga_module.GetDataIteratorEnd();
    std::copy(itr_begin, itr_end, fifo_data + kNumCycle*kOneCycleLength + kByteHeader);
  }
	
  ofs.write((char*)fifo_data, kReadLength + kByteHeader);
  
  std::cout << "#D: Scaler is read " << std::endl;

  ofs.close();

  return 0;

}// main
