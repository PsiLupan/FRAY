.extern HSD_TExpGetType

	.globl assign_reg
assign_reg:
    mfspr      r0,LR
    stw	r0, 0x0004 (r1)
    stwu	r1, -0x0050 (r1)
    stmw	r22, 0x0028 (r1)
    subi	r26, r3, 1
    rlwinm     r0,r26,0x2,0x0,0x1d
    mr	r22, r5
    add	r28, r6, r0
    addi       r30,r1,0x20
    addi       r31,r1,0x1c
    li		   r24,0x4
    li         r23,0x4
    lwz	r4, -0x11F8 (r2)
    lwz	r3, -0x11F4 (r2)
    stw	r4, 0x0020 (r1)
    stw	r3, 0x001C (r1)
    b          LAB_8038591c
LAB_803857dc:
    lwz	r3, 0 (r28)
    li         r25,0x0
    rlwinm     r0,r25,0x3,0x0,0x1c
    mulli	r3, r3, 40
    add	r27, r22, r3
    lwz	r3, 0 (r27)
    add	r29, r3, r0
LAB_803857f8:
    lwz	r3, 0x0028 (r29)
    bl         HSD_TExpGetType
    cmpwi      r3,0x1
    bne        LAB_80385840
    lbz        r0,0x25(r29)
    cmplwi     r0,0x1
    bne        LAB_8038582c
    lwz	r3, 0x0028 (r29)
    lbz	r4, 0x000C (r3)
    lbzx	r3, r30, r4
    subi	r0, r3, 1
    stbx	r0, r30, r4
    b          LAB_80385840
LAB_8038582c:
    lwz	r3, 0x0028 (r29)
    lbz	r4, 0x0018 (r3)
    lbzx	r3, r31, r4
    subi	r0, r3, 1
    stbx	r0, r31, r4
LAB_80385840:
    lwz	r3, 0x0048 (r29)
	bl         HSD_TExpGetType
    cmpwi	r3, 1
    bne        LAB_80385864
    lwz	r3, 0x0048 (r29)
    lbz	r4, 0x0018 (r3)
    lbzx	r3, r31, r4
    subi	r0, r3, 1
    stbx	r0, r31, r4
LAB_80385864:
    addi       r25,r25,0x1
    cmpwi      r25,0x4
    addi       r29,r29,0x8
    blt        LAB_803857f8
    lwz	r4, 0 (r27)
    lwz	r0, 0x0008 (r4)
    cmpwi      r0,0x0
    ble        LAB_803858c8
    li         r0,0x4
    mtctr	r0
    addi	r3, r1, 35
    li	r5, 3
LAB_80385894:
    lbz	r0, 0 (r3)
    cmplwi     r0,0x0
    bne        LAB_803858bc
    lwz	r0, 0x0008 (r4)
    cmpw	r24, r5
    stbx	r0, r30, r5
    stb	r5, 0x000C (r4)
    ble        LAB_803858c8
    mr	r24, r5
    b          LAB_803858c8
LAB_803858bc:
    subi	r3, r3, 1
    subi	r5, r5, 1
    bdnz       LAB_80385894
LAB_803858c8:
    lwz	r5, 0x0014 (r4)
    cmpwi	r5, 0
    ble        LAB_80385914
    li         r0,0x4
    mtctr	r0
    addi	r3, r1, 31
    li	r6, 3
LAB_803858e4:
    lbz	r0, 0 (r3)
    cmplwi     r0,0x0
    bne        LAB_80385908
    stbx	r5, r31, r6
    cmpw	r23, r6
    stb	r6, 0x0018 (r4)
    ble        LAB_80385914
    mr	r23, r6
    b          LAB_80385914
LAB_80385908:
    subi	r3, r3, 1
    subi	r6, r6, 1
    bdnz       LAB_803858e4
LAB_80385914:
    subi       r28,r28,0x4
    subi       r26,r26,0x1
LAB_8038591c:
    cmpwi      r26,0x0
    bge        LAB_803857dc
    subfic	r3, r24, 4
    subfic	r0, r23, 4
    add	r3, r3, r0
    lwz	r0, 0x0054 (sp)
    lmw	r22, 0x0028 (sp)
    addi       r1,r1,0x50
    mtlr	r0
    blr
	
	.globl HSD_TExpMakeDag
