#include<ios>
#include<iomanip>
#include<iostream>
#include<cstdio>
#include<fstream>
#include<cstdlib>
#include<unistd.h>

#include"Utility.hh"
#include"RegisterMapCommon.hh"
#include"FPGAModule.hh"
#include"FlashMemoryProgrammer.hh"
#include"SPIFlashMemoryMap.hh"

namespace HUL{

namespace{
  const std::string class_name = "FlashMemoryProgrammer";
};

using namespace LBUS;
  
// -----------------------------------------------------------------
// Public
// -----------------------------------------------------------------

// Constructor _____________________________________________________
FlashMemoryProgrammer::FlashMemoryProgrammer(FPGAModule& fModule)
  : fpga_module_(fModule)
{

}

// Destructor ______________________________________________________
FlashMemoryProgrammer::~FlashMemoryProgrammer()
{

}

// CheckSpiDevice __________________________________________________
std::string
FlashMemoryProgrammer::CheckSpiDevice()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: Start " << func_name << std::endl;
  }

  const uint32_t device_id = ReadSpiDeviceID();
  if(device_id == 0xffffff){
    std::cerr << "#E: " << func_name
	      << " Fail ReadDeviceID"
	      << std::endl;
    std::exit(-1);
  }

  const auto itr = SPI::g_reverse_spi.find(device_id);
  if(itr == SPI::g_reverse_spi.end()){
    std::cerr << "#E: " << func_name
	      << " No such device ID(0x" << std::hex << device_id << ")"
	      << std::dec
	      << std::endl;
    std::exit(-1);
  }

  if(conf_reg_[kIndexDevice] && current_device_ != itr->second){
    std::cerr << "#E: " << func_name << "\n"
	      << " Device mismatch\n"
	      << " - Given device:  " << current_device_ << "\n"
      	      << " - Target device: " << itr->second << "\n"
	      << std::endl;
    std::exit(-1);
  }

  // Device found
  current_device_ = itr->second;
  conf_reg_.set(kIndexDevice);

  std::cerr << "#D: " << func_name << "\n"
	    << " Target device:  " << itr->second << "\n"
	    << std::endl;

  return itr->second;
}

// EraseFlashMemory ________________________________________________
void
FlashMemoryProgrammer::EraseFlashMemory()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: Start " << func_name << std::endl;
  }

  SetWriteEnable();
  BulkErase();

  std::cout << "#D: " << func_name << std::endl;
  
  do{
    Utility::FlashMessage(" Erasing");
    sleep(1);
  }while(ReadWIP() == 1);

  std::cout << std::endl;
  std::cout << "#D: " << func_name << "\n"
	    << " Flash memory erased.\n"
	    << std::endl;

  SetWriteDisable();

  return; 
}

// ProgramFlashMemory __________________________________________________
void
FlashMemoryProgrammer::ProgramFlashMemory()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  if(!conf_reg_[kIndexDevice]){
    std::cerr << "#E: " << func_name << "\n"
	      << " Target SPI flash memory type is not given."
	      << std::endl;
    std::exit(-1);
  }

  if(!conf_reg_[kIndexMcsReady]){
    std::cerr << "#E: " << func_name << "\n"
	      << " MCS file is not loaded."
	      << std::endl;
    std::exit(-1);
  }
  
  const SPI::Device device = std::get<SPI::kDevice>((SPI::g_spi.find(current_device_)->second));
  const uint32_t page_size = device.page_size;
  
  int32_t extra_size = page_size - mcs_data_binary_.size()%page_size;
  BinaryData mcs_binary(mcs_data_binary_.size()+extra_size, 0xff);
  std::copy(mcs_data_binary_.begin(), mcs_data_binary_.end(),
	    mcs_binary.begin());
  
  uint32_t num_page_write = mcs_binary.size()/page_size;
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: " << func_name << "\n"
	      << std::dec
	      << " Write page number: " << num_page_write
	      << std::endl;
  }

  std::cout << "#D: " << func_name << "\n"
	    << " Proceeding program."
	    << std::endl;

  for(uint32_t i = 0; i<num_page_write; ++i){
    if(!conf_reg_[kIndexDebug]){
      Utility::PrintProgressBar(num_page_write-1, i);
    } 
    
    if(conf_reg_[kIndexDebug]){
      std::cout << "#D: " << func_name << "\n"
		<< std::dec
		<< " Page write: " << i << "-th page"
		<< std::endl;
    }

    uint8_t page_data[page_size];
    auto itr_page_start = mcs_binary.begin() + page_size*i;
    auto itr_page_end   = mcs_binary.begin() + page_size*(i+1);
    std::copy(itr_page_start, itr_page_end, page_data);

    if(conf_reg_[kIndexDebug]){
      for(uint32_t i = 0; i<page_size; ++i){
	std::cout << std::hex << std::setw(2) << std::setfill('0')
		  << (uint32_t)(page_data[i] & 0xff) << " ";
	if((i+1)%16 == 0) std::cout << std::endl;
      }
      std::cout << std::endl;
    }

    SetWriteEnable();
    
    const uint32_t page_address = page_size*i;
    DoPageProgram(page_address, page_size, page_data);
  }// for(i)

  sleep(1);
  
  std::cout << "#D: " << func_name << "\n"
	    << " Program done.\n"
	    << std::endl;

  SetWriteDisable();
  
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
  }

  return;
}


