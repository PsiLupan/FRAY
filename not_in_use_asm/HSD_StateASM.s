#include <ogc/machine/asm.h>
.extern GX_SetChanAmbColor
.extern GX_SetChanMatColor
.extern GX_SetChanCtrl

    .globl HSD_SetupChannel
HSD_SetupChannel:
	mflr	r0
	stw	r0, 0x0004 (r1)
	stwu	r1, -0x0060 (r1)
	stmw	r27, 0x004C (r1)
	mr.	r27, r3
	lis	r3, 0x8040
	addi	r31, r3, 23448
    beq        LAB_803623bc
    lwz	r3, 0x0004 (r27)
    cmpwi	r3, 255
    bne        LAB_803620d4
    b          LAB_803623bc
LAB_803620d4:
    lbz        r0,0x14(r27)
    addi       r28,r3,0x0
    rlwinm     r29,r3,0x0,0x1e,0x1f
    cmplwi     r0,0x0
    rlwinm     r30,r3,0x0,0x1f,0x1f
    beq        LAB_803621d8
    lwz        r0,0x18(r27)
    cmpwi      r0,0x0
    bne        LAB_803621d8
    rlwinm     r0,r30,0x2,0x0,0x1d
    subi       r3,r13,0x409c
    add        r3,r3,r0
    lwz        r0,0x0(r3)
    cmpwi      r0,0x0
    beq        LAB_80362140
    li         r0,0x0
    stw        r0,0x0(r3)
    addi       r4,r1,0x40
    addi       r3,r30,0x4
    lwz        r0,0xc(r27)
    stw	r0, 0x0040 (sp)
    bl         GX_SetChanAmbColor
    mulli      r3,r30,0x30
    lwz        r0,0xc(r27)
    add        r3,r31,r3
    stw        r0,0x3c(r3)
    b          LAB_803621d8
LAB_80362140:
    subi       r0,r28,0x4
    cmplwi     r0,0x1
    bgt        LAB_8036216c
    mulli      r0,r30,0x30
    lwz        r4,0xc(r27)
    add        r3,r31,r0
    lwz        r0,0x3c(r3)
	cmplw      r4,r0
    beq        LAB_803621d8
    stw        r4,0x3c(r3)
    b          LAB_803621c4
LAB_8036216c:
    cmplwi     r28,0x1
    bgt        LAB_803621a4
    mulli      r0,r30,0x30
    lwz        r5,0xc(r27)
    add        r3,r31,r0
    addi       r4,r3,0x3c
    lwz        r3,0x3c(r3)
    xor        r0,r5,r3
    rlwinm.    r0,r0,0x0,0x0,0x17
    beq        LAB_803621d8
    rlwinm     r0,r5,0x0,0x0,0x17
    rlwimi     r0,r3,0x0,0x18,0x1f
    stw        r0,0x0(r4)
    b          LAB_803621c4
LAB_803621a4:
    mulli      r0,r30,0x30
    lbz        r3,0xf(r27)
    add        r4,r31,r0
    lbz        r0,0x3f(r4)
    addi       r4,r4,0x3f
    cmplw      r3,r0
    beq        LAB_803621d8
    stb        r3,0x0(r4)
LAB_803621c4:
    lwz        r0,0xc(r27)
    addi       r4,r1,0x3c
    addi       r3,r28,0x0
    stw        r0,0x003C(r1)
    bl         GX_SetChanAmbColor
LAB_803621d8:
    lwz        r0,0x1c(r27)
    cmpwi      r0,0x0
    bne        LAB_803622c4
    rlwinm     r0,r30,0x2,0x0,0x1d
    subi       r3,r13,0x4094
    add        r3,r3,r0
    lwz        r0,0x0(r3)
    cmpwi      r0,0x0
    beq        LAB_8036222c
    li         r0,0x0
    stw        r0,0x0(r3)
    addi       r4,r1,0x38
    addi       r3,r30,0x4
    lwz        r0,0x10(r27)
    stw        r0,0x0038(r1)
    bl         GX_SetChanMatColor
    mulli      r3,r30,0x30
    lwz        r0,0x10(r27)
    add        r3,r31,r3
    stw        r0,0x40(r3)
    b          LAB_803622c4
