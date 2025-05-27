
.section .text
.global s_mode_entry
s_mode_entry:
    la sp, s_mode_stack_top      # Load S-mode stack top
    call s_mode_main             # Call C function
    j .                          # Loop forever
