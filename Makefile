CC			=	lcc
CARTRIDGE	=	CARTRIDGE.gb

PNG2ASSET	=	png2asset
AUTOGEN_DIR	=	src/autogen

AUTOGEN_DIRS	:=	$(shell find $(AUTOGEN_DIR) -type d 2>/dev/null)
INC_DIRS 			:= include $(AUTOGEN_DIRS)
CFLAGS				:=	$(addprefix -I,$(INC_DIRS)) -Wa-l -Wl-m -Wl-j -Wl-yt0x1B -Wl-ya4

ASSETS_PNG	:=	$(shell find assets -name "*.png")
AUTOGEN_C		:= $(patsubst assets/%.png, $(AUTOGEN_DIR)/%.c, $(ASSETS_PNG))

ASM_FILES	:=	$(wildcard src/asm/*.asm)
C_FILES		:=	$(wildcard src/c/*.c) $(AUTOGEN_C)

SRC	=	$(ASM_FILES) $(C_FILES)

all: $(AUTOGEN_DIRS) $(CARTRIDGE)

$(AUTOGEN_DIRS):
	@mkdir -p $@
	@echo -e "\x1b[36m[Autogen] Directory: $@\x1b[0m"

$(AUTOGEN_DIR)/%.c: assets/%.png
	@printf "\x1b[31m"
	@$(PNG2ASSET) $< -c $@
	@echo -e "\x1b[34m[Autogen] Default: $@\x1b[0m"

$(AUTOGEN_DIR)/8x8/%.c: assets/8x8/%.png
	@printf "\x1b[31m"
	@$(PNG2ASSET) $< -c $@ -spr8x8 -keep_duplicate_tiles
	@echo -e "\x1b[34m[Autogen] Sprite: $@\x1b[0m"

$(AUTOGEN_DIR)/map/%.c: assets/map/%.png
	@printf "\x1b[31m"
	@$(PNG2ASSET) $< -c $@ -map -noflip
	@echo -e "\x1b[34m[Autogen] Map: $@\x1b[0m"

$(CARTRIDGE): $(SRC)
	@echo -e "\x1b[33m[Compiling] $@...\x1b[0m"
	@$(CC) $(CFLAGS) -o $@ $(SRC)
	@echo -e "\x1b[32m[OK] $@ compiled\x1b[0m"

clean:
	@rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi
	@rm -rf $(AUTOGEN_DIR)
	@echo -e "\x1b[35m[Clean] Objects removed\x1b[0m"

fclean: clean
	@rm -f $(CARTRIDGE)
	@echo -e "\x1b[35m[Fclean] Cartridge removed\x1b[0m"

re: fclean all

.PHONY: all clean fclean re