LAB_8036222c:
    subi       r0,r28,0x4
    cmplwi     r0,0x1
    bgt        LAB_80362258
    mulli      r0,r30,0x30
    lwz        r4,0x10(r27)
    add        r3,r31,r0
    lwz        r0,0x40(r3)
    cmplw      r4,r0
    beq        LAB_803622c4
    stw        r4,0x40(r3)
    b          LAB_803622b0
LAB_80362258:
    cmplwi     r28,0x1
    bgt        LAB_80362290
    mulli      r0,r30,0x30
    lwz        r5,0x10(r27)
    add        r3,r31,r0
    addi       r4,r3,0x40
    lwz        r3,0x40(r3)
    xor        r0,r5,r3
    rlwinm.    r0,r0,0x0,0x0,0x17
    beq        LAB_803622c4
    rlwinm     r0,r5,0x0,0x0,0x17
    rlwimi     r0,r3,0x0,0x18,0x1f
    stw        r0,0x0(r4)
    b          LAB_803622b0
LAB_80362290:
    mulli      r0,r30,0x30
    lbz        r3,0x13(r27)
    add        r4,r31,r0
    lbz        r0,0x43(r4)
    addi       r4,r4,0x43
    cmplw      r3,r0
    beq        LAB_803622c4
    stb        r3,0x0(r4)
LAB_803622b0:
    lwz        r0,0x10(r27)
    addi       r4,r1,0x34
    addi       r3,r28,0x0
    stw        r0,0x0034(r1)
    bl         GX_SetChanMatColor
LAB_803622c4:
    mulli      r30,r29,0x30
    lbz        r4,0x14(r27)
    add        r29,r31,r30
    lbzu       r0,0x44(r29)
    cmplw      r4,r0
    bne        LAB_80362330
    add        r5,r31,r30
    lwz        r3,0x18(r27)
    lwz        r0,0x48(r5)
    cmpw       r3,r0
    bne        LAB_80362330
    lwz        r3,0x1c(r27)
    lwz        r0,0x4c(r5)
    cmpw       r3,r0
    bne        LAB_80362330
    lwz        r3,0x20(r27)
    lwz        r0,0x50(r5)
    cmpw       r3,r0
    bne        LAB_80362330
    lwz        r3,0x24(r27)
    lwz        r0,0x54(r5)
    cmpw       r3,r0
    bne        LAB_80362330
    lwz        r3,0x28(r27)
    lwz        r0,0x58(r5)
    cmpw       r3,r0
    beq        LAB_803623bc
LAB_80362330:
    lbz        r5,0x18(r27)
    or         r3,r28,r28
    lbz        r6,0x1c(r27)
    lbz        r7,0x20(r27)
    lbz        r8,0x24(r27)
    lbz        r9,0x28(r27)
    bl         GX_SetChanCtrl
    lbz        r4,0x14(r27)
    subi       r0,r28,0x4
    add        r3,r31,r30
    stb        r4,0x0(r29)
    cmplwi     r0,0x1
    lwz        r0,0x18(r27)
    stw        r0,0x48(r3)
    lwz        r0,0x1c(r27)
    stw        r0,0x4c(r3)
    lwz        r0,0x20(r27)
    stw        r0,0x50(r3)
    lwz        r0,0x24(r27)
    stw        r0,0x54(r3)
    lwz        r0,0x28(r27)
    stw        r0,0x58(r3)
    bgt        LAB_803623bc
    lbz        r0,0x14(r27)
    stb        r0,0xa4(r3)
    lwz        r0,0x18(r27)
    stw        r0,0xa8(r3)
    lwz        r0,0x1c(r27)
    stw        r0,0xac(r3)
    lwz        r0,0x20(r27)
    stw        r0,0xb0(r3)
    lwz        r0,0x24(r27)
    stw        r0,0xb4(r3)
    lwz        r0,0x28(r27)
    stw        r0,0xb8(r3)
LAB_803623bc:
    lmw	r27, 0x004C (sp)
    lwz	r0, 0x0064 (sp)
    addi       r1,r1,0x60
    mflr      r0
	blr
