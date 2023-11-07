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
    ; call test_WriteScaled		; test function for WriteScaled procedure

    ; call part_03			; parameter message:	31h 31h 31h 31h 31h 31h 31h 31h 31h 31h
					; expected value:	c4h 03h 18h 31h 88h 01h 0ch 10h 10h 00h

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
    mov esi, OFFSET key			; move address of encryption key into esi
    mov edx, 0				; initialize index counter
    
encrypt:
    cmp BYTE PTR [message + edx], 0	; if reached end of string
    jz end_encrypt			; true -> end of string reached complete function

    cmp BYTE PTR [esi], 0		; if rotation value is zero

    jz loop_encrypt			; true -> continue to next iteration
    jns right_shift			; shift value is positive -> jump to right_shift
    js left_shift			; shift value is negative -> jump to left_shift

right_shift:
    mov cl, BYTE PTR [esi]		; move shift value into cl
    shr [message + edx], cl		; right shift message at current index by cl
    jmp loop_encrypt			; continue to next iteration

left_shift:
    mov cl, BYTE PTR [esi]		; move shift value into cl
    neg cl				; convert negative value to positive
    shl [message + edx], cl		; left shift message at current index by cl
    jmp loop_encrypt			; continue to next iteration

loop_encrypt:
    add edx, TYPE BYTE			; increment edx
    add esi, TYPE BYTE			; increment esi

    jmp encrypt				; continue encryption

end_encrypt:
    ret					; end of function reached
part_03 ENDP

END main