// ReadMCSFile _____________________________________________________
void
FlashMemoryProgrammer::ReadMCSFile(const std::string file_name)
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  
  std::ifstream ifs(file_name.c_str());
  if(!ifs.is_open()){
    std::cerr << "#E: " << func_name
	      << " No such file(" << file_name << ")"
	      << std::endl;
    std::exit(-1);
  }

  mcs_file_path_ = file_name;
  mcs_data_binary_.clear();

  // MCS Command
  const uint32_t kNumCommand = 9;
  const uint32_t kIndexType  = 8;

  int32_t byte_address = 0;
  while(!ifs.eof()){
    std::string line;
    getline(ifs, line);
    if(line == "") continue;
    
    std::string command = line.substr(0, kNumCommand);
    //    std::cout << command << std::endl;

    // Separator & file end
    if(command[kIndexType] == '4' || command[kIndexType] == '1') continue;

    int32_t num_byte = std::stoi(command.substr(1, 2), nullptr, 16);
    std::string ascii_data = line.substr(kNumCommand, 2*num_byte);

    if(conf_reg_[kIndexDebug]){
      std::cout << std::setw(8) << std::setfill('0')
		<< std::hex << byte_address;
      std::cout << "  ";
    } 

    for(int32_t i = 0; i<num_byte; ++i){
      int32_t binary_data = std::stoi(ascii_data.substr(2*i, 2), nullptr, 16);
      if(conf_reg_[kIndexDebug]){
	std::cout << std::hex << std::setw(2) << std::setfill('0')
		  << binary_data << " ";
      }
      mcs_data_binary_.push_back(static_cast<uint8_t>(binary_data & 0xff));
    }// for(i)
    
    if(conf_reg_[kIndexDebug]) std::cout << std::endl;
    byte_address += num_byte;
  }// while
  
  std::cout << "#D: " << func_name << "\n"
	    << " MCS file:    " << file_name << "\n"
	    << std::dec
    	    << " Binary size: " << byte_address/1000./1000. << "MB ("<<byte_address<<" bytes)\n"
	    <<  std::endl;
  
  conf_reg_.set(kIndexMcsReady);
}

// ReadBinaryMCSFile _______________________________________________
void
FlashMemoryProgrammer::ReadBinaryMCSFile(const std::string file_name)
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  
  std::ifstream ifs(file_name.c_str(), std::ios::binary);
  if(!ifs.is_open()){
    std::cerr << "#E: " << func_name
	      << " No such file(" << file_name << ")"
	      << std::endl;
    std::exit(-1);
  }

  binary_mcs_file_path_ = file_name;
  
  ifs.seekg(0, std::fstream::end);
  const uint32_t pos_file_end = ifs.tellg();
  ifs.clear();
  ifs.seekg(0, std::fstream::beg);
  const uint32_t pos_file_begin = ifs.tellg();

  const uint32_t size_file = pos_file_end - pos_file_begin;
  mcs_data_binary_.resize(size_file);

  ifs.read((char*)mcs_data_binary_.data(), size_file);
  ifs.close();
  
  std::cout << "#D: " << func_name << "\n"
	    << " Binary MCS file:  " << file_name << "\n"
	    << " File size: " << size_file/1000./1000. << "MB ("<<size_file<<" bytes)\n"
	    <<  std::endl;
  
  conf_reg_.set(kIndexMcsReady);
}

