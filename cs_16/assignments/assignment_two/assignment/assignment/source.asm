; Assignment #2

; Program Description: This will be a first foray into assembly language, which include utilizing the mov, add, and sub instructions.
; Author: Corey Mostero 2566652
; Creation Date: 24 September 2023
; Revisions: N/A
; Date: 24 September 2023

.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode: DWORD

.data
x DWORD 1000h
y DWORD 2000h
result1 DWORD ?
result2 DWORD ?

.code
main PROC
	mov eax, x
	sub eax, y
	mov result1, eax
	
	mov eax, x
	add eax, y
	add result1, eax

	mov eax, result1
	mov result2, eax

	mov eax, x
	add eax, y
	sub result2, eax

	INVOKE ExitProcess, 0
main ENDP
END main