HSD_TExpMakeDag:
	mflr      r0
    lis	r5, 0x8040
    stw	r0, 0x0004 (sp)
    stwu	sp, -0x0140 (sp)
    stmw	r22, 0x0118 (sp)
    addi	r22, r3, 0
    addi       r29,r5,0x7aa0
    addi	r31, r4, 0
    bl         HSD_TExpGetType
    cmpwi	r3, 1
    beq        LAB_80385c9c
    bl	_exit
LAB_80385c9c:
    li         r0,0x0
    addi       r26,r1,0x94
    rlwinm     r0,r0,0x2,0x0,0x1d
    stwx	r22, r26, r0
    addi       r30,r26,0x0
    li	r24, 1
    li	r25, 0
    b          LAB_80385db8
LAB_80385cbc:
    cmpwi      r25,0x20
    blt        LAB_80385cd4
    bl         _exit
LAB_80385cd4:
    lwz	r5, 0 (r30)
    li	r6, 0
    addi	r8, r5, 0
LAB_80385ce0:
    lbz	r0, 0x0024 (r5)
    cmplwi     r0,0x1
    bne        LAB_80385d34
    cmpwi	r24, 0
    mtctr	r24
    addi	r4, r26, 0
    li         r7,0x0
    ble        LAB_80385d1c
LAB_80385d00:
    lwz	r3, 0 (r4)
    lwz	r0, 0x0028 (r5)
    cmplw	r3, r0
    beq        LAB_80385d1c
    addi	r4, r4, 4
    addi	r7, r7, 1
    bdnz       LAB_80385d00
LAB_80385d1c:
    cmpw	r7, r24
    blt        LAB_80385d34
    lwz        r3,0x28(r5)
    rlwinm	r0, r24, 2, 0, 29
    addi	r24, r24, 1
    stwx       r3,r26,r0
LAB_80385d34:
    addi	r6, r6, 1
    cmpwi	r6, 4
    addi	r5, r5, 8
    blt        LAB_80385ce0
    li	r6, 0
    addi	r5, r8, 0
LAB_80385d4c:
    lbz	r0, 0x0044 (r5)
    cmplwi     r0,0x1
    bne        LAB_80385da0
    cmpwi	r24, 0
    mtctr	r24
    addi	r4, r26, 0
    li	r7, 0
    ble        LAB_80385d88
LAB_80385d6c:
    lwz	r3, 0 (r4)
    lwz	r0, 0x0048 (r5)
    cmplw	r3, r0
    beq        LAB_80385d88
    addi	r4, r4, 4
    addi	r7, r7, 1
    bdnz       LAB_80385d6c
LAB_80385d88:
    cmpw	r7, r24
    blt        LAB_80385da0
    lwz	r3, 0x0048 (r5)
    rlwinm	r0, r24, 2, 0, 29
    addi	r24, r24, 1
    stwx	r3, r26, r0
LAB_80385da0:
    addi	r6, r6, 1
    cmpwi	r6, 4
    addi	r5, r5, 8
    blt        LAB_80385d4c
    addi       r30,r30,0x4
    addi	r25, r25, 1
LAB_80385db8:
    cmpw	r25, r24
    blt        LAB_80385cbc
    mr.	r23, r24
    li	r4, 0
    ble        LAB_80385e44
    cmpwi      r23,0x8
    subi	r3, r23, 8
    ble        LAB_803860dc
    addi	r0, r3, 7
    rlwinm     r0,r0,0x1d,0x3,0x1f
    cmpwi	r3, 0
    mtctr	r0
    addi	r3, sp, 20
    li         r0,-0x1
    ble        LAB_803860dc
LAB_80385df4:
    stw	r0, 0 (r3)
    addi	r4, r4, 8
    stw	r0, 0x0004 (r3)
    stw	r0, 0x0008 (r3)
    stw	r0, 0x000C (r3)
    stw	r0, 0x0010 (r3)
    stw	r0, 0x0014 (r3)
    stw	r0, 0x0018 (r3)
    stw	r0, 0x001C (r3)
    addi	r3, r3, 32
    bdnz       LAB_80385df4
    b          LAB_803860dc
