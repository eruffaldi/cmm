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
PUBLIC _f                               ;f
_TEXT SEGMENT
_a$ = -4
_b$ = -8
_c$ = -12
_f PROC NEAR                            ;f
		push	ebp                                
		mov	ebp, esp                            
		sub	esp, 12                             
		push	DWORD PTR _c$[ebp]                 
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _b$[ebp], eax             
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _a$[ebp], eax             
		pop	eax                                 
		$uscita:                                
		mov	esp, ebp                            
		pop	ebp                                 
		ret                                     
_f ENDP                                 ;f
                                        
_TEXT ENDS
_DATA	SEGMENT
_DATA	ENDS
END