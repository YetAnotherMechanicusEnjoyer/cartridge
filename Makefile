CC			=	lcc
CFLAGS	=	-Iinclude -Wa-l -Wl-m -Wl-j -Wl-yt0x1B -Wl-ya4

CARTRIDGE	=	CARTRIDGE.gb

ASM_FILES	=	engine.asm	\
						input.asm		\
						sram.asm		\
						text.asm		\
						timer.asm		\
						utoa.asm		\
						vram.asm

C_FILES		=	asm_wrapper.c			\
						default_states.c	\
						main.c						\
						test.c

SRC	=	$(addprefix src/asm/, $(ASM_FILES))	\
			$(addprefix src/c/, $(C_FILES))

all: $(CARTRIDGE)

$(CARTRIDGE): $(SRC)
	@$(CC) $(CFLAGS) -o $@ $(SRC)
	@echo -e "\x1b[32m[OK] Cartridge $@ compiled\x1b[0m"

clean:
	@rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.sav
	@echo -e "\x1b[35m[Clean] Objects removed\x1b[0m"

fclean: clean
	@rm -f $(BINARY)
	@echo -e "\x1b[35m[Fclean] Cartridge removed\x1b[0m"

re: fclean all