// SaveBinaryMCSFile _______________________________________________
void
FlashMemoryProgrammer::SaveBinaryMCSFile(const std::string file_name)
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(!conf_reg_[kIndexMcsReady]){
    std::cerr << "#E: " << func_name << "\n"
	      << " MCS is not loadded."
	      << std::endl;
    std::exit(-1);
  }

  std::ofstream ofs(file_name.c_str(), std::ios::binary);
  ofs.write((char*)mcs_data_binary_.data(), mcs_data_binary_.size());
  ofs.close();
}

// SetSpiDevice ________________________________________________________
void
FlashMemoryProgrammer::SetSpiDevice(const std::string device_name)
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");

  const auto itr_tuple = SPI::g_spi.find(device_name);
  if(itr_tuple == SPI::g_spi.end()){
    std::cerr << "#E: " << func_name 
	      << " No such the device (" << device_name << ")"
	      << std::endl;
    std::exit(-1);
  }

  if(conf_reg_[kIndexDevice] && current_device_ != device_name){
    std::cerr << "#E: " << func_name  << "\n"
	      << " Device mismatch\n"
	      << " - Current devie:   " << current_device_ << "\n"
      	      << " - Your suggestion: " << device_name << "\n"
	      << std::endl;
    std::exit(-1);
  }
  
  current_device_ = device_name;
  conf_reg_.set(kIndexDevice);
}

// VerifyMCS ___________________________________________________________
void
FlashMemoryProgrammer::VerifyMCS()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  if(!conf_reg_[kIndexDevice]){
    std::cerr << "#E: " << func_name << "\n"
	      << " Target SPI flash memory type is not given."
	      << std::endl;
    return;
  }

  const SPI::Device device = std::get<SPI::kDevice>((SPI::g_spi.find(current_device_)->second));
  const uint32_t page_size = device.page_size;
  uint32_t num_page_read   = 0;
  if(conf_reg_[kIndexMcsReady]){
    num_page_read = mcs_data_binary_.size()%page_size == 0?
      mcs_data_binary_.size()/page_size : mcs_data_binary_.size()/page_size +1;
  }else{
    num_page_read = device.max_size/page_size;
  }
  
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: " << func_name << "\n"
	      << " Read page number: " << num_page_read
	      << std::endl;
  }

  std::cout << "#D: " << func_name << "\n"
	    << " Proceeding verify."
	    << std::endl;
  
  readback_mcs_data_.resize(page_size*num_page_read);
  SetPageReadMode(page_size);  
  for(uint32_t i = 0; i<num_page_read; ++i){
    if(!conf_reg_[kIndexDebug]){
      Utility::PrintProgressBar(num_page_read-1, i);
    } 
    
    if(conf_reg_[kIndexDebug]){
      std::cout << "#D: " << func_name << "\n"
		<< " Page read: " << i << "-th page"
		<< std::endl;
    }

    uint8_t page_data[page_size];
    const uint32_t page_address = page_size*i;
    //    DoFastPageRead(page_address, page_size, page_data);
    DoPageRead(page_address, page_size, page_data);
    std::copy(page_data, page_data+page_size,
	      readback_mcs_data_.begin() + page_size*i);    
  }

  std::cout << "#D: " << func_name << "\n"
	    << " Readback done.\n"
	    << std::endl;

  readback_mcs_data_.resize(mcs_data_binary_.size());
  if(readback_mcs_data_ == mcs_data_binary_){
    std::cout << "#D: " << func_name << "\n"
	      << " MCS data is Verified."
	      << std::endl;
  }else{
    std::cerr << "#E: " << func_name << "\n"
	      << " MCS data mismatch"
	      << std::endl;
  }

  std::ofstream ofs("readback_mcs.dat", std::ios::binary);
  ofs.write((char*)readback_mcs_data_.data(), readback_mcs_data_.size());
  ofs.close();
  
  if(conf_reg_[kIndexDebug]){
    std::ofstream ofs("readback_mcs.dat", std::ios::binary);
    ofs.write((char*)readback_mcs_data_.data(), readback_mcs_data_.size());
    ofs.close();
    std::cout << "------------------------------------------" << std::endl;
  }
}

// InitializeRegister ______________________________________________
void
FlashMemoryProgrammer::InitializeRegister()
{
  for(int32_t i = 0; i < FMP::kNumRegister; ++i){
    uint32_t multi_byte_offset = i << 16;
    fpga_module_.WriteModule(FMP::kAddrRegister + multi_byte_offset, 0);
  }  
}

