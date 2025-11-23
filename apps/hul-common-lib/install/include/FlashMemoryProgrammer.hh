#ifndef HUL_FLASH_MEMORY_PROGRAMMER_HH
#define HUL_FLASH_MEMORY_PROGRAMMER_HH

#include<vector>
#include<string>
#include<bitset>
#include<stdint.h>

namespace SPI{
  class Command;
};

namespace HUL{
class FPGAModule;

class FlashMemoryProgrammer
{
 public:
  using BinaryData = std::vector<uint8_t>;

  FlashMemoryProgrammer() = delete;
  FlashMemoryProgrammer(const FlashMemoryProgrammer&) = delete;
  FlashMemoryProgrammer(FPGAModule& fModule);
  virtual ~FlashMemoryProgrammer();
  FlashMemoryProgrammer& operator=(const FlashMemoryProgrammer&) = delete;

  //-------------------------------------------------------
  // Read device ID from flash memory.
  std::string CheckSpiDevice();
  // Give SPI flash memory name manually.
  void        SetSpiDevice(const std::string device_name);
  // Enable/Disable debug mode of this class
  void        EnableDebugMode()  {conf_reg_.set(kIndexDebug);};
  void        DisableDebugMode() {conf_reg_.reset(kIndexDebug);};
  // Execute bulk erase
  void        EraseFlashMemory();
  // Program MCS data to flash memory
  void        ProgramFlashMemory();
  // Readback MCS data from flash memory.
  void        VerifyMCS();
  // Load local MCS file.
  void        ReadMCSFile(const std::string file_name);
  // Load local binary MCS file (modified mcs file).
  void        ReadBinaryMCSFile(const std::string file_name);
  // Save modified (binary) mcs file
  void        SaveBinaryMCSFile(const std::string file_name);

  // HUL command-------------------------------------------
  void        InitializeRegister();
  void        SendDummyClock();

  // SPI command-------------------------------------------
  // Common command
  void        ResetSpiDevice();
  uint32_t    ReadSpiDeviceID();
  void        SetWriteEnable();
  void        SetWriteDisable();
  uint32_t    ReadWIP();

  // for s25fl and mt25ql
  void        EnableExAddrMode();
  
  // for n25q128a and mt25ql512
  uint32_t    ReadStatusRegister(); 
  uint32_t    ReadFlagStatusRegister();
  void        ClearFlagStatusRegister();
  uint32_t    ReadVolatileConfRegister(); 
  void        WriteVolatileConfRegister();

  // for mt25ql512
  void        Enter4ByteAddressMode();
  void        Exit4ByteAddressMode();
  
  // for s25fl256s
  uint32_t    ReadBankRegister();
  void        WriteBankRegister();
  void        SetLatencyCode();
  
  uint32_t    ReadStatusRegister1();
  uint32_t    ReadStatusRegister2();
  void        ClearStatusRegister();
  uint32_t    ReadConfigurationRegister();

 private:
  enum IndexConfRegister
    {
      kIndexDebug, kIndexMcsReady, kIndexDevice,
      sizeConfReg
    };

  std::string               current_device_ {""};
  std::bitset<sizeConfReg>  conf_reg_ ;
  FPGAModule&               fpga_module_;
  BinaryData                mcs_data_binary_;
  BinaryData                readback_mcs_data_;
  uint32_t                  spi_device_id_  {0};
  std::string               mcs_file_path_  {""};
  std::string               binary_mcs_file_path_  {""};

  // SPI command -------------------------------------------
  // Common command
  void     InstructOneByte(const std::string func_name,
			   const std::string reg_name );
  uint32_t ReadOneByte(const std::string func_name,
		       const std::string reg_name );
  void     WriteOneByte(const std::string func_name,
			const std::string reg_name,
			const uint8_t reg
			);
  
  void     BulkErase();
  void     SetPageReadMode(const uint32_t page_langeth);
  void     SetFastPageReadMode(const uint32_t page_langeth);
  void     DoPageRead(const uint32_t page_address,
		      const uint32_t page_langeth,
		      uint8_t*       page_data
		      );
           
  void     DoPageProgram(const uint32_t page_address,
			 const uint32_t page_langeth,
			 const uint8_t* page_data);
           
           
  void     ExecSpiCommand(const SPI::Command cmd,
			  const uint8_t*     send_data,
			  const std::string  arg
			  );
  
  const SPI::Command GetCommand(const std::string inst_name,
				const std::string func_name
				);
  
};
};

#endif
