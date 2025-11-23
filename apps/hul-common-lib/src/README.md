# hul-common-lib
A common library for HUL based front-end electronics.
AMANEQ, CIRASAME, and RAYRAW software require this library.

## Tested system
Requires C++17.
| System | Version | Compiler                 | CMake                          |
| ---    | ---     | ---                      | ---                            | 
| AlmaLinux | 9       | GCC 11.3.1 (Require > 7.1) | 3.20.2 (Require > 3.13) |

## For developer
Forking-workflow is dopted. Please fork the official repository in spadi-alliance to your accout and make a pull-request to merge your work.
**Do not push your contribution to the official directly.**

## [INSTALLATION](INSTALL.md) for Users

## List of avairable executables for each FEEs

These two executables can read and write the values specified address.
- read_register
- write_register

### AMANEQ

- SiTCP
  - erase_eeprom
  - set_ctrl_eeprom
  - set_sitcpreg
- SPI flash memory
  - flash_memory_programmer
  - mcs_converter
  - verify_mcs
- Jitter cleaner
  - set_cdce62002
- FW internal functions
  - gen_user_reset
  - get_version
  - inject_sem_error
  - read_sem
  - read_xadc
  - reconfig_fpga
  - reset_sem
  - read_scr

### CIRASAME

- SiTCP
  - erase_eeprom
  - set_ctrl_eeprom
  - set_sitcpreg
- SPI flash memory
  - flash_memory_programmer
  - mcs_converter
  - verify_mcs
- Jitter cleaner
  - set_cdce62002
- APD bias
  - set_max1932
- FW internal functions
  - gen_user_reset
  - get_version
  - inject_sem_error
  - read_sem
  - read_xadc
  - reconfig_fpga
  - reset_sem
  - read_scr
 
### RAYRAW

- SiTCP
  - erase_eeprom
  - set_ctrl_eeprom
  - set_sitcpreg
- SPI flash memory
  - flash_memory_programmer
  - mcs_converter
  - verify_mcs
- Jitter cleaner
  - set_cdce62002
- APD bias
  - set_max1932
- FW internal functions
  - gen_user_reset
  - get_version
  - inject_sem_error
  - read_sem
  - read_xadc
  - reconfig_fpga
  - reset_sem
  - read_scr