// SetDummyClock ___________________________________________________
void
FlashMemoryProgrammer::SendDummyClock()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }
  
  const uint32_t ctrl_reg = FMP::kCtrlDummyMode;
  fpga_module_.WriteModule(FMP::kAddrCtrl, ctrl_reg);

  const SPI::Command command = SPI::g_dummy_command;
  ExecSpiCommand(command, nullptr, func_name);

  fpga_module_.WriteModule(FMP::kAddrCtrl, 0);
}

// ------------------------------------------------------------------
// SPI command
// ------------------------------------------------------------------

// ResetSpiDevice ___________________________________________________
void
FlashMemoryProgrammer::ResetSpiDevice()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(current_device_ == "n25q128a"){
    InstructOneByte(func_name, "reset_enable");
  } 
  InstructOneByte(func_name, "reset");
}

// ReadSpiDeviceID _________________________________________________
uint32_t
FlashMemoryProgrammer::ReadSpiDeviceID()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  // SPI command select
  //  const SPI::Command command = GetCommand("read_device_id", func_name);
  const SPI::Command command = SPI::g_read_device_id;
  if(command.mode == "null") return 0xffffff;

  // Do SPI instruction
  ExecSpiCommand(command, nullptr, func_name);

  // Result
  fpga_module_.ReadModule_nByte(FMP::kAddrReadFifo, command.length_read);
  auto itr     = fpga_module_.GetDataIteratorBegin();
  uint32_t device_id = 0;
  int shift = static_cast<int32_t>(command.length_read)-1;
  for(int32_t i = 0; i<static_cast<int32_t>(command.length_read); ++i, ++itr){
    uint8_t reg = *itr;
    device_id += (reg & 0xff) << 8*(shift-i);
  }
  spi_device_id_ = device_id;
  
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: Result of " << func_name << " is\n";
    std::cout << std::hex << " 0x" << device_id << std::dec << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::endl;
  }

  return device_id;
}

// SetWriteEnable ___________________________________________________
void
FlashMemoryProgrammer::SetWriteEnable()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "write_enable");
}

// SetWriteDisable __________________________________________________
void
FlashMemoryProgrammer::SetWriteDisable()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "write_disable");
}

// ReadWIP __________________________________________________________
uint32_t
FlashMemoryProgrammer::ReadWIP()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  uint32_t status_reg = 0;
  if(false
     || current_device_ == "s25fl256s"
     || current_device_ == "s25fl128s"
     ){
    status_reg = ReadOneByte(func_name, "read_status_register1");
  }

  if(false
     || current_device_ == "n25q128a"
     || current_device_ == "mt25ql512"
     ){
    status_reg = ReadOneByte(func_name, "read_status_register");
  } 

  return (status_reg & 0x01);
}

// ------------------------------------------------------------------
// for mt25ql512 and s25fl256
// ------------------------------------------------------------------
// EnableExAddrMode ________________________________________________
void
FlashMemoryProgrammer::EnableExAddrMode()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  if(false
     || current_device_ == "n25q128a"
     ){
    std::cerr << "#E: " << func_name << "\n"
	      << " This function is for S25FLxxx or MT25QL512\n"
	      << " Current device: " << current_device_
	      << std::endl;
  }else if(current_device_ == "mt25ql512"){
    InstructOneByte(func_name, "enable_4byte_address_mode");
  }else if(false
	   || current_device_ == "s25fl256s"
	   || current_device_ == "s25fl128s"
	   ){
    WriteBankRegister();
  }else{
    std::cerr << "#E: " << func_name << "\n"
      	      << " Unknown device: " << current_device_ << "\n"
	      << " This function is for S25FL256 or MT25QL512"
	      << std::endl;
  }
}

// ------------------------------------------------------------------
// for n25q128a
// ------------------------------------------------------------------

// ReadStatusRegister ______________________________________________
uint32_t
FlashMemoryProgrammer::ReadStatusRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "read_status_register");
}

// ReadFlagStatusRegister __________________________________________
uint32_t
FlashMemoryProgrammer::ReadFlagStatusRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "read_flag_status_register");
}

// ClearFlagStatusRegister _________________________________________
void
FlashMemoryProgrammer::ClearFlagStatusRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "clear_flag_status_register");
}

// ReadVolatileConfRegister ________________________________________
uint32_t
FlashMemoryProgrammer::ReadVolatileConfRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "read_volatile_conf_register");
}

