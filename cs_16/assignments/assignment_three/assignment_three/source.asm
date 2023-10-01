; Assignment #3

; Program Description: This assignment has you working with arrays and loops for the first time in assembly language.
; Author: Corey Mostero 2566652
; Creation Date: 1 October 2023
; Revisions: N/A
; Date: 1 October 2023

.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode: DWORD

.data
list DWORD 100h, 200h, 300h, 400h, 500h, 600h
list_two DWORD 0, 2, 5, 9, 10

.code
main PROC
	call array_exchange

	call sum_of_gaps

	INVOKE ExitProcess, 0
main ENDP

array_exchange PROC
	mov esi, 0					; current index
	mov ecx, LENGTHOF list		; counter

exchange_elements:
	mov eax, list[esi]				; cache current element's value
	mov ebx, list[esi + TYPE list]	; cache current element + 1's value
	
	mov list[esi], ebx				; swap
	mov list[esi + TYPE list], eax	; swap

	add esi, 2 * TYPE list			; increment index counter by the size of the list element times two
	sub ecx, 1						; decrement loop counter an extra time to account for the swap of two elements
	loop exchange_elements

	ret
array_exchange ENDP

sum_of_gaps PROC
	mov esi, TYPE list_two		; begin current index from second element
	mov ecx, LENGTHOF list_two	; counter
	sub ecx, 1					; decrement counter to account for n - 1 iterations
	mov eax, 0					; sum accumulator

sum_gap:
	add eax, list_two[esi]				; add current element to accumulator
	sub eax, list_two[esi - TYPE list]	; subtract the previous element to accumulator to find the difference

	add esi, TYPE list_two				; increment the index
	loop sum_gap

	ret
sum_of_gaps ENDP

END main