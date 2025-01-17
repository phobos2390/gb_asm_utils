IF !DEF(JOYPAD_EVAL_INC)
JOYPAD_EVAL_INC SET 1

INCLUDE "func_utils.inc"
INCLUDe "interrupt_vectors.inc"

SECTION "Joypad callback tables", HRAM
button_jump_table:
button_a_f: DS 2
button_b_f: DS 2
button_select_f: DS 2
button_start_f: DS 2
pad_right_f: DS 2
pad_left_f: DS 2
pad_up_f: DS 2
pad_down_f: DS 2
button_data: DS 1
button_data_prev: DS 1
held_count: DS 1
held_refresh_rate: DS 1
held_mask: DS 1         ; set for callbacks that only operate on rising edge

button_a EQU      %00000001
button_b EQU      %00000010
button_select EQU %00000100
button_start EQU  %00001000

button_right EQU %00010000
button_left EQU  %00100000
button_up EQU    %01000000
button_down EQU  %10000000

disable_a_held EQU      %11111110
disable_b_held EQU      %11111101
disable_select_held EQU %11111011
disable_start_held EQU  %11110111

disable_right_held EQU %11101111
disable_left_held EQU  %11011111
disable_up_held EQU    %10111111
disable_down_held EQU  %01111111

enable_all_held EQU %11111111

SECTION "joypad table initialize", ROM0
init_joypad_table:
  push bc
    push hl
      ld a, $A
      ld [held_count], a
      ld [held_refresh_rate], a
      ld a, enable_all_held
      ld [held_mask], a
      ld bc, null_cb
      ld hl, pad_up_f
      call ld_ihl_bc
      ld hl, pad_down_f
      call ld_ihl_bc
      ld hl, pad_left_f
      call ld_ihl_bc
      ld hl, pad_right_f
      call ld_ihl_bc
      ld hl, button_a_f
      call ld_ihl_bc
      ld hl, button_b_f
      call ld_ihl_bc
      ld hl, button_start_f
      call ld_ihl_bc
      ld hl, button_select_f
      call ld_ihl_bc
    pop hl
  pop bc
  ret
  
SECTION "Eval continual", ROM0
eval_held_button:
  push bc
    PUSH_HL_DE
      ld a, [held_mask]
      ld d, a
      ld hl, button_data_prev
      ld a, [button_data]
      and [hl]
      and d
    POP_HL_DE
    ld b, a
    cp $0
    jp z, .none_held

    ld a, [held_count]
    dec a
    cp $0
    jp nz, .pop_ret
    ld a, [held_refresh_rate]
    ld [held_count], a
    ld a, b
  pop bc
  jp eval_joypad
.none_held:
    ld a, [held_refresh_rate]
.pop_ret:
    ld [held_count], a
  pop bc
  ret

SECTION "JOYPAD eval", ROM0
eval_rising_edge:
  push hl
    ld hl, button_data_prev
    ld a, [button_data]
    xor [hl]
    ld hl, button_data
    and [hl]
  pop hl
  jp eval_joypad

SECTION "Continue joypad eval", ROM0
eval_joypad:
  push hl
  push af
    ld hl, button_jump_table
    cp $0
    jr z, .end
    jr .loop_start ; start in the middle
.loop:
    inc hl
    inc hl   ; next callback
    srl a    ; next flag
    jr z, .end
.loop_start:
    bit 0, a
    jr z, .loop
.call_callback:
    push af
    push hl
      ld a, [hli]
      ld h, [hl]
      ld l, a
      CALL_HL
    pop hl
    pop af
    jr .loop
.end:
  pop af
  pop hl
  ret


SECTION "JOYPAD Read", ROM0
read_joypad:
  push bc
    ld a, [button_data]
    ld [button_data_prev], a
    ld a, ~P1F_4 ; direction
    ld [rP1], a
    REPT 3
      ld a, [rP1] ; Debouncing logic
    ENDR
    and a, P1F_3 | P1F_2 | P1F_1 | P1F_0
    swap a
    ld b, a
    ld a, ~P1F_5 ; other buttons
    ld [rP1], a
    REPT 6
      ld a, [rP1] ; Debouncing logic
    ENDR
    and a, P1F_3 | P1F_2 | P1F_1 | P1F_0
    or a, b
    cpl
    ld [button_data], a
  pop bc
  ret

ENDC ; JOYPAD_EVAL_INC