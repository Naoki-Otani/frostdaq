BIN_DIR		= ../bin
CXXFLAGS 	+= -Wall -g -MMD -std=c++0x
LDFLAGS		+=

SRCALL 		= $(wildcard *.cc)
DEPENDS		= $(SRCALL:.cc=.d)

SRC		= $(filter-out %main.cc, $(SRCALL))
OBJ		= $(SRC:.cc=.o)

TARGET 		= rayraw_control

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/rayraw_control: rayraw_control_main.o $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $< $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(BIN_DIR)/* *.o *.d *~

echo 		= /bin/echo -e
show:
	@ $(echo) "CXXFLAGS = $(CXXFLAGS)"
	@ $(echo) "LDFLAGS  = $(LDFLAGS)"
	@ $(echo) "SRCALL   = $(SRCALL)"
	@ $(echo) "DEPENDS  = $(DEPENDS)"
	@ $(echo) "SRC      = $(SRC)"
	@ $(echo) "OBJ      = $(OBJ)"

-include $(DEPENDS)