LAB_80385e24:
    sub	r0, r23, r4
    cmpw	r4, r23
    mtctr	r0
    li         r0,-0x1
    bge        LAB_80385e44
LAB_80385e38:
    stw	r0, 0 (r3)
    addi	r3, r3, 4
    bdnz       LAB_80385e38
LAB_80385e44:
    lwz	r5, 0x0094 (r1)
    addi	r6, r23, 0
    addi	r3, r1, 148
    addi	r4, r1, 20
    li	r7, 0
    bl         CalcDistance
    li	r6, 0
    b          LAB_80385ecc
LAB_80385e64:
    addi	r3, r6, 1
    sub	r0, r23, r3
    cmpw	r3, r23
    mtctr	r0
    rlwinm	r0, r3, 2, 0, 29
    addi	r4, sp, 20
    addi	r5, sp, 148
    add	r4, r4, r0
    add	r5, r5, r0
    bge        LAB_80385ec8
LAB_80385e8c:
    lwz	r3, -0x0004 (r4)
    lwz	r0, 0 (r4)
    cmpw	r3, r0
    ble        LAB_80385ebc
    lwz	r3, -0x0004 (r5)
    lwz	r0, 0 (r5)
    stw	r0, -0x0004 (r5)
    stw	r3, 0 (r5)
    lwz	r3, -0x0004 (r4)
    lwz	r0, 0 (r4)
    stw	r0, -0x0004 (r4)
    stw	r3, 0 (r4)
LAB_80385ebc:
    addi	r4, r4, 4
    addi	r5, r5, 4
    bdnz       LAB_80385e8c
LAB_80385ec8:
    addi	r6, r6, 1
LAB_80385ecc:
    cmpw	r6, r23
    blt        LAB_80385e64
    subi	r25, r23, 1
    mulli	r0, r25, 40
    rlwinm	r30, r25, 2, 0, 29
    addi       r26,r1,0x94
    add	r27, r31, r0
    add        r26,r26,r30
    b          LAB_803860cc
LAB_80385ef0:
    lwz	r22, 0 (r26)
    li	r24, 0
    stb	r25, 0x0004 (r27)
    mr	r28, r22
    stb	r24, 0x0006 (r27)
    stb	r24, 0x0005 (r27)
    stw	r22, 0 (r27)
LAB_80385f0c:
    lbz	r0, 0x0024 (r28)
    cmplwi	r0, 1
    bne        LAB_80385fd0
    addi	r5, r25, 0
    sub	r0, r23, r5
    addi	r4, sp, 148
    mtctr	r0
    cmpw	r5, r23
    add	r4, r4, r30
    bge        LAB_80385fb8
LAB_80385f34:
    lwz	r3, 0x0028 (r28)
    lwz	r0, 0 (r4)
    cmplw	r3, r0
    bne        LAB_80385fac
    lbz	r4, 0x0005 (r27)
    mulli	r0, r5, 40
    mtctr	r4
    cmpwi	r4, 0
    addi	r3, r27, 0
    add	r6, r31, r0
    li	r5, 0
    ble        LAB_80385f7c
LAB_80385f64:
    lwz	r0, 0x0008 (r3)
    cmplw	r0, r6
    beq        LAB_80385f7c
    addi	r3, r3, 4
    addi	r5, r5, 1
    bdnz       LAB_80385f64
LAB_80385f7c:
    cmpw	r5, r4
    blt        LAB_80385fb8
    lbz	r3, 0x0005 (r27)
    addi	r0, r3, 1
    rlwinm	r3, r3, 2, 0, 29
    stb	r0, 0x0005 (r27)
    addi	r0, r3, 8
    stwx	r6, r27, r0
    lbz	r3, 0x0006 (r6)
    addi	r0, r3, 1
    stb	r0, 0x0006 (r6)
    b          LAB_80385fb8
LAB_80385fac:
    addi	r4, r4, 4
    addi	r5, r5, 1
    bdnz       LAB_80385f34
LAB_80385fb8:
    cmpw	r5, r23
    blt        LAB_80385fd0
    bl         _exit
LAB_80385fd0:
    addi	r24, r24, 1
    cmpwi	r24, 4
    addi	r28, r28, 8
    blt        LAB_80385f0c
    li	r24, 0
    addi	r28, r22, 0