// WriteVolatileConfRegister _______________________________________
void
FlashMemoryProgrammer::WriteVolatileConfRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  const uint8_t dummy_cycle = 0xf0;
  WriteOneByte(func_name, "write_volatile_conf_register", dummy_cycle);
}

// ------------------------------------------------------------------
// for mt25ql512
// ------------------------------------------------------------------

// Enter4ByteAddressMode ___________________________________________
void
FlashMemoryProgrammer::Enter4ByteAddressMode()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "enter_4byte_address_mode");
}

// Exit4ByteAddressMode ____________________________________________
void
FlashMemoryProgrammer::Exit4ByteAddressMode()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "exit_4byte_address_mode");
}

// ------------------------------------------------------------------
// for s25fl256s
// ------------------------------------------------------------------

// ReadBankRegister ________________________________________________
uint32_t
FlashMemoryProgrammer::ReadBankRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "bank_register_read");
}

// WriteBankRegister ________________________________________________
void
FlashMemoryProgrammer::WriteBankRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  const uint8_t enable_extended_address = 0x80;
  WriteOneByte(func_name,"bank_register_write", enable_extended_address);
}

// SetLatencyCode ___________________________________________________
void
FlashMemoryProgrammer::SetLatencyCode()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");

  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  const uint32_t status_reg1 = ReadStatusRegister1();
  const uint32_t conf_reg    = ReadConfigurationRegister();

  // For fast read
  const uint8_t latency_code = 0x80; // Corresponding to 10b.
  uint8_t send_data[2] = {0,0};
  send_data[0] = status_reg1 & 0xff;
  send_data[1] = (conf_reg & 0xff) | latency_code;

  // Write enable
  SetWriteEnable();
  
  // SPI command select
  const SPI::Command command = GetCommand("write_register", func_name);
  if(command.mode == "null") std::exit(-1);

  // Do SPI instruction
  ExecSpiCommand(command, send_data, func_name);

  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::endl;
  }
}

// ReadStatusRegister1 _____________________________________________
uint32_t
FlashMemoryProgrammer::ReadStatusRegister1()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "read_status_register1");
}

// ReadStatusRegister2 _____________________________________________
uint32_t
FlashMemoryProgrammer::ReadStatusRegister2()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "read_status_register2");
}

// ClearStatusRegister ______________________________________________
void
FlashMemoryProgrammer::ClearStatusRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "clear_status_register");
}

// ReadConfigurationRegister _______________________________________
uint32_t
FlashMemoryProgrammer::ReadConfigurationRegister()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  return ReadOneByte(func_name, "read_configuration_register");
}

// -----------------------------------------------------------------
// Private
// -----------------------------------------------------------------

// InstructOneByte ___________________________________________________
void
FlashMemoryProgrammer::InstructOneByte(const std::string func_name,
				       const std::string reg_name )
{
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  // SPI command select
  const SPI::Command command = GetCommand(reg_name, func_name);
  if(command.mode == "null") std::exit(-1);

  // Do SPI instruction
  ExecSpiCommand(command, nullptr, func_name);

  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::endl;
  }

  return;
}

// ReadOneByte _____________________________________________________
uint32_t
FlashMemoryProgrammer::ReadOneByte(const std::string func_name,
				   const std::string reg_name )
{
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  // SPI command select
  const SPI::Command command = GetCommand(reg_name, func_name);
  if(command.mode == "null") return 0xffffffff;

  // Do SPI instruction
  ExecSpiCommand(command, nullptr, func_name);

  // Result
  uint32_t ret = fpga_module_.ReadModule(FMP::kAddrReadFifo, 1);
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: Result of " << func_name << " is\n";
    std::cout << std::hex << " 0x" << ret << std::dec << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::endl;
  }

  return ret;
}

// WriteOneByte _____________________________________________________
void
FlashMemoryProgrammer::WriteOneByte(const std::string func_name,
				    const std::string reg_name,
				    const uint8_t reg )
{
  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "#D: Start " << func_name << std::endl;
  }

  // SPI command select
  const SPI::Command command = GetCommand(reg_name, func_name);
  if(command.mode == "null") std::exit(-1);

  // Do SPI instruction
  ExecSpiCommand(command, &reg, func_name);

  if(conf_reg_[kIndexDebug]){
    std::cout << "------------------------------------------" << std::endl;
    std::cout << std::endl;
  }
}

