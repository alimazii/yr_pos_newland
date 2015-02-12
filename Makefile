#file searching path

CROSS_COMPILE=arm-unknown-linux-gnu-
#CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar
RM=rm
CP=cp
MAKENLD=makenld

# 应用程序目录定义
BINDIR= 	bin
SRCDIR= 	src
INCDIR= 	inc
OBJDIR=		obj_err
LIBDIR=		lib
LIBAPIDIR=		libapi

#API目录定义
APIDIR= 	biosapi

#转换时间
POSDATE = $(subst /, , $(shell date '+%x'))
#取年
POSYEAR = $(word 3, $(POSDATE))
#取日
POSDAY = $(word 2, $(POSDATE))
#取月
POSMONTH = $(word 1, $(POSDATE))
#应用标识）
TAG=AA
#应用行业名称	（继承于该版本的非银联版本需要修改为相应银行名称!!）
BANK=DMNB
#应用编号		（00开始递增）
ifeq ($(TMSFUNC),USETMS)
APPNO = 50
else
APPNO = 00
endif

#版本序号			（每次定版，版本都要递增!!!!）
VERNO = 01

#内部版本号为编译日期（内部版本号定义，格式为:4位银行名称+1位应用类别+1位银行版本+YYYYMMDD+2位序号）
INTERVER = $(join "\"$(TAG)$(BANK)$(APPNO), $(POSYEAR)$(POSMONTH)$(POSDAY)$(VERNO)\"")

CFLAGS += -Wall -DNDEBUG -DRECEIPT_CONF $(CROSS_CFLAGS) -O2 $(INCPATH)



#文件搜索路径
VPATH =src $(OBJDIR)

#头文件搜索路径
INCLPATH =  -I$(INCDIR) -I$(APIDIR)

# 程序链接参数
LDFLAGS += -L$(APIDIR) -lndk -L$(LIBDIR) -lcurl -lexpat -lrt -pthread

# 生成的程序名
NAME = main
APPNAME = AADMNB00.NDK.NLD
# 程序中用到的模块
SRCS=			$(wildcard $(SRCDIR)/*.c)
SRSS=			$(notdir $(SRCS))
OBJS=			$(subst .c,.o,$(SRSS))

OBJSD=		$(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS))
OBJSY=		$(subst .c,.o,$(OBJSD))


#包含依赖文件
all: NLD

$(NAME):config $(OBJS)
#	-$(RM) $(BINDIR)/$(NAME)
	$(CC) $(CFLAGS) -o $(BINDIR)/$(NAME) $(OBJSY) $(LDFLAGS)
	$ sed -i '/^strVerBuf/c\\strVerBuf=$(APPVER)' $(BINDIR)/headerinfo.ini 
	$ sed -i '/^strBuildTime/c\\strBuildTime=$(POSYEAR)$(POSMONTH)$(POSDAY)' $(BINDIR)/headerinfo.ini 
	unix2dos $(BINDIR)/headerinfo.ini 
%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLPATH) $< -o $(OBJDIR)/$@ >& $(OBJDIR)/$(notdir $(basename $<)).err
#自动生成依赖文件
config: $(subst .o,.deps, $(OBJS))

%.deps: %.c
	$(CC) -MM $(INCLPATH) $< >$(OBJDIR)/$@ 

.PHONY:clean
clean:
	-$(RM) $(BINDIR)/$(NAME)
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(OBJDIR)/*.deps
	
NLD:$(NAME)
	$(MAKENLD) -h HeaderInfo.ini -f main pri_* config.txt print.bmp -o $(APPNAME) -C $(BINDIR)/ 
