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