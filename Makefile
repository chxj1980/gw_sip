
DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

LIVEDIR = /work/compile/arm/live
SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 
CPPFLAGS = -Wno-write-strings -I${DIR_INC} \
		   -I/root/Desktop/davinci/dm3730/guowang/arm_compile/yyhonviflib/include \
		   -I/work/compile/arm/live/BasicUsageEnvironment/include \
		   -I/work/compile/arm/live/groupsock/include \
		   -I/work/compile/arm/live/liveMedia/include \
		   -I/work/compile/arm/live/UsageEnvironment/include \
		   -I/usr/local/include/sqlite \
		   -I/root/Desktop/davinci/dm3730/guowang/arm_compile/libeXosip2-3.5.0/include\
		   -I/root/Desktop/davinci/dm3730/guowang/arm_compile/libosip2-3.5.0/include \
		   -I/root/Desktop/davinci/dm3730/guowang/arm_compile/libeXosip2-3.5.0/src\
		   -I/usr/local/include/libxml2\
		   -I/usr/local/include/jrtplib3\
		   -I/usr/local/include
CPPLDFLAGS =-L/usr/lib -lyyhonvif -L/usr/local/lib -luuid -lssl -lcrypto -lz -lpthread -lsqlite \
            -losip2  -losipparser2  -lxml2 -leXosip2 -ljthread -ljrtp \
			-L$(LIVEDIR)/liveMedia -lliveMedia \
			-L$(LIVEDIR)/BasicUsageEnvironment -lBasicUsageEnvironment \
			-L$(LIVEDIR)/groupsock -lgroupsock \
			-L$(LIVEDIR)/UsageEnvironment -lUsageEnvironment \

TARGET = gw_sip.mod
BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = arm-arago-linux-gnueabi-g++
${BIN_TARGET}:${OBJ}
	$(CC) ${OBJ} $(CPPLDFLAGS) -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CPPFLAGS) -c $^ -o $@

.PHONY:clean
clean:
	find ${DIR_OBJ} -name *.o -exec rm -rf {} \;
	find ${DIR_BIN} -name *.mod -exec rm -rf {} \;

