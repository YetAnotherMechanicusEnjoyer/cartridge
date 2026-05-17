  .module scroll
  .area _CODE

  .globl _apply_scroll
  .globl _scroll_x, _scroll_y

  ; void apply_scroll(void)
_apply_scroll:
  ld a, (#_scroll_x)
  ldh (#0xff43), a

  ld a, (#_scroll_y)
  ldh (#0xff42), a
  ret
