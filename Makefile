CFLAGS := -I./nano/ -g -O2 -Wall
#LDFLAGS := -lz -lmagic  -Wl,-Bsymbolic-functions -lncursesw -ltinfo
LDFLAGS := -lncursesw

# Files lists
NANO_C_SRC := browser.c  chars.c  color.c  cut.c  files.c  global.c  help.c  move.c  nano.c  prompt.c  rcfile.c  search.c  text.c  utils.c  winio.c
C_SRC := $(addprefix nano/, $(NANO_C_SRC))
C_OBJS := $(C_SRC:%.c=%.o)
TARGET := nanos

# Install targets
TARGET_DIR_BIN := /usr/local/bin
TARGET_BIN := $(TARGET_DIR_BIN)/$(TARGET)

# Commands
CC := gcc
CP := cp -f
RM := rm -rf

all : $(TARGET).bin

%.o : %.c Makefile
	$(CC) -c $< $(CFLAGS) -o $@

$(TARGET).bin : $(C_OBJS)
	$(CC) $(C_OBJS) $(LDFLAGS) -o $@

install :
	mkdir -p $(TARGET_DIR_BIN)
	$(CP) $(TARGET).bin $(TARGET_BIN)

uninstall :
	$(RM) $(TARGET_BIN)

clean : 
	$(RM) $(C_OBJS)
	$(RM) $(TARGET).bin

