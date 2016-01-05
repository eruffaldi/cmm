.set noreorder
.set volatile
.set noat
#include<regdef.h>
.data                                   
.text
.align 3
.globl f
.ent f
f:
		ldgp	gp,0(pv)                           
		lda	sp,-56(sp)                          
		.frame fp, 56, ra , 0                   
		stq	ra,16(sp)                           
		stq	fp,24(sp)                           
		stq	s0,32(sp)                           
		bis	sp,sp,fp                            
		.prologue 1                             
		lda	s0,16(sp)                           
		ldl	$0,48(fp)                           //c
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		lda	$0,44(fp)                           //b
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		stl	t1,0(t0)                            
		                                        
		lda	$0,40(fp)                           //a
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		stl	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		$1kkk:                                  
		bis	sp,sp,fp                            
		ldq	s0,32(sp)                           
		ldq	ra,16(sp)                           
		ldq	fp,24(sp)                           
		addq	sp, 56,sp                          
		ret $31, (ra), 1                        
.end f
                                        
