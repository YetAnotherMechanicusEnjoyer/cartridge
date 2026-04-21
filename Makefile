CC			=	lcc
CFLAGS	=	-Iinclude -Wa-l -Wl-m -Wl-j -Wl-yt0x1B -Wl-ya4

BINARY	=	CARTRIDGE.gb

ASM_FILES	=	engine.s	\
						sram.s		\
						text.s		\
						utoa.s		\
						vram.s

C_FILES		=	asm_wrapper.c	\
						main.c

SRC	=	$(addprefix src/asm/, $(ASM_FILES))	\
			$(addprefix src/c/, $(C_FILES))

all: $(BINARY)

$(BINARY): $(SRC)
	@$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	@rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm

fclean: clean
	@rm -f $(BINARY)

re: fclean all
