X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib
PKG_CONFIG = pkg-config

CFLAGS := -I. -I./nano/ -I./st/ -I./pictures/ -g -O2 -Wall -I$(X11INC) $(shell $(PKG_CONFIG) --cflags fontconfig) $(shell $(PKG_CONFIG) --cflags freetype2)
LDFLAGS := -lncursesw -I$(X11INC) -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft $(shell $(PKG_CONFIG) --libs fontconfig) $(shell $(PKG_CONFIG) --libs freetype2)

# Files lists
NANO_C_SRC := browser.c  chars.c  color.c  cut.c  files.c  global.c  help.c  move.c  nano.c  prompt.c  rcfile.c  search.c  text.c  utils.c  winio.c
ST_C_SRC := st.c x.c
PICTURES_C_SRC = bg.c
C_SRC := $(addprefix nano/, $(NANO_C_SRC)) $(addprefix st/, $(ST_C_SRC)) $(addprefix pictures/, $(PICTURES_C_SRC))
C_SRC += nanos.c
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

%.o : %.c st/config_st.h nano/config_nano.h pictures/bg.h
	$(CC) -c $< $(CFLAGS) -o $@

pictures/bg.c : pictures/*.ff
	COUNT=$$(ls pictures/bg*.ff | wc -l); \
		echo "" > $@; \
		for i in $$(seq 1 $$COUNT); \
			do xxd -i pictures/bg$$i.ff >> $@; \
		done; \
		sed -i $@ -e "s:unsigned int.*::"; \
		sed -i $@ -e "s:unsigned:static const unsigned:"; \
		echo "const unsigned char* bg[] = {" >> $@; \
		for i in $$(seq 1 $$COUNT); \
			do printf "pictures_bg%i_ff,\n" $$i >> $@; \
		done; \
		echo "};" >> $@; \
		echo "const unsigned int bg_len = $$COUNT;" >> $@;

$(TARGET).bin : $(C_OBJS)
	$(CC) $(C_OBJS) $(LDFLAGS) -o $@

clean : 
	$(RM) $(C_OBJS)
	$(RM) $(TARGET).bin
	$(RM) pictures/bg.c

