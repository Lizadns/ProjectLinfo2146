CONTIKI_PROJECT = sub_gateway
all: $(CONTIKI_PROJECT)

VPATH += ../net
PROJECT_SOURCEFILES += network_greenhouse.c

CONTIKI = ../..

MAKE_NET = MAKE_NET_NULLNET

CFLAGS += -I../net

include $(CONTIKI)/Makefile.include
