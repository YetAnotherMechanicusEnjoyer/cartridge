CC			=	lcc
CFLAGS	=	-Iinclude -Isrc/autogen -Isrc/autogen/8x8 -Wa-l -Wl-m -Wl-j -Wl-yt0x1B -Wl-ya4

CARTRIDGE	=	CARTRIDGE.gb

ASM_FILES	=	dma.asm			\
						engine.asm	\
						input.asm		\
						sram.asm		\
						text.asm		\
						timer.asm		\
						utoa.asm		\
						vram.asm

C_FILES		=	asm_wrapper.c			\
						default_states.c	\
						main.c						\
						oam.c							\
						test.c

PNG2ASSET	=	png2asset

AUTOGEN_DIR	=	src/autogen

ASSETS_PNG	=	$(wildcard assets/*.png)
ASSETS_C		=	$(patsubst assets/%.png, $(AUTOGEN_DIR)/%.c, $(ASSETS_PNG))

ASSETS_8_PNG	=	$(wildcard assets/8x8/*.png)
ASSETS_8_C		=	$(patsubst assets/8x8/%.png, $(AUTOGEN_DIR)/8x8/%.c, $(ASSETS_8_PNG))

SRC	=	$(addprefix src/asm/, $(ASM_FILES))	\
			$(addprefix src/c/, $(C_FILES))			\
			$(ASSETS_C)													\
			$(ASSETS_8_C)

all: $(AUTOGEN_DIR) $(CARTRIDGE)

$(AUTOGEN_DIR):
	@mkdir -p $@
	@echo -e "\x1b[36m[Mkdir] $@ directory created\x1b[0m"
	@mkdir -p $@/8x8
	@echo -e "\x1b[36m[Mkdir] $@/8x8 directory created\x1b[0m"

$(AUTOGEN_DIR)/%.c: assets/%.png
	@printf "\x1b[34m"
	@$(PNG2ASSET) $< -c $@
	@echo -e "\x1b[34m[Asset] Converted $< to $@\x1b[0m"

$(AUTOGEN_DIR)/8x8/%.c: assets/8x8/%.png
	@printf "\x1b[31m"
	@$(PNG2ASSET) $< -c $@ -spr8x8
	@echo -e "\x1b[34m[Asset] Converted $< to $@\x1b[0m"

$(CARTRIDGE): $(SRC)
	@echo -e "\x1b[33m[Compiling] $(SRC)\x1b[0m"
	@$(CC) $(CFLAGS) -o $@ $(SRC)
	@echo -e "\x1b[32m[OK] Cartridge $@ compiled\x1b[0m"

clean:
	@rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi
	@rm -rf $(AUTOGEN_DIR)
	@echo -e "\x1b[35m[Clean] Objects removed\x1b[0m"

fclean: clean
	@rm -f $(CARTRIDGE)
	@echo -e "\x1b[35m[Fclean] Cartridge removed\x1b[0m"

re: fclean all
