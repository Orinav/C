.entry LENGTH
    .extern W
mcro m1
mov @r1, @r2
    sub @r5, @r6
    endmcro
m1
MAIN:       mov @r3 ,LENGTH
LOOP:       jmp 8
prn -5
bne W
sub @r1, @r4
    bne 98
L1:         inc K
    .entry LOOP
jmp W
END:        stop
STR:        .string "abcdef"
LENGTH:     .data 6,-9,15
K:          .data 22
    .extern L3

