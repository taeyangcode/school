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

.data

.code
main PROC
    INVOKE ExitProcess, 0
main ENDP
