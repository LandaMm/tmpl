format ELF64
; Symbols:

public STDOUT_FILENO
public main
extrn strlen
extrn write

section '.data' data readable writeable

STDOUT_FILENO: db ...
