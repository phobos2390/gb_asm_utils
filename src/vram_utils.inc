IF !DEF(VRAM_UTILS_INC)
VRAM_UTILS_INC SET 1

INCLUDE "hardware.inc"
INCLUDE "register_utils.inc"
INCLUDE "mem_utils.inc"
INCLUDE "interrupt_vectors.inc"

SECTION "VRAM iterator", WRAM0
vram_iterator: DS 2

SECTION "newline offset", WRAM0
newline_offset: DS 2

VBLANK_WAIT_INT : MACRO
  push bc
    ld c, LOW(callback_flags)
    ldh a, [c]
    res vblank_int_bit, a
    ldh [c], a
.loop\@:
    halt
    ldh a, [c]
    bit vblank_int_bit, a    
    jp z, .loop\@
  pop bc
ENDM

VBLANK_WAIT : MACRO
.waitVBlank\@:
  ld a, [rLY]
  cp $90 ; Check if the LCD is past VBlank
  jr c, .waitVBlank\@
ENDM

RET_V : MACRO
  ld a, [rLY]
  cp $90 ; Check if the LCD is past VBlank
  ret c
ENDM

SECTION "init vram iterator", ROM0
init_vram_iterator:
  push hl
    push bc
      ld hl, _SCRN0 ; background 1
      ld bc, vram_iterator
      call ld_ibc_hl
      ld hl, $0000
      ld bc, newline_offset
      call ld_ibc_hl
    pop bc
  pop hl
  ret

SECTION "clear screen", ROM0
clear_screen:
  push hl
    push bc
      push de
        ;  memset(0, _SCRN0, sizeof(SCRN0))
        ld d, $0 ; param 1
        ld hl, _SCRN0 ; &background 1
        ld bc, (_SCRN1 - _SCRN0) ; sizeof(background 1)
        call memset
      pop de
    pop bc
  pop hl
  ret


SECTION "init window", ROM0
init_window:
  ld a, $7
  ld [rWX], a
  ld a, $90
  ld [rWY], a
  ret

ENDC ; VRAM_UTILS_INC
