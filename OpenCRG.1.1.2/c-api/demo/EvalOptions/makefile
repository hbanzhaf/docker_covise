#Makefile for OpenCRG project
#
#    Copyright 2009 VIRES Simulationstechnologie GmbH
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

#directories
LIB_INC_DIR = ../../baselib/inc
LIB_DIR     = ../../baselib/lib
SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = inc
BIN_TGT     =../bin/crgEvalOpts

#Compiler
COMP = gcc

#Compiler options
CFLGS = -Wall -ggdb -ansi -I$(LIB_INC_DIR) -I$(INC_DIR)	#all Warnings with debugging

#linker options
LFLGS = -L$(LIB_DIR) -lOpenCRG -lm

#Compiler call
CC = $(COMP)

#SOURCE FILES
SOURCES = \
	main.c

#EXTERNAL OBJECT FILES
OBJECTS = $(SOURCES:.c=.o)

#Make
all : $(OBJECTS)
	$(CC) $(OBJ_DIR)/$(OBJECTS) $(LFLGS) -o $(BIN_TGT)
    
clean :
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_TGT)

%.o:	$(SRC_DIR)/%.c
	$(CC) $(CFLGS) -c $? -o $(OBJ_DIR)/$@

#*** FILE DEPENCIES : WHERE TO FIND FILES
.PATH: $(SRC_DIR)


