.set noreorder
.set volatile
.set noat
#include<regdef.h>
.data                                   
.text
.align 3
.globl func
.ent func
func:
		ldgp	gp,0(pv)                           
		lda	sp,-96(sp)                          
		.frame fp, 96, ra , 0                   
		stq	ra,16(sp)                           
		stq	fp,24(sp)                           
		stq	s0,32(sp)                           
		bis	sp,sp,fp                            
		.prologue 1                             
		stl	a0,88(fp)                           
		lda	s0,16(sp)                           
		ldq	$0,48(fp)                           //p2
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		bis	$31,4,$0                            // 4
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		addl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		lda	$0,40(fp)                           //p1
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	t1,0(s0)                            
		stq	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	$0,40(fp)                           //p1
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		bis	$31,8,$0                            // 8
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		addl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		lda	$0,48(fp)                           //p2
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	t1,0(s0)                            
		stq	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	$0,48(fp)                           //p2
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		bis	$31,4,$0                            // 4
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		subl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		lda	$0,40(fp)                           //p1
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	t1,0(s0)                            
		stq	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	$0,48(fp)                           //p2
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldl	$0,80(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		bis	$31,4,$0                            // 4
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		mull	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		addl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		lda	$0,40(fp)                           //p1
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	t1,0(s0)                            
		stq	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	$0,48(fp)                           //p2
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		lda	$0,40(fp)                           //p1
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		subl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		lda	$0,80(fp)                           //j
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		stl	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	$0,80(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		$1kkk:                                  
		bis	sp,sp,fp                            
		ldq	s0,32(sp)                           
		ldq	ra,16(sp)                           
		ldq	fp,24(sp)                           
		addq	sp, 96,sp                          
		ret $31, (ra), 1                        
.end func
                                        
