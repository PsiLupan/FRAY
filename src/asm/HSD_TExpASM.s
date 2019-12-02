#include <ogc/machine/asm.h>
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