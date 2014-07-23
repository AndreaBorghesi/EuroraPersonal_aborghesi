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

# header file for the current Python installation (this is only needed to
# buid the SWIG wrapper)
PYTHON_INC := -I/usr/include/python2.7 -I/usr/lib/python2.7

# ----------------------------------------------------------------------------
# CHANGE THE FOLLOWING PATHS TO MATCH YOUR MACHINE SETTINGS
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Makefile variables setup
# ----------------------------------------------------------------------------

# Header folders for some google libraries
# GFLAGS_INC := $(OR_TOOLS_BASE)/dependencies/sources/gflags/src
# PROTOBUF_BASE := /Users/lompa//Documents/usr/or-tools/protobuf-2.3.0

DEPENDENCIES_INC := $(OR_TOOLS_BASE)/dependencies/install/include

# Important folders for the project
SRC_FOLDER := src
INC_FOLDER := include
OUT_FOLDER := out
# ABSOLUTE path for the libraries and python module
PYNC_FOLDER := $(CURDIR)/pync

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

# Linker flags
# add constraint solver libraries
LDFLAGS := -fPIC -std=c++0x -O4 -DNDEBUG
LDFLAGS += -DARCH_K8 -Idependencies/sources/TinyThread++-1.1/source -Wno-deprecated
LDFLAGS += -Wl,-rpath $(OR_TOOLS_BASE)/lib -L$(OR_TOOLS_BASE)/lib
LDFLAGS += -DUSE_CBC -DUSE_CLP 
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

# ld flags for libraries (most of this stuff is OSX specific)
#LDFLAGS_LIB := -arch x86_64 -flat_namespace -undefined suppress
##LDFLAGS_LIB += -macosx_version_min 10.6 -lSystem -compatibility_version 1.0
#LDFLAGS_LIB += -current_version 1.0.0
#LDFLAGS_LIB += $(LDFLAGS) 

# path of the SWIG binary
SWIG_BINARY := $(OR_TOOLS_BASE)/dependencies/install/bin/swig

# ------------------------------------------------------------------------------
# Main commands
# ------------------------------------------------------------------------------

all: alternative_cst_test pywrapcp_acst
	
.PHONY: all alternative_cst_test pywrapcp_acst clean

# ------------------------------------------------------------------------------
# Rules to build the core constraint files
# ------------------------------------------------------------------------------

$(OUT_FOLDER)/alternative_cst.o: $(SRC_FOLDER)/alternative_cst.cc $(INC_FOLDER)/alternative_cst.h
	$(CCC) $(CFLAGS) -o $(OUT_FOLDER)/alternative_cst.o -c $(SRC_FOLDER)/alternative_cst.cc

$(OUT_FOLDER)/main.o: $(SRC_FOLDER)/main.cc
	$(CCC) $(CFLAGS) -o $(OUT_FOLDER)/main.o -c $(SRC_FOLDER)/main.cc

alternative_cst_test: $(OUT_FOLDER)/alternative_cst.o $(OUT_FOLDER)/main.o
	$(CCC) -o alternative_cst_test $(OUT_FOLDER)/main.o $(OUT_FOLDER)/alternative_cst.o $(LDFLAGS)
	
# shared object
alternative_cst_so: $(OUT_FOLDER)/alternative_cst.o
	$(CCC) -shared $(LDFLAGS_LIB) -o $(PYNC_FOLDER)/libalternative_cst.so $(OUT_FOLDER)/alternative_cst.o
	
# ------------------------------------------------------------------------------
# Phase 3: build a python wrapper using SWIG
# ------------------------------------------------------------------------------

pywrapcp_acst: alternative_cst_so _pywrapcp_acst.so

$(PYNC_FOLDER)/pywrapcp_acst.py: $(INC_FOLDER)/pywrapcp_acst.swig $(INC_FOLDER)/alternative_cst.h
	$(SWIG_BINARY) -I$(OR_TOOLS_BASE)/src -c++ -python -o $(PYNC_FOLDER)/pywrapcp_acst.cc -module pywrapcp_acst $(INC_FOLDER)/pywrapcp_acst.swig

$(PYNC_FOLDER)/pywrapcp_acst.cc: $(PYNC_FOLDER)/pywrapcp_acst.py

$(PYNC_FOLDER)/pywrapcp_acst.o: $(PYNC_FOLDER)/pywrapcp_acst.cc
	$(CCC) $(CFLAGS) $(PYTHON_INC) -c $(PYNC_FOLDER)/pywrapcp_acst.cc -o $(PYNC_FOLDER)/pywrapcp_acst.o

_pywrapcp_acst.so: $(PYNC_FOLDER)/pywrapcp_acst.o $(OUT_FOLDER)/alternative_cst.o
	$(LD) -bundle $(LDFLAGS_LIB) -lalternative_cst -o _pywrapcp_acst.so $(PYNC_FOLDER)/pywrapcp_acst.o
	mv $(PYNC_FOLDER)/pywrapcp_acst.py .


# ------------------------------------------------------------------------------
# Phase 4: a rule to clean everything
# ------------------------------------------------------------------------------

clean:
	rm -f $(OUT_FOLDER)/*
	rm -f alternative_cst_test
	rm -f _pywrapcp_acst.so
	rm -f pywrapcp_acst.py
	rm -f pywrapcp_acst.pyc
	rm -f $(PYNC_FOLDER)/*

