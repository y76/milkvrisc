
.section .text
.global s_mode_entry
s_mode_entry:
    call s_mode_main             # Call C function
    j .                          # Loop forever
