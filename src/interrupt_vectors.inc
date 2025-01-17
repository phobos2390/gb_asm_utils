IF !DEF(INTERRUPT_VECTORS_INC)
INTERRUPT_VECTORS_INC SET 1

INCLUDE "joypad_eval.inc"
INCLUDE "register_utils.inc"

;-------------------------------------------------------------------------------
; Reset Vectors
;-------------------------------------------------------------------------------

SECTION "RST_00",ROM0[$00]
    ret ; Reserved for the interrupt handler. If an interrupt vector is $0000
    ; it jumps here when it's triggered and returns.

SECTION "RST_08",ROM0[$08]
jp      hl ; Reserved for interrupt handler and CALL_HL macro.

SECTION "RST_10",ROM0[$10]
ret

SECTION "RST_18",ROM0[$18]
ret

SECTION "RST_20",ROM0[$20]
ret

SECTION "RST_28",ROM0[$28]
ret

SECTION "RST_30",ROM0[$30]
ret

SECTION "RST_38",ROM0[$38]
jp      reset ; Undefined reads are $FF most times, which corresponds to
              ; RST $38. It's good practice to put a reset here.

SECTION "reset", ROM0
reset:
  jp entrypoint

REPT $150 - $104
    db 0
ENDR
            
SECTION "entrypoint", ROM0[$100]
entrypoint:
  di
  jp main

REPT $150 - $104
    db 0
ENDR

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------

vblank_int_flag SET %00000001
timer_int_flag  SET %00000010
lcd_int_flag    SET %00000100
serial_int_flag SET %00001000
joypad_int_flag SET %00010000

vblank_int_bit SET 1
timer_int_bit  SET 2
lcd_int_bit    SET 3
serial_int_bit SET 4
joypad_int_bit SET 5

SECTION "VBLANK INTERRUPT", ROM0[$40]
vblank_vector:
  push af
    ld a, vblank_int_flag
    jp int_common

SECTION "LCD Stat", ROM0[$48]
lcd_vector:
  push af
    ld a, lcd_int_flag
    jp int_common

SECTION "Timer", ROM0[$50]
timer_vector:
  push af
    ld a, timer_int_flag
    jp int_common

SECTION "Serial", ROM0[$58]
serial_vector:
  push af
    ld a, serial_int_flag
    jp int_common

SECTION "Joypad0", ROM0[$60]
joypad0_vector:
  push af
    ld a, joypad_int_flag
    jp int_common

;-------------------------------------------------------------------------------

SECTION "Interrupt_logic", ROM0
int_common:
    push hl
      ld hl, callback_flags
      or [hl]
      ld [hl], a
    pop hl
  pop af
  reti

SECTION "main_loop", ROM0
main_loop:
  ld a, [callback_flags]
  ld hl, interrupt_callback_table
  cp $0
  jr z, .end
  jr .flag_evaluate
.next_flag:
  inc hl
  inc hl
  srl a
.flag_evaluate:
  bit 0, a
  jr z, .next_flag
  push hl
    ld a, [hli]
    ld h, [hl]
    ld l, a
    CALL_HL
  pop hl
  xor a
  ld [callback_flags], a
.end:
  halt
  jp main_loop 

SECTION "Null callback", ROM0
null_cb:
  ret

SECTION "int_set_timer", ROM0
int_set_timer_de: ; de - timer interrupt callback function
  push bc
    ld bc, timer_f
    call ld_ibc_de
  pop bc
  ret

SECTION "int_set_joypad", ROM0
int_set_joypad_de: ; de - joypad interrupt callback function
  push bc
    ld bc, joypad_f
    call ld_ibc_de
  pop bc
  ret

SECTION "Init_all_callbacks", ROM0
init_callbacks:
  push bc
    push de
      ; vblank_f = &null_cb
      ld de, null_cb
      ld bc, vblank_f
      call ld_ibc_de
      
      ; lcd_f = &null_cb
      ld bc, lcd_f
      call ld_ibc_de
      
      ; timer_f = &null_cb
      ld bc, timer_f
      call ld_ibc_de
      
      ; serial_f = &null_cb
      ld bc, serial_f
      call ld_ibc_de
      
      ; joypad_f = &null_cb
      ld bc, joypad_f
      call ld_ibc_de
    pop de
  pop bc
  ret

SECTION "Interrupt_callbacks", HRAM
interrupt_callback_table:
vblank_f: DS 2
timer_f: DS 2
lcd_f: DS 2
serial_f: DS 2
joypad_f: DS 2

SECTION "Callback Flags", HRAM
callback_flags: DS 1
              
SECTION "Callback Stats", WRAM0
amount_cb_invoked: DS 2

ENDC ; INTERRUPT_VECTORS_INC