LAB_80385fe8:
    lbz	r0, 0x0044 (r28)
    cmplwi	r0, 1
    bne        LAB_803860ac
    addi	r5, r25, 0
    sub	r0, r23, r5
    addi	r4, sp, 148
    mtctr	r0
    cmpw	r5, r23
    add	r4, r4, r30
    bge        LAB_80386094
LAB_80386010:
    lwz	r3, 0x0048 (r28)
    lwz	r0, 0 (r4)
    cmplw	r3, r0
    bne        LAB_80386088
    lbz	r4, 0x0005 (r27)
    mulli	r0, r5, 40
    mtctr	r4
    cmpwi	r4, 0
    addi	r3, r27, 0
    add	r6, r31, r0
    li	r5, 0
    ble        LAB_80386058
LAB_80386040:
    lwz	r0, 0x0008 (r3)
    cmplw	r0, r6
    beq        LAB_80386058
    addi	r3, r3, 4
    addi	r5, r5, 1
    bdnz       LAB_80386040
LAB_80386058:
    cmpw	r5, r4
    blt        LAB_80386094
    lbz	r3, 0x0005 (r27)
    addi	r0, r3, 1
    rlwinm	r3, r3, 2, 0, 29
    stb	r0, 0x0005 (r27)
    addi	r0, r3, 8
    stwx	r6, r27, r0
    lbz	r3, 0x0006 (r6)
    addi	r0, r3, 1
    stb	r0, 0x0006 (r6)
    b          LAB_80386094
LAB_80386088:
    addi	r4, r4, 4
    addi	r5, r5, 1
    bdnz       LAB_80386010
LAB_80386094:
    cmpw	r5, r23
    blt        LAB_803860ac
    bl         _exit
LAB_803860ac:
    addi	r24, r24, 1
    cmpwi	r24, 4
    addi	r28, r28, 8
    blt        LAB_80385fe8
    subi       r27,r27,0x28
    subi       r26,r26,0x4
    subi	r25, r25, 1
    subi       r30,r30,0x4
LAB_803860cc:
    cmpwi	r25, 0
    bge        LAB_80385ef0
    mr	r3, r23
    b          LAB_803860ec
LAB_803860dc:
    rlwinm	r0, r4, 2, 0, 29
    addi	r3, sp, 20
    add	r3, r3, r0
    b          LAB_80385e24
LAB_803860ec:
    lmw	r22, 0x0118 (sp)
    lwz	r0, 0x0144 (sp)
    addi       r1,r1,0x140
    mtlr	r0
    blr
	
.extern HSD_TExpRef
.extern HSD_TExpUnref

    .globl HSD_TExpSimplify2
HSD_TExpSimplify2:
	mflr	r0
	stw	r0, 0x0004 (r1)
	stwu	r1, -0x0028 (r1)
	stmw	r27, 0x0014 (r1)
	addi	r27, r3, 0
	addi	r31, r3, 0
	li	r30, 0
LAB_80387bc0:
	lbz	r0, 0x0024 (r31)
	lwz	r28, 0x0028 (r31)
	cmplwi	r0, 1
	lbz	r29, 0x0025 (r31)
	bne-	 LAB_80387cc0
	cmplwi	r29, 1
	bne-	 LAB_80387cc0
	lbz	r0, 0x000D (r28)
	li	r3, 0
	addi	r4, r3, 0
	cmplwi	r0, 0
	addi	r5, r3, 0
	addi	r6, r3, 0
	bne-	 LAB_80387c08
	lbz	r0, 0x0025 (r28)
	cmplwi	r0, 7
	bne-	 LAB_80387c08
	li	r6, 1
LAB_80387c08:
	cmpwi	   r6, 0
	beq-	   LAB_80387c20
    lbz		   r0, 0x002D (r28)
    cmplwi     r0,0x7
    bne        LAB_80387c20
    li         r5,0x1
LAB_80387c20:
    cmpwi      r5,0x0
    beq        LAB_80387c38
    lbz	r0, 0x000F (r28)
    cmplwi     r0,0x0
    bne        LAB_80387c38
    li         r4,0x1
LAB_80387c38:
    cmpwi      r4,0x0
    beq        LAB_80387c50
    lbz	r0, 0x0010 (r28)
    cmplwi     r0,0x0
    bne        LAB_80387c50
    li	r3, 1