// BulkErase ________________________________________________________
void
FlashMemoryProgrammer::BulkErase()
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");
  InstructOneByte(func_name, "bulk_erase");
}

// SetPageReadMode _________________________________________________
void
FlashMemoryProgrammer::SetPageReadMode(const uint32_t page_length)
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");

  // SPI command select
  const SPI::Command command = GetCommand("read", func_name);
  if(command.mode == "null") std::exit(-1);

  uint32_t reg_ctrl = fpga_module_.ReadModule(FMP::kAddrCtrl, 1);
  reg_ctrl = (reg_ctrl & 0xfc) | FMP::kCtrlReadMode;
  fpga_module_.WriteModule(FMP::kAddrCtrl, reg_ctrl, 1);

  // Set instruction for this page
  fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexInst,
  			   command.instruction);

  fpga_module_.WriteModule(FMP::kAddrInstLength,
   			   command.length_inst, 1);
  fpga_module_.WriteModule(FMP::kAddrReadLength,
   			   page_length, 2);
}

// SetFastPageReadMode _____________________________________________
void
FlashMemoryProgrammer::SetFastPageReadMode(const uint32_t page_length)
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");

  // SPI command select
  const SPI::Command command = GetCommand("fast_read", func_name);
  if(command.mode == "null") std::exit(-1);

  uint32_t reg_ctrl = fpga_module_.ReadModule(FMP::kAddrCtrl, 1);
  reg_ctrl = (reg_ctrl & 0xfc) | FMP::kCtrlReadMode;
  fpga_module_.WriteModule(FMP::kAddrCtrl, reg_ctrl, 1);

  // Set instruction for this page
  fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexInst,
  			   command.instruction);
  fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexDummy,
			   0);

  fpga_module_.WriteModule(FMP::kAddrInstLength,
   			   command.length_inst, 1);
  fpga_module_.WriteModule(FMP::kAddrReadLength,
   			   page_length, 2);
}

// DoPageRead ______________________________________________________
void
FlashMemoryProgrammer::DoPageRead(const uint32_t page_address,
				  const uint32_t page_length,
				  uint8_t*       page_data
				  )
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");

  // Set instruction for this page
  if(false
     || current_device_ == "n25q128a"
     ){
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr1,
			     (page_address >> 0) & 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr2,
			     (page_address >> 8)& 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr3,
			     (page_address >> 16)& 0xff);
  }else{
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr0,
			     (page_address >> 0) & 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr1,
			     (page_address >> 8) & 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr2,
			     (page_address >> 16)& 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr3,
			     (page_address >> 24)& 0xff);
  }

  // Execute
  fpga_module_.WriteModule(FMP::kAddrExecute, 1);

  const uint32_t read_length = 128; // byte
  const uint32_t num_loop    = page_length/read_length;
  for(uint32_t i = 0; i<num_loop; ++i){
    fpga_module_.ReadModule_nByte(FMP::kAddrReadFifo, read_length);
    auto itr_begin = fpga_module_.GetDataIteratorBegin();
    auto itr_end   = fpga_module_.GetDataIteratorEnd();
    std::copy(itr_begin, itr_end, page_data + read_length*i);    
  }

}

// DoPageProgram ___________________________________________________
void
FlashMemoryProgrammer::DoPageProgram(const uint32_t page_address,
				     const uint32_t page_length,
				     const uint8_t* page_data
				     )
{
  static const std::string func_name("["+class_name+"::"+__func__+"()");

  // SPI command select
  const SPI::Command command = GetCommand("page_program", func_name);
  if(command.mode == "null") std::exit(-1);

  uint32_t reg_ctrl = fpga_module_.ReadModule(FMP::kAddrCtrl, 1);
  reg_ctrl = (reg_ctrl & 0xfc) | FMP::kCtrlWriteMode;
  fpga_module_.WriteModule(FMP::kAddrCtrl, reg_ctrl, 1);

  // Set instruction for this page
  fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexInst,
  			   command.instruction);

  if(false
     || current_device_ == "n25q128a"
     ){
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr1,
			     (page_address >> 0) & 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr2,
			     (page_address >> 8)& 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr3,
			     (page_address >> 16)& 0xff);
  }else{
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr0,
			     (page_address >> 0) & 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr1,
			     (page_address >> 8) & 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr2,
			     (page_address >> 16)& 0xff);
    fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexAddr3,
			     (page_address >> 24)& 0xff);
  }

  fpga_module_.WriteModule(FMP::kAddrInstLength,
   			   command.length_inst, 1);
  fpga_module_.WriteModule(FMP::kAddrWriteLength,
   			   page_length, 2);

  
  const uint32_t write_length = 128; // byte
  const uint32_t num_loop     = page_length/write_length;
  for(uint32_t i = 0; i<num_loop; ++i){
    fpga_module_.WriteModule_nByte(FMP::kAddrWriteFifo,
				   page_data + i*write_length,
				   write_length);
  }

  // Execute
  fpga_module_.WriteModule(FMP::kAddrExecute, 1);
  usleep(1000);
}

