.global _start
.intel_syntax noprefix

_start:
    mov rax, 1              
    mov rdi, 1              
    lea rsi, [rip + hello]  
    mov rdx, 26             
    syscall                 

    mov rax, 60             
    xor rdi, rdi            
    syscall                

hello:
    .asciz "Hello, world! world hello\n"
