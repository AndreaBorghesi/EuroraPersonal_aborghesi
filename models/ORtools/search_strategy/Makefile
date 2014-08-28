# Copyright 2014 <andrea.borghesi3@unibo.it>
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#-----------------------------------------------------------------------------
# Main configuration variables
#-----------------------------------------------------------------------------

# Base folder for the main libraries; change to match the values on your system
OR_TOOLS_BASE := /media/sda4/ORtools/or-tools-read-only

# ----------------------------------------------------------------------------
# CHANGE THE FOLLOWING PATHS TO MATCH YOUR MACHINE SETTINGS
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Makefile variables setup
# ----------------------------------------------------------------------------

DEPENDENCIES_INC := $(OR_TOOLS_BASE)/dependencies/install/include

# Important folders for the project
SRC_FOLDER := src
INC_FOLDER := include
OUT_FOLDER := out

# Compiler and linker
CCC := clang++
LD := ld

# Compiler flags
CFLAGS = -fPIC
# From or-tools compile log
# use latest c++ standard (required for or-tools)
CFLAGS += -std=c++0x
CFLAGS += -DARCH_K8
# enable this during debug
CFLAGS += -O0 -g
# enable this during the final deployment
#CFLAGS += -O3
#CFLAGS += -DNDEBUG
# disable some deprecation warnings
CFLAGS += -Idependencies/sources/TinyThread++-1.1/source
CFLAGS += -Wno-deprecated
CFLAGS += -DUSE_CBC
CFLAGS += -DUSE_CLP
# add search path for current project
CFLAGS += -Iinclude
# add search path for or-tools
CFLAGS += -I$(OR_TOOLS_BASE)/src
# add search path for gflags
CFLAGS += -I$(DEPENDENCIES_INC)
CFLAGS += -I$(OR_TOOLS_BASE)/src/gen

# Linker flags
# add constraint solver libraries
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -lconstraint_solver
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -lsat 
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -lalgorithms 
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -llinear_solver
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -lutil
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -lbase
# add or-tools dependencies
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/dependencies/install/lib/
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/dependencies/install/lib
LDFLAGS += -lCbcSolver -lCbc -lCgl -lOsi -lOsiCbc -lOsiClp 
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/dependencies/install/lib
LDFLAGS += -lClp -lCoinUtils
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/dependencies/install/lib
LDFLAGS += -lgflags -lz
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/dependencies/install/lib
LDFLAGS += -lprotobuf -lpthread -lrt -lpthread
LDFLAGS += -L$(PYNC_FOLDER)
LDFLAGS += -Wl,-rpath $(PYNC_FOLDER)
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/src/constraint_solver

LDFLAGS_LIB := $(LDFLAGS) 

# ------------------------------------------------------------------------------
# Main commands
# ------------------------------------------------------------------------------

all: search_strategy 
	
.PHONY: all search_strategy clean

# ------------------------------------------------------------------------------
# Rules to build the core constraint files
# ------------------------------------------------------------------------------

$(OUT_FOLDER)/alternative_cst.o: $(SRC_FOLDER)/alternative_cst.cc $(INC_FOLDER)/alternative_cst.h
	$(CCC) $(CFLAGS) -o $(OUT_FOLDER)/alternative_cst.o -c $(SRC_FOLDER)/alternative_cst.cc

$(OUT_FOLDER)/main.o: $(SRC_FOLDER)/main.cc
	$(CCC) $(CFLAGS) -o $(OUT_FOLDER)/main.o -c $(SRC_FOLDER)/main.cc

search_strategy: $(OUT_FOLDER)/alternative_cst.o $(OUT_FOLDER)/main.o
	$(CCC) -o search_strategy $(OUT_FOLDER)/main.o $(OUT_FOLDER)/alternative_cst.o $(LDFLAGS)
	
# ------------------------------------------------------------------------------
# Phase 4: a rule to clean everything
# ------------------------------------------------------------------------------

clean:
	rm -f $(OUT_FOLDER)/*
	rm -f search_strategy

