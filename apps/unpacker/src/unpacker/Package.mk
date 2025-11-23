#!/usr/bin/make

# Author: Tomonori Takahashi

package_name       := unpacker

#_______________________________________________________________________________
# ordinary source-files (package/src/%.cc), 
#          header-files (package/include/%.hh), 
#          object-files (package/src/*.o)
local_sources      :=  $(filter %.cc,$(wildcard $(package_name)/src/*))
local_headers      :=  $(filter %.hh,$(wildcard $(package_name)/include/*))
local_objects      :=  $(subst .cc,.o,$(local_sources))

#_______________________________________________________________________________
# library files (lib/lib%.so)
local_libraries    := $(addprefix \
                        $(lib_dir)/lib,\
			$(notdir \
			  $(subst .cc,.so,\
                              $(local_sources))))

#_______________________________________________________________________________
# add local files to global files 
sources              += $(local_sources)
headers              += $(subst $(package_name)/include,\
	                 $(include_dir),$(local_headers))
package_headers      += $(local_headers)
objects              += $(local_objects)
libraries            += $(local_libraries)

