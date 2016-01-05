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
		lda	sp,-72(sp)                          
		.frame fp, 72, ra , 0                   
		stq	ra,24(sp)                           
		stq	fp,32(sp)                           
		stq	s0,40(sp)                           
		bis	sp,sp,fp                            
		.prologue 1                             
		stl	a0,56(fp)                           
		stl	a1,64(fp)                           
		lda	s0,24(sp)                           
		lda	$0,48(fp)                           //w
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		stl	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		lda	$0,48(fp)                           //w
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
		ldq	s0,40(sp)                           
		ldq	ra,24(sp)                           
		ldq	fp,32(sp)                           
		addq	sp, 72,sp                          
		ret $31, (ra), 1                        
.end f
                                        
