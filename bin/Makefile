TARGET_ARCH = linux64
# TARGET_ARCH = macosx64

CC     = g++
OPT    = -O0
DEBUG  = -g
OTHER  = # -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER) # $(DEBUG)

VPATH = ../src:.
MODULE = NeuronNoxim 
SRCS =  NoximNoC.cpp NoximRouter.cpp NoximProcessingElement.cpp NoximBuffer.cpp \
		NoximStats.cpp NoximGlobalStats.cpp NoximGlobalRoutingTable.cpp \
		NoximLocalRoutingTable.cpp NoximGlobalTrafficTable.cpp NoximReservationTable.cpp \
		NoximPower.cpp NoximCmdLineParser.cpp NoximMain.cpp \
		Neuron.cpp NeuronArbiter.cpp NeuronEncoder.cpp \
		NeuronDecoder.cpp NeuronPE.cpp Neuron_config_parser.cpp
OBJS = $(SRCS:.cpp=.o)

include ./Makefile.defs
