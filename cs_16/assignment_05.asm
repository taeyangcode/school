; Assignment #5

; Program Description: This assignment has you work with bit-shifting operations, as well as writing your first procedures in assembly language (aside from the main procedure).
; Author: Corey Mostero 2566652
; Creation Date: 03 November 2023
; Revisions: N/A
; Date: 10 November 2023

.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode: DWORD

INCLUDE Irvine32.inc

DECIMAL_OFFSET=5

.data

decimal_one BYTE "100123456789765"
decimal_two BYTE "123456"
decimal_three BYTE "12345"

message BYTE "1111111111"
key BYTE -2, 4, 1, 0, -3, 5, 2, -4, -4, 6

.code

main PROC
    ; call test_WriteScaled ; test function for WriteScaled procedure

    call part_03

    INVOKE ExitProcess, 0
main ENDP

part_01 PROC
    mov bx, ax				; get multiplicand
    shl bx, 4				; multiply by factor 16
    push bx				; save factor 16

    mov bx, ax				; get multiplicand
    shl bx, 3				; multiply by factor 8

    shl ax, 1				; multiply by factor 2
    add ax, bx				; add factor 8
    pop bx				; recover factor 16
    add ax, bx				; add factor 16
part_01 ENDP

WriteScaled PROC
    mov ebx, ecx			; overwrite ebx to length of string
    sub ebx, DECIMAL_OFFSET		; subtract decimal offset to find position to print decimal

write_string:
    cmp WORD PTR [edx], 0		; if current letter is 0
    jz end_loop				; true -> reached end of string function done

    cmp ebx, 0				; if decimal position is reached
    jnz write_char			; false -> print current letter

    mov al, '.'				; decimal position is reached, move into al register
    call WriteChar			; print al register

    dec ebx				; decrement ebx to avoid repeated decimal print

    jmp write_string			; jmp instead of loop to avoid ecx decrement (original byte position hasn't been printed)

write_char:
    mov al, BYTE PTR [edx]		; move current letter into al register
    call WriteChar			; print al register

    dec ebx				; decrement ebx to reach or avoid repeated decimal positon
    add edx, TYPE BYTE			; increment string position

    loop write_string			; decrement loop counter

end_loop:
    call CrlF				; reached end of string, print new line
    ret					; return from function
WriteScaled ENDP

test_WriteScaled PROC
    mov edx, OFFSET decimal_one
    mov ecx, LENGTHOF decimal_one
    mov ebx, OFFSET DECIMAL_OFFSET

    call WriteScaled			; expected output: "1001234567.89765"

    mov edx, OFFSET decimal_two
    mov ecx, LENGTHOF decimal_two
    mov ebx, OFFSET DECIMAL_OFFSET

    call WriteScaled			; expected output: "1.23456"

    mov edx, OFFSET decimal_three
    mov ecx, LENGTHOF decimal_three
    mov ebx, OFFSET DECIMAL_OFFSET

    call WriteScaled			; expected output: ".12345"
test_WriteScaled ENDP

part_03 PROC
    mov esi, OFFSET key
    mov edx, 0
    
encrypt:
    cmp BYTE PTR [message + edx], 0
    jz end_encrypt

    cmp BYTE PTR [esi], 0

    jz loop_encrypt
    jns right_shift
    js left_shift

right_shift:
    mov cl, BYTE PTR [esi]
    shr [message + edx], cl
    jmp loop_encrypt

left_shift:
    mov cl, BYTE PTR [esi]
    neg cl
    shl [message + edx], cl
    jmp loop_encrypt

loop_encrypt:
    add edx, TYPE BYTE
    add esi, TYPE BYTE

    jmp encrypt

end_encrypt:
    ret
part_03 ENDP

END main
