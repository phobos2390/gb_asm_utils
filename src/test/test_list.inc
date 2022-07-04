IF !DEF(TEST_LIST_INC)
TEST_LIST_INC=1

INCLUDE "sample_test.inc"
INCLUDE "half_hex_digit_tests.inc"
INCLUDE "timing_table_test.inc"

SECTION "test_directory", ROM0
test_defs: 
.begin:
.first_begin:
  DW hello_world_test_name, hello_world_test
.first_end:
  ; -------------------------------------------------------------------
  ;     ADD any additional tests below
  ; -------------------------------------------------------------------
  DW hex_digits_stitch_test_name, hex_digits_stitch_test
  DW init_timing_table_test_name, init_timing_table_test
  DW entry_set_test_name, entry_set_test
  DW entry_invocation_test_name, entry_invocation_test
  DW twice_tick_test_name, twice_tick_invocation_test
.end:

sizeof_test_defs EQU (test_defs.end - test_defs.begin)
sizeof_single_test_def EQU (test_defs.first_end - test_defs.first_begin)
number_of_tests EQU sizeof_test_defs / sizeof_single_test_def

SECTION "test_names", ROM0
hello_world_test_name: DB "hello world! test",$0
; -------------------------------------------------------------------
;     ADD any additional test names below
; -------------------------------------------------------------------
hex_digits_stitch_test_name: DB "Hex digits test (",$FF,")",$0
init_timing_table_test_name: DB "Init timing table", $0
entry_set_test_name:         DB "Timing entry set",$0
entry_invocation_test_name:  DB "Timing entry test", $0
twice_tick_test_name:        DB "Tick twice test", $0
ENDC ; TEST_LIST_INC
