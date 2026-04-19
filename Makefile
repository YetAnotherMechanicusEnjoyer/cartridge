CC			=	lcc
CFLAGS	=	-Iinclude -Wa-l -Wl-m -Wl-j -Wl-yt0x1B -Wl-ya4

BINARY	=	CARTRIDGE.gb
FILES		=	main.c	\
					math_asm.s
SRC			=	$(addprefix src/, $(FILES))

all: $(BINARY)

$(BINARY): $(SRC)
	@$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	@rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm

fclean: clean
	@rm -f $(BINARY)

re: fclean all
