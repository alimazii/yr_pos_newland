#file searching path

CROSS_COMPILE=arm-unknown-linux-gnu-
#CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar
RM=rm
CP=cp
MAKENLD=makenld

# Ӧ�ó���Ŀ¼����
BINDIR= 	bin
SRCDIR= 	src
INCDIR= 	inc
OBJDIR=		obj_err
LIBDIR=		lib
LIBAPIDIR=		libapi

#APIĿ¼����
APIDIR= 	biosapi

#ת��ʱ��
POSDATE = $(subst /, , $(shell date '+%x'))
#ȡ��
POSYEAR = $(word 3, $(POSDATE))
#ȡ��
POSDAY = $(word 2, $(POSDATE))
#ȡ��
POSMONTH = $(word 1, $(POSDATE))
#Ӧ�ñ�ʶ��
TAG=AA
#Ӧ����ҵ����	���̳��ڸð汾�ķ������汾��Ҫ�޸�Ϊ��Ӧ��������!!��
BANK=DMNB
#Ӧ�ñ��		��00��ʼ������
ifeq ($(TMSFUNC),USETMS)
APPNO = 50
else
APPNO = 00
endif

#�汾���			��ÿ�ζ��棬�汾��Ҫ����!!!!��
VERNO = 01

#�ڲ��汾��Ϊ�������ڣ��ڲ��汾�Ŷ��壬��ʽΪ:4λ��������+1λӦ�����+1λ���а汾+YYYYMMDD+2λ��ţ�
INTERVER = $(join "\"$(TAG)$(BANK)$(APPNO), $(POSYEAR)$(POSMONTH)$(POSDAY)$(VERNO)\"")

CFLAGS += -Wall -DNDEBUG -DRECEIPT_CONF $(CROSS_CFLAGS) -O2 $(INCPATH)



#�ļ�����·��
VPATH =src $(OBJDIR)

#ͷ�ļ�����·��
INCLPATH =  -I$(INCDIR) -I$(APIDIR)

# �������Ӳ���
LDFLAGS += -L$(APIDIR) -lndk -L$(LIBDIR) -lcurl -lexpat -lrt -pthread

# ���ɵĳ�����
NAME = main
APPNAME = AADMNB00.NDK.NLD
# �������õ���ģ��
SRCS=			$(wildcard $(SRCDIR)/*.c)
SRSS=			$(notdir $(SRCS))
OBJS=			$(subst .c,.o,$(SRSS))

OBJSD=		$(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS))
OBJSY=		$(subst .c,.o,$(OBJSD))


#���������ļ�
all: NLD

$(NAME):config $(OBJS)
#	-$(RM) $(BINDIR)/$(NAME)
	$(CC) $(CFLAGS) -o $(BINDIR)/$(NAME) $(OBJSY) $(LDFLAGS)
	$ sed -i '/^strVerBuf/c\\strVerBuf=$(APPVER)' $(BINDIR)/headerinfo.ini 
	$ sed -i '/^strBuildTime/c\\strBuildTime=$(POSYEAR)$(POSMONTH)$(POSDAY)' $(BINDIR)/headerinfo.ini 
	unix2dos $(BINDIR)/headerinfo.ini 
%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLPATH) $< -o $(OBJDIR)/$@ >& $(OBJDIR)/$(notdir $(basename $<)).err
#�Զ����������ļ�
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
