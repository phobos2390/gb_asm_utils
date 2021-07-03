IF !DEF(TIMING_TEST_INC)
TIMING_TEST_INC SET 1

INCLUDE "timing_utils.inc"

SECTION "expected initial timing table", ROM0
expected_initial_timing_table:
REPT timer_table_end-number_of_table_entries
  DB 0
ENDR
expected_initial_timing_table_end:

SECTION "initial timing table test", ROM0
init_timing_table_test:
  call init_timing_table

  ld de, expected_initial_timing_table
  ld hl, number_of_table_entries
  ld b, timer_table_end-number_of_table_entries
  call memcmp_fast

  jp ASSERT_A_EQ_0

SECTION "test_value", HRAM
test_value: ds 1

SECTION "example callback", ROM0
; callback where bc -> test_value
sample_callback:
  xor a
  ld [c], a
  ret

SECTION "entry invocation test", ROM0
entry_invocation_test:
  ld a, $1
  ld [test_value], a

  call init_timing_table
  ld hl, sample_callback
  ld bc, test_value
  ld de, $1
  call add_timing_table_entry_callback
  
  call timing_table_cb

  ld a, [bc]

  jp ASSERT_A_EQ_0

SECTION "entry expected values", ROM0
set_expected_values_start:
set_expected_values:
.entry_count:
  DB 1 ; number of callback entries
.valid_entry:
  DB 1 ; entry is valid
.vblank_entry:
  DB 0 ; entry is not needed in vblank
.ticks_current:
  DW $0001
.ticks_per_invocation:
  DW $0001
.callback_f:
  DW sample_callback
.context:
  DW test_value
set_expected_values_end:

SECTION "entry_set_test", ROM0
entry_set_test:
  ld a, $1
  ld [test_value], a

  call init_timing_table
  ld hl, sample_callback
  ld bc, test_value
  ld de, $1
  call add_timing_table_entry_callback
  
  ld hl, set_expected_values_start
  ld de, number_of_table_entries
  ld b, set_expected_values_end - set_expected_values_start
  call memcmp_fast

  jp ASSERT_A_EQ_0

SECTION "test_value_dec_cb", ROM0
test_value_decrement:
  ldh a, [c]
  dec a
  ldh [c], a
  ret

SECTION "twice tick expected values", ROM0
twice_tick_expected_initial_start:
twice_tick_expected_initial_values:
.entry_count:
  DB 1 ; number of callback entries
.valid_entry:
  DB 1 ; entry is valid
.vblank_entry:
  DB 0 ; entry is not needed in vblank
.ticks_current:
  DW $0002
.ticks_per_invocation:
  DW $0002
.callback_f:
  DW test_value_decrement
.context:
  DW test_value
twice_tick_expected_initial_end:
twice_tick_expected_first_invoke_start:
twice_tick_expected_first_invoke_values:
.entry_count:
  DB 1 ; number of callback entries
.valid_entry:
  DB 1 ; entry is valid
.vblank_entry:
  DB 0 ; entry is not needed in vblank
.ticks_current:
  DW $0001
.ticks_per_invocation:
  DW $0002
.callback_f:
  DW test_value_decrement
.context:
  DW test_value
twice_tick_expected_first_invoke_end:

SECTION "twice tick test", ROM0
twice_tick_invocation_test:
  ld a, $1
  ld [test_value], a

  call init_timing_table
  ld hl, test_value_decrement
  ld bc, test_value
  ld de, $2
  call add_timing_table_entry_callback
  
  ld hl, twice_tick_expected_initial_start
  ld de, number_of_table_entries
  ld b,  twice_tick_expected_initial_end - twice_tick_expected_initial_start
  call memcmp_fast
  ld hl, initialization_failed_str
  or a
  jr nz, .fail 

  call timing_table_cb

  ld hl, twice_tick_expected_first_invoke_start
  ld de, number_of_table_entries
  ld b,  twice_tick_expected_first_invoke_end - twice_tick_expected_first_invoke_start
  call memcmp_fast
  ld hl, first_invoke_failed_str
  or a
  jr nz, .fail 


  ld a, [test_value]
  or a
  jr z, .fail
  call timing_table_cb

  ld hl, twice_tick_expected_initial_start
  ld de, number_of_table_entries
  ld b,  twice_tick_expected_initial_end - twice_tick_expected_initial_start
  call memcmp_fast
  ld hl, second_invoke_failed_str
  or a
  jr nz, .fail 

  ld a, [test_value]
  or a
  jr z, .success
  jr .fail

.success:
  ld hl, $1
  ret
.fail:
  call print_hl
  ld hl, $0
  ret

SECTION "strings", ROM0
initialization_failed_str: DB "\ninitialization failed", $0
first_invoke_failed_str:   DB "\ninvoke 1 failed", $0
second_invoke_failed_str:  DB "\ninvoke 2 failed", $0

SECTION "ASSERT_NOT_0(a)", ROM0
ASSERT_A_NE_0:
  or a
  jp nz, .success
  jp .fail
.success:
  ld hl, $1
  ret
.fail:
  ld hl, $0
  ret

SECTION "ASSERT_0(a)", ROM0
ASSERT_A_EQ_0:
  or a
  jp z, .success
  jp .fail
.success:
  ld hl, $1
  ret
.fail:
  ld hl, $0
  ret

ENDC ; TIMING_TEST_INC