// ExecSpiCommand __________________________________________________
void
FlashMemoryProgrammer::ExecSpiCommand(const SPI::Command command,
				      const uint8_t*     send_data,
				      const std::string  func_name
				      )
{
  // Change SPI command mode
  uint32_t reg_ctrl = fpga_module_.ReadModule(FMP::kAddrCtrl, 1);
  if(command.mode =="read"){
    reg_ctrl = (reg_ctrl & 0xfc) | FMP::kCtrlReadMode;
  }else if(command.mode =="write"){
    reg_ctrl = (reg_ctrl & 0xfc) | FMP::kCtrlWriteMode;
  }else{
    reg_ctrl = (reg_ctrl & 0xfc) | FMP::kCtrlInstMode;
  }                      
  fpga_module_.WriteModule(FMP::kAddrCtrl, reg_ctrl, 1);

  // Set SPI instruction
  if(conf_reg_[kIndexDebug]){
    std::cout << "#D:" << func_name << " Set instruction"
	      << std::endl;    
  }

  fpga_module_.WriteModule(FMP::kAddrRegister +FMP::kIndexInst,
  			   command.instruction);


  // Set SPI command length
  if(conf_reg_[kIndexDebug]){
      std::cout << "#D:" << func_name << " Set length"
	      << std::endl;    
  }

  fpga_module_.WriteModule(FMP::kAddrInstLength,
   			   command.length_inst, 1);

  if(command.mode =="read"){
    fpga_module_.WriteModule(FMP::kAddrReadLength,
			     command.length_read, 2);
  }else if(command.mode =="write"){
    fpga_module_.WriteModule(FMP::kAddrWriteLength,
			     command.length_write, 2);
  }

  // Set write data
  if(send_data != nullptr && command.length_write){
    if(conf_reg_[kIndexDebug]){
      std::cout << "#D:" << func_name << " Set write data"
		<< std::endl;    
    }

    fpga_module_.WriteModule_nByte(FMP::kAddrWriteFifo,
				   send_data,
				   command.length_write);
  }

  fpga_module_.WriteModule(FMP::kAddrExecute, 1);
}


// GetCommand ______________________________________________________
const SPI::Command
FlashMemoryProgrammer::GetCommand(const std::string inst_name,
				  const std::string func_name
				  )
{
  if(!conf_reg_[kIndexDevice]){
    std::cout << "#E: " << func_name
	      << " Memory device is not selected."
	      << std::endl;
    return SPI::g_null_command;
  }

  const auto itr_tuple = SPI::g_spi.find(current_device_);
  if(itr_tuple == SPI::g_spi.end()){
    std::cout << "#E: " << func_name 
	      << " No such the device (" << current_device_ << ")"
	      << std::endl;
    return SPI::g_null_command;
  }

  const auto& map_cmd = std::get<SPI::kCommand>(itr_tuple->second);
  const auto  itr_cmd = map_cmd.find(inst_name);

  if(itr_cmd == map_cmd.end()){
    std::cout << "#E: " << func_name 
	      << " No such command (" << inst_name << ")"
	      << std::endl;
    return SPI::g_null_command;
  }

  if(conf_reg_[kIndexDebug]){
    std::cout << "#D: " << func_name << "\n"
	      << " Mode:      " << (itr_cmd->second).mode << "\n"
	      << std::hex
      	      << " Inst:      0x" << (itr_cmd->second).instruction << "\n"
	      << std::dec
    	      << " Inst Len:  " << (itr_cmd->second).length_inst << "\n"
    	      << " Read Len:  " << (itr_cmd->second).length_read << "\n"
	      << " Write Len: " << (itr_cmd->second).length_write<< "\n"
	      << std::endl;
  }
  
  return itr_cmd->second;
}
};
