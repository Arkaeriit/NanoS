X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib
PKG_CONFIG = pkg-config

CFLAGS := -I. -I./nano/ -I./st/ -I./pictures/ -g -O2 -Wall -I$(X11INC) $(shell $(PKG_CONFIG) --cflags fontconfig) $(shell $(PKG_CONFIG) --cflags freetype2)
LDFLAGS := -lncursesw -I$(X11INC) -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft $(shell $(PKG_CONFIG) --libs fontconfig) $(shell $(PKG_CONFIG) --libs freetype2)

# Files lists
C_SRC := nanos.c

NANO_C_SRC := browser.c  chars.c  color.c  cut.c  files.c  global.c  help.c  move.c  nano.c  prompt.c  rcfile.c  search.c  text.c  utils.c  winio.c
C_SRC += $(addprefix nano/, $(NANO_C_SRC))

ST_C_SRC := st.c x.c
C_SRC += $(addprefix st/, $(ST_C_SRC))

PICTURES_C_SRC = bg.c pictures.c
C_SRC += $(addprefix pictures/, $(PICTURES_C_SRC))

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

BG_COUNT := $(shell ls pictures/bg*.ff | wc -l)

pictures/bg.c : pictures/*.ff
	echo "" > $@
	for i in $$(seq 1 $(BG_COUNT)); \
		do xxd -i pictures/bg$$i.ff >> $@; \
	done
	sed -i $@ -e "s:unsigned int.*::"
	sed -i $@ -e "s:unsigned:static const unsigned:"
	echo "static const unsigned char* _bg[] = {" >> $@
	for i in $$(seq 1 $(BG_COUNT)); \
		do printf "pictures_bg%i_ff,\n" $$i >> $@; \
	done
	echo "};" >> $@
	echo "const unsigned char** bg = &_bg[0];" >> $@
	echo "const unsigned int bg_len = $(BG_COUNT);" >> $@

$(TARGET).bin : $(C_OBJS)
	$(CC) $(C_OBJS) $(LDFLAGS) -o $@

clean : 
	$(RM) $(C_OBJS)
	$(RM) $(TARGET).bin
	$(RM) pictures/bg.c
	$(RM) test-pictures.bin
	$(RM) test-pictures.ff
	$(RM) test-pictures.png

test-pictures.ff : test-pictures.bin
	./test-pictures.bin

test-pictures.png : test-pictures.ff
	ff2png < $< > $@

test-pictures.bin : pictures/pictures.c
	$(CC) $^ $(CFLAGS) -Wextra -Werror -DTEST_PICTURES_C -o $@

