#include <iostream>
#include <cstdlib>

#include "rayraw_const.hh"
#include "SPIConfigLoader.hh"

int main(int argc, char* argv[])
{

  SPIConfigLoader& g_conf = SPIConfigLoader::get_instance();
  g_conf.make_RegisterValues();
  
  return 0;
}
