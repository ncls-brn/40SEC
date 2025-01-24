bits 64
global _start

section .data
Hello db "Hello world", 10

section .text
_start:
    mov rax, 1              
    mov rdi, 1          
    lea rsi, [Hello]     
    mov rdx, 13       
    syscall             

    mov rax, 60           
    xor rdi, rdi       
    syscall                 
