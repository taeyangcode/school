.data
X DWORD 1000h
Y DWORD 2000h
X_Y_SUM DWORD ?
X_Y_DIF DWORD ?
result1 DWORD ?
result2 DWORD ?

.code
main PROC
    mov eax, X
    sub eax, Y
    mov X_Y_DIF, eax
    mov result1, eax

    mov eax, X
    add eax, Y
    mov X_Y_SUM, eax
    add result1, eax

    mov eax, result1
    mov result2, eax
    mov eax, X_Y_DIF
    sub result2, eax
main ENDP
END main
