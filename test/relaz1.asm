.386P
if @Version gt 510
.model FLAT
else
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT DWORD USE32 PUBLIC 'BSS'
_BSS	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS
FLAT	GROUP _DATA, CONST, _BSS
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC _proc                            ;proc
_TEXT SEGMENT
_k$ = 8
_p$ = 12
_c$ = 16
_proc PROC NEAR                         ;proc
		push	ebp                                
		mov	ebp, esp                            
		push	10                                 ; 10
		lea	eax, BYTE PTR _c$[ebp]              
		push	eax                                
		                                        
		pop	eax                                 
		movsx	eax, al                           
		push	eax                                
		pop	eax                                 
		add	DWORD PTR [esp], eax                
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _k$[ebp], eax             
		pop	eax                                 
		push	DWORD PTR _k$[ebp]                 
		mov	eax, DWORD PTR [esp]                
		pop	ebx                                 
		mov DWORD PTR [ebx], eax                
		pop	eax                                 
		push	DWORD PTR _k$[ebp]                 
		push	20                                 ; 20
		pop	eax                                 
		imul	eax, dword ptr [esp]               
		mov	dword ptr [esp], eax                
		pop	eax                                 
		jmp	$uscita                             
		$uscita:                                
		pop	ebp                                 
		ret                                     
_proc ENDP                              ;proc
                                        
_TEXT ENDS
END