LAB_80387c50: 
    cmpwi      r3, 0x0
    beq        LAB_80387cc0
    lbz		   r0, 0x003C (r28)
    cmpwi      r0,0x6
    beq        LAB_80387c74
    bge        LAB_80387cc0
    cmpwi      r0,0x5
    bge        LAB_80387c98
    b          LAB_80387cc0
LAB_80387c74:
    lbz	r3, 0x001F (r27)
    cmplwi	r3, 255
    bne        LAB_80387c8c
    lbz	r0, 0x001F (r28)
    stb        r0,0x1f(r27)
    b          LAB_80387c98
LAB_80387c8c:
    lbz	r0, 0x001F (r28)
    cmplw	r3, r0
    bne        LAB_80387cc0
LAB_80387c98:
    lwz	r3, 0x003C (r28)
    lwz	r0, 0x0040 (r28)
    stw	r3, 0x0024 (r31)
    stw	r0, 0x0028 (r31)
    lwz	r3, 0x0028 (r31)
    lbz	r4, 0x0025 (r31)
    bl         HSD_TExpRef
    addi	r3, r28, 0
    addi       r4,r29,0x0
    bl         HSD_TExpUnref
LAB_80387cc0:
    addi       r30,r30,0x1
    cmpwi      r30,0x4
    addi	r31, r31, 8
    blt        LAB_80387bc0
    li         r30,0x0
    addi	r31, r27, 0
LAB_80387cd8:
    lbz	r0, 0x0044 (r31)
    lwz	r29, 0x0048 (r31)
    cmplwi	r0, 1
    lbz	r28, 0x0045 (r31)
    bne        LAB_80387dd0
    lbz	r0, 0x0019 (r29)
    li	r3, 0
    addi	r4, r3, 0
    cmplwi	r0, 0
    addi	r5, r3, 0
    addi	r6, r3, 0
    bne        LAB_80387d18
    lbz	r0, 0x0045 (r29)
    cmplwi	r0, 7
    bne        LAB_80387d18
    li         r6,0x1
LAB_80387d18:
    cmpwi      r6,0x0
    beq        LAB_80387d30
    lbz	r0, 0x004D (r29)
    cmplwi	r0, 7
    bne        LAB_80387d30
    li         r5,0x1
LAB_80387d30:
    cmpwi      r5,0x0
    beq        LAB_80387d48
    lbz	r0, 0x001B (r29)
    cmplwi	r0, 0
    bne        LAB_80387d48
    li         r4,0x1
LAB_80387d48:
    cmpwi      r4,0x0
    beq        LAB_80387d60
    lbz	r0, 0x001C (r29)
    cmplwi	r0, 0
    bne        LAB_80387d60
    li	r3, 1
LAB_80387d60:
    cmpwi	r3, 0
    beq        LAB_80387dd0
    lbz	r0, 0x005C (r29)
    cmpwi	r0, 6
    beq        LAB_80387d84
    bge        LAB_80387dd0
    cmpwi	r0, 5
    bge        LAB_80387da8
    b          LAB_80387dd0
LAB_80387d84:
    lbz	r3, 0x0020 (r27)
    cmplwi	r3, 255
    bne        LAB_80387d9c
    lbz	r0, 0x0020 (r29)
    stb	r0, 0x0020 (r27)
    b          LAB_80387da8
LAB_80387d9c:
    lbz	r0, 0x0020 (r29)
    cmplw	r3, r0
    bne        LAB_80387dd0
LAB_80387da8:
    lwz	r3, 0x005C (r29)
    lwz	r0, 0x0060 (r29)
    stw	r3, 0x0044 (r31)
    stw	r0, 0x0048 (r31)
    lwz	r3, 0x0048 (r31)
    lbz	r4, 0x0045 (r31)
    bl         HSD_TExpRef
    addi	r3, r29, 0
    addi	r4, r28, 0
    bl         HSD_TExpUnref
LAB_80387dd0:
    addi       r30,r30,0x1
    cmpwi      r30,0x4
    addi	r31, r31, 8
    blt        LAB_80387cd8
    lmw	r27, 0x0014 (r1)
    li	r3, 0
    lwz	r0, 0x002C (r1)
    addi       r1,r1,0x28
    mtlr	r0
    blr