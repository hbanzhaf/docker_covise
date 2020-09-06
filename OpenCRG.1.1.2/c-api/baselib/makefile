# Makefile for OpenCRG project
#
#    Copyright 2013 VIRES Simulationstechnologie GmbH
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

#library version
REVISION = 1.1.2

#directories
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc

#Compiler
COMP = gcc

#Compiler options
# CFLGS = -Wall -ggdb -ansi -I$(INC_DIR)	#all Warnings with debugging
CFLGS = -Wall -O3 -ansi -fPIC -I$(INC_DIR)      #all Warnings with level 3 optimizations

#Compiler call
CC = $(COMP) $(CFLGS)

#SOURCE FILES
SOURCES = \
	crgMgr.c \
	crgMsg.c \
	crgStatistics.c \
	crgContactPoint.c \
	crgEvalxy2uv.c \
	crgEvaluv2xy.c \
	crgEvalz.c \
	crgEvalpk.c \
        crgLoader.c \
        crgOptionMgmt.c \
        crgPortability.c

#EXTERNAL OBJECT FILES
OBJECTS = $(SOURCES:.c=.o)

#Make
lib : all archive
    
all : $(OBJECTS)

archive :
	rm -f $(LIB_DIR)/libOpenCRG*.a
	ar -r $(LIB_DIR)/libOpenCRG.$(REVISION).a obj/*.o
	cd $(LIB_DIR); ln -s libOpenCRG.$(REVISION).a libOpenCRG.a
    
clean :
	rm -f $(OBJ_DIR)/*.o
	rm -f $(LIB_DIR)/libOpenCRG*.a

%.o:	$(SRC_DIR)/%.c
	$(CC) -c $? -o $(OBJ_DIR)/$@

#*** FILE DEPENCIES : WHERE TO FIND FILES
.PATH: $(SRC_DIR) $(INC_DIR)


