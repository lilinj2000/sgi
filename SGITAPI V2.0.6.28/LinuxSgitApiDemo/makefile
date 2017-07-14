#
#
#
.SUFFIXES:
.SUFFIXES: .cpp .o
#
#
PROGRAME=SgitApiDemo
#
BASE_DIR=$(HOME)
#
INC_DIR= -I./  
LIB_DIR= -L./
#
OBJS=SgitApiDemo.o
#
LIBS= -lsgitquotapi -lsgittradeapi  -ldl -lpthread
#
CPPDEFS= -O2 -g -fPIC -DPIC -DLINUX -D_DEBUG -Wall 

all: $(OBJS)
	g++ -m64 -o $(PROGRAME) $(LIB_DIR) $(LIBS) $(OBJS) 

.cpp.o:
	g++ -c  -m64 -s $(CPPDEFS) $(INC_DIR) $< 

clean:
	rm *.o
	rm -f $(SGIT_LIB)$(PROGRAME)
