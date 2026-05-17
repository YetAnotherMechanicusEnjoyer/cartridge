#ifndef FONT_H
#define FONT_H

#include <types.h>
#include <stdint.h>
 
#define FONT_256ENCODING    0
#define FONT_128ENCODING    1
#define FONT_NOENCODING     2
 
#define FONT_COMPRESSED     4
 
/* See gb.h/M_NO_SCROLL and gb.h/M_NO_INTERP */
 
typedef uint16_t font_t;
 
 
extern uint8_t font_spect[], font_italic[], font_ibm[], font_min[];
 
extern uint8_t font_ibm_fixed[];
 
void font_init(void);
 
font_t font_load(void *font) OLDCALL;
 
font_t font_set(font_t font_handle) OLDCALL;
 
typedef struct sfont_handle mfont_handle;
typedef struct sfont_handle *pmfont_handle;
 
struct sfont_handle {
    uint8_t first_tile;
    void *font;
};
 
void font_color(uint8_t forecolor, uint8_t backcolor) OLDCALL;

#endif // FONT_H
