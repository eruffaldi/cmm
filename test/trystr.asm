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
_f PROC NEAR                            ;f
		push	ebp                                
		mov	ebp, esp                            
		push	OFFSET FLAT:$SG0                   
		pop	eax                                 
		jmp	$uscita                             
		$uscita:                                
		pop	ebp                                 
		ret                                     
_f ENDP                                 ;f
                                        
_TEXT ENDS
_DATA	SEGMENT
$SG0    DB       'Hello World', 00H, 00H
_DATA	ENDS
END
