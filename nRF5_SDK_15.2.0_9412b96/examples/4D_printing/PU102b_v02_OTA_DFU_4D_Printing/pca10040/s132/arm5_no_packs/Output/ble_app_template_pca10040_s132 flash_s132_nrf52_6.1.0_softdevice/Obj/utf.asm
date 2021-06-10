	.cpu cortex-m4
	.eabi_attribute 27, 1
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"utf.c"
	.text
.Ltext0:
	.section	.text.utf8EncodeRune,"ax",%progbits
	.align	1
	.global	utf8EncodeRune
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf8EncodeRune, %function
utf8EncodeRune:
.LFB0:
	.file 1 "C:\\Nordic SDK\\tes\\nRF5_SDK_15.2.0_9412b96\\external\\utf_converter\\utf.c"
	.loc 1 13 0
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #16
.LCFI0:
	str	r0, [sp, #4]
	str	r1, [sp]
	.loc 1 15 0
	movs	r3, #0
	strb	r3, [sp, #14]
	.loc 1 16 0
	movs	r3, #0
	strb	r3, [sp, #13]
	.loc 1 17 0
	movs	r3, #0
	strb	r3, [sp, #12]
	.loc 1 21 0
	ldr	r3, [sp, #4]
	cmp	r3, #1114112
	bcc	.L2
	.loc 1 22 0
	movw	r3, #65533
	str	r3, [sp, #4]
.L2:
	.loc 1 24 0
	ldr	r3, [sp, #4]
	cmp	r3, #55296
	bcc	.L3
	.loc 1 24 0 is_stmt 0 discriminator 1
	ldr	r3, [sp, #4]
	cmp	r3, #57344
	bcs	.L3
	.loc 1 25 0 is_stmt 1
	movw	r3, #65533
	str	r3, [sp, #4]
.L3:
	.loc 1 27 0
	ldr	r3, [sp, #4]
	cmp	r3, #127
	bhi	.L4
	.loc 1 28 0
	ldr	r3, [sp, #4]
	strb	r3, [sp, #15]
	.loc 1 29 0
	movs	r3, #1
	str	r3, [sp, #8]
	.loc 1 30 0
	b	.L5
.L4:
	.loc 1 32 0
	ldr	r3, [sp, #4]
	cmp	r3, #2048
	bcs	.L6
	.loc 1 33 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	strb	r3, [sp, #14]
	.loc 1 34 0
	ldrb	r3, [sp, #14]
	orn	r3, r3, #127
	strb	r3, [sp, #14]
	.loc 1 35 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #6
	str	r3, [sp, #4]
	.loc 1 36 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #31
	strb	r3, [sp, #15]
	.loc 1 37 0
	ldrb	r3, [sp, #15]
	orn	r3, r3, #63
	strb	r3, [sp, #15]
	.loc 1 38 0
	movs	r3, #2
	str	r3, [sp, #8]
	.loc 1 39 0
	b	.L5
.L6:
	.loc 1 41 0
	ldr	r3, [sp, #4]
	cmp	r3, #65536
	bcs	.L7
	.loc 1 42 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	strb	r3, [sp, #13]
	.loc 1 43 0
	ldrb	r3, [sp, #13]
	orn	r3, r3, #127
	strb	r3, [sp, #13]
	.loc 1 44 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #6
	str	r3, [sp, #4]
	.loc 1 45 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	strb	r3, [sp, #14]
	.loc 1 46 0
	ldrb	r3, [sp, #14]
	orn	r3, r3, #127
	strb	r3, [sp, #14]
	.loc 1 47 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #6
	str	r3, [sp, #4]
	.loc 1 48 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #15
	strb	r3, [sp, #15]
	.loc 1 49 0
	ldrb	r3, [sp, #15]
	orn	r3, r3, #31
	strb	r3, [sp, #15]
	.loc 1 50 0
	movs	r3, #3
	str	r3, [sp, #8]
	.loc 1 51 0
	b	.L5
.L7:
	.loc 1 54 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	strb	r3, [sp, #12]
	.loc 1 55 0
	ldrb	r3, [sp, #12]
	orn	r3, r3, #127
	strb	r3, [sp, #12]
	.loc 1 56 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #6
	str	r3, [sp, #4]
	.loc 1 57 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	strb	r3, [sp, #13]
	.loc 1 58 0
	ldrb	r3, [sp, #13]
	orn	r3, r3, #127
	strb	r3, [sp, #13]
	.loc 1 59 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #6
	str	r3, [sp, #4]
	.loc 1 60 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	strb	r3, [sp, #14]
	.loc 1 61 0
	ldrb	r3, [sp, #14]
	orn	r3, r3, #127
	strb	r3, [sp, #14]
	.loc 1 62 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #6
	str	r3, [sp, #4]
	.loc 1 63 0
	ldr	r3, [sp, #4]
	uxtb	r3, r3
	and	r3, r3, #7
	strb	r3, [sp, #15]
	.loc 1 64 0
	ldrb	r3, [sp, #15]
	orn	r3, r3, #15
	strb	r3, [sp, #15]
	.loc 1 65 0
	movs	r3, #4
	str	r3, [sp, #8]
.L5:
	.loc 1 68 0
	ldr	r3, [sp]
	ldrb	r2, [sp, #15]
	strb	r2, [r3]
	.loc 1 69 0
	ldr	r3, [sp, #8]
	cmp	r3, #1
	bls	.L8
	.loc 1 70 0
	ldr	r3, [sp]
	adds	r3, r3, #1
	ldrb	r2, [sp, #14]
	strb	r2, [r3]
.L8:
	.loc 1 71 0
	ldr	r3, [sp, #8]
	cmp	r3, #2
	bls	.L9
	.loc 1 72 0
	ldr	r3, [sp]
	adds	r3, r3, #2
	ldrb	r2, [sp, #13]
	strb	r2, [r3]
.L9:
	.loc 1 73 0
	ldr	r3, [sp, #8]
	cmp	r3, #3
	bls	.L10
	.loc 1 74 0
	ldr	r3, [sp]
	adds	r3, r3, #3
	ldrb	r2, [sp, #12]
	strb	r2, [r3]
.L10:
	.loc 1 75 0
	ldr	r3, [sp, #8]
	.loc 1 76 0
	mov	r0, r3
	add	sp, sp, #16
.LCFI1:
	@ sp needed
	bx	lr
.LFE0:
	.size	utf8EncodeRune, .-utf8EncodeRune
	.section	.text.utf8DecodeRune,"ax",%progbits
	.align	1
	.global	utf8DecodeRune
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf8DecodeRune, %function
utf8DecodeRune:
.LFB1:
	.loc 1 79 0
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #40
.LCFI2:
	str	r0, [sp, #12]
	str	r1, [sp, #8]
	str	r2, [sp, #4]
	.loc 1 85 0
	ldr	r3, [sp, #12]
	ldrb	r3, [r3]
	strb	r3, [sp, #23]
	.loc 1 86 0
	ldrsb	r3, [sp, #23]
	cmp	r3, #0
	blt	.L13
	.loc 1 87 0
	ldrb	r2, [sp, #23]	@ zero_extendqisi2
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 88 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #1
	str	r3, [sp, #12]
	.loc 1 89 0
	ldr	r3, [sp, #12]
	b	.L14
.L13:
	.loc 1 94 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	cmp	r3, #193
	bls	.L15
	.loc 1 94 0 is_stmt 0 discriminator 1
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	cmp	r3, #244
	bls	.L16
.L15:
	.loc 1 95 0 is_stmt 1
	ldr	r3, [sp, #4]
	movw	r2, #65533
	str	r2, [r3]
	.loc 1 96 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #1
	str	r3, [sp, #12]
	.loc 1 97 0
	ldr	r3, [sp, #12]
	b	.L14
.L16:
	.loc 1 101 0
	movs	r3, #128
	strb	r3, [sp, #39]
	.loc 1 102 0
	movs	r3, #191
	strb	r3, [sp, #38]
	.loc 1 103 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	subs	r3, r3, #224
	cmp	r3, #20
	bhi	.L17
	adr	r2, .L19
	ldr	pc, [r2, r3, lsl #2]
	.p2align 2
.L19:
	.word	.L18+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L20+1
	.word	.L17+1
	.word	.L17+1
	.word	.L21+1
	.word	.L17+1
	.word	.L17+1
	.word	.L17+1
	.word	.L22+1
	.p2align 1
.L18:
	.loc 1 106 0
	movs	r3, #160
	strb	r3, [sp, #39]
	.loc 1 107 0
	b	.L17
.L20:
	.loc 1 110 0
	movs	r3, #159
	strb	r3, [sp, #38]
	.loc 1 111 0
	b	.L17
.L21:
	.loc 1 114 0
	movs	r3, #144
	strb	r3, [sp, #39]
	.loc 1 115 0
	b	.L17
.L22:
	.loc 1 118 0
	movs	r3, #143
	strb	r3, [sp, #38]
	.loc 1 119 0
	nop
.L17:
	.loc 1 123 0
	movs	r3, #1
	str	r3, [sp, #28]
	.loc 1 124 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	cmp	r3, #223
	bls	.L23
	.loc 1 125 0
	ldr	r3, [sp, #28]
	adds	r3, r3, #1
	str	r3, [sp, #28]
.L23:
	.loc 1 126 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	cmp	r3, #239
	bls	.L24
	.loc 1 127 0
	ldr	r3, [sp, #28]
	adds	r3, r3, #1
	str	r3, [sp, #28]
.L24:
	.loc 1 128 0
	ldr	r3, [sp, #8]
	cmp	r3, #0
	beq	.L25
	.loc 1 129 0
	ldr	r3, [sp, #8]
	subs	r3, r3, #1
	str	r3, [sp, #8]
	.loc 1 130 0
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #28]
	cmp	r2, r3
	bcs	.L25
	.loc 1 131 0
	ldr	r3, [sp, #4]
	movw	r2, #65533
	str	r2, [r3]
	.loc 1 132 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #1
	str	r3, [sp, #12]
	.loc 1 133 0
	ldr	r3, [sp, #12]
	b	.L14
.L25:
	.loc 1 139 0
	movs	r3, #0
	str	r3, [sp, #24]
	.loc 1 140 0
	movs	r3, #0
	str	r3, [sp, #32]
	b	.L26
.L30:
	.loc 1 141 0
	ldr	r3, [sp, #32]
	adds	r3, r3, #1
	ldr	r2, [sp, #12]
	add	r3, r3, r2
	ldrb	r3, [r3]
	strb	r3, [sp, #22]
	.loc 1 142 0
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	ldrb	r3, [sp, #39]	@ zero_extendqisi2
	cmp	r2, r3
	bcc	.L27
	.loc 1 142 0 is_stmt 0 discriminator 1
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	ldrb	r3, [sp, #38]	@ zero_extendqisi2
	cmp	r2, r3
	bls	.L28
.L27:
	.loc 1 143 0 is_stmt 1
	movs	r3, #1
	str	r3, [sp, #24]
	.loc 1 144 0
	b	.L29
.L28:
	.loc 1 147 0 discriminator 2
	movs	r3, #128
	strb	r3, [sp, #39]
	.loc 1 148 0 discriminator 2
	movs	r3, #191
	strb	r3, [sp, #38]
	.loc 1 140 0 discriminator 2
	ldr	r3, [sp, #32]
	adds	r3, r3, #1
	str	r3, [sp, #32]
.L26:
	.loc 1 140 0 is_stmt 0 discriminator 1
	ldr	r2, [sp, #32]
	ldr	r3, [sp, #28]
	cmp	r2, r3
	bcc	.L30
.L29:
	.loc 1 150 0 is_stmt 1
	ldr	r3, [sp, #24]
	cmp	r3, #0
	beq	.L31
	.loc 1 151 0
	ldr	r3, [sp, #4]
	movw	r2, #65533
	str	r2, [r3]
	.loc 1 152 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #1
	str	r3, [sp, #12]
	.loc 1 153 0
	ldr	r3, [sp, #12]
	b	.L14
.L31:
	.loc 1 157 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	cmp	r3, #223
	bhi	.L32
	.loc 1 158 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	and	r2, r3, #31
	ldr	r3, [sp, #4]
	str	r2, [r3]
	b	.L33
.L32:
	.loc 1 159 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	cmp	r3, #239
	bhi	.L34
	.loc 1 160 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	and	r2, r3, #15
	ldr	r3, [sp, #4]
	str	r2, [r3]
	b	.L33
.L34:
	.loc 1 162 0
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	and	r2, r3, #7
	ldr	r3, [sp, #4]
	str	r2, [r3]
.L33:
	.loc 1 163 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #1
	str	r3, [sp, #12]
	.loc 1 166 0
	b	.L35
.L36:
	.loc 1 167 0 discriminator 2
	ldr	r3, [sp, #12]
	ldrb	r3, [r3]
	strb	r3, [sp, #22]
	.loc 1 168 0 discriminator 2
	ldr	r3, [sp, #12]
	adds	r3, r3, #1
	str	r3, [sp, #12]
	.loc 1 169 0 discriminator 2
	ldrb	r3, [sp, #22]
	and	r3, r3, #63
	strb	r3, [sp, #22]
	.loc 1 170 0 discriminator 2
	ldr	r3, [sp, #4]
	ldr	r3, [r3]
	lsls	r2, r3, #6
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 171 0 discriminator 2
	ldr	r3, [sp, #4]
	ldr	r2, [r3]
	ldrb	r3, [sp, #22]	@ zero_extendqisi2
	orrs	r2, r2, r3
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 166 0 discriminator 2
	ldr	r3, [sp, #28]
	subs	r3, r3, #1
	str	r3, [sp, #28]
.L35:
	.loc 1 166 0 is_stmt 0 discriminator 1
	ldr	r3, [sp, #28]
	cmp	r3, #0
	bne	.L36
	.loc 1 174 0 is_stmt 1
	ldr	r3, [sp, #12]
.L14:
	.loc 1 175 0
	mov	r0, r3
	add	sp, sp, #40
.LCFI3:
	@ sp needed
	bx	lr
.LFE1:
	.size	utf8DecodeRune, .-utf8DecodeRune
	.section	.text.utf16EncodeRune,"ax",%progbits
	.align	1
	.global	utf16EncodeRune
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf16EncodeRune, %function
utf16EncodeRune:
.LFB2:
	.loc 1 179 0
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #16
.LCFI4:
	str	r0, [sp, #4]
	str	r1, [sp]
	.loc 1 183 0
	ldr	r3, [sp, #4]
	cmp	r3, #1114112
	bcc	.L38
	.loc 1 184 0
	movw	r3, #65533
	str	r3, [sp, #4]
.L38:
	.loc 1 186 0
	ldr	r3, [sp, #4]
	cmp	r3, #55296
	bcc	.L39
	.loc 1 186 0 is_stmt 0 discriminator 1
	ldr	r3, [sp, #4]
	cmp	r3, #57344
	bcs	.L39
	.loc 1 187 0 is_stmt 1
	movw	r3, #65533
	str	r3, [sp, #4]
.L39:
	.loc 1 189 0
	ldr	r3, [sp, #4]
	cmp	r3, #65536
	bcs	.L40
	.loc 1 190 0
	ldr	r3, [sp, #4]
	uxth	r2, r3
	ldr	r3, [sp]
	strh	r2, [r3]	@ movhi
	.loc 1 191 0
	movs	r3, #1
	b	.L41
.L40:
	.loc 1 194 0
	ldr	r3, [sp, #4]
	sub	r3, r3, #65536
	str	r3, [sp, #4]
	.loc 1 195 0
	ldr	r3, [sp, #4]
	uxth	r3, r3
	ubfx	r3, r3, #0, #10
	strh	r3, [sp, #14]	@ movhi
	.loc 1 196 0
	ldr	r3, [sp, #4]
	lsrs	r3, r3, #10
	str	r3, [sp, #4]
	.loc 1 197 0
	ldr	r3, [sp, #4]
	uxth	r3, r3
	ubfx	r3, r3, #0, #10
	strh	r3, [sp, #12]	@ movhi
	.loc 1 198 0
	ldrh	r3, [sp, #12]	@ movhi
	orr	r3, r3, #-16777216
	orr	r3, r3, #16711680
	orr	r3, r3, #55296
	uxth	r2, r3
	ldr	r3, [sp]
	strh	r2, [r3]	@ movhi
	.loc 1 199 0
	ldr	r3, [sp]
	adds	r2, r3, #2
	ldrh	r3, [sp, #14]	@ movhi
	orr	r3, r3, #-16777216
	orr	r3, r3, #16711680
	orr	r3, r3, #56320
	uxth	r3, r3
	strh	r3, [r2]	@ movhi
	.loc 1 200 0
	movs	r3, #2
.L41:
	.loc 1 201 0
	mov	r0, r3
	add	sp, sp, #16
.LCFI5:
	@ sp needed
	bx	lr
.LFE2:
	.size	utf16EncodeRune, .-utf16EncodeRune
	.section	.text.utf16DecodeRune,"ax",%progbits
	.align	1
	.global	utf16DecodeRune
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf16DecodeRune, %function
utf16DecodeRune:
.LFB3:
	.loc 1 205 0
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #24
.LCFI6:
	str	r0, [sp, #12]
	str	r1, [sp, #8]
	str	r2, [sp, #4]
	.loc 1 208 0
	ldr	r3, [sp, #12]
	ldrh	r3, [r3]
	cmp	r3, #55296
	bcc	.L43
	.loc 1 208 0 is_stmt 0 discriminator 1
	ldr	r3, [sp, #12]
	ldrh	r3, [r3]
	cmp	r3, #57344
	bcc	.L44
.L43:
	.loc 1 210 0 is_stmt 1
	ldr	r3, [sp, #12]
	ldrh	r3, [r3]
	mov	r2, r3
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 211 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	str	r3, [sp, #12]
	.loc 1 212 0
	ldr	r3, [sp, #12]
	b	.L45
.L44:
	.loc 1 214 0
	ldr	r3, [sp, #12]
	ldrh	r3, [r3]
	cmp	r3, #56320
	bcc	.L46
	.loc 1 216 0
	ldr	r3, [sp, #4]
	movw	r2, #65533
	str	r2, [r3]
	.loc 1 217 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	str	r3, [sp, #12]
	.loc 1 218 0
	ldr	r3, [sp, #12]
	b	.L45
.L46:
	.loc 1 220 0
	ldr	r3, [sp, #8]
	cmp	r3, #1
	bne	.L47
	.loc 1 221 0
	ldr	r3, [sp, #4]
	movw	r2, #65533
	str	r2, [r3]
	.loc 1 222 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	str	r3, [sp, #12]
	.loc 1 223 0
	ldr	r3, [sp, #12]
	b	.L45
.L47:
	.loc 1 225 0
	ldr	r3, [sp, #12]
	ldrh	r3, [r3]	@ movhi
	strh	r3, [sp, #22]	@ movhi
	.loc 1 226 0
	ldrh	r3, [sp, #22]	@ movhi
	ubfx	r3, r3, #0, #10
	strh	r3, [sp, #22]	@ movhi
	.loc 1 227 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	ldrh	r3, [r3]
	cmp	r3, #56320
	bcc	.L48
	.loc 1 227 0 is_stmt 0 discriminator 1
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	ldrh	r3, [r3]
	cmp	r3, #57344
	bcc	.L49
.L48:
	.loc 1 229 0 is_stmt 1
	ldr	r3, [sp, #4]
	movw	r2, #65533
	str	r2, [r3]
	.loc 1 230 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	str	r3, [sp, #12]
	.loc 1 231 0
	ldr	r3, [sp, #12]
	b	.L45
.L49:
	.loc 1 233 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	str	r3, [sp, #12]
	.loc 1 234 0
	ldr	r3, [sp, #12]
	ldrh	r3, [r3]	@ movhi
	strh	r3, [sp, #20]	@ movhi
	.loc 1 235 0
	ldr	r3, [sp, #12]
	adds	r3, r3, #2
	str	r3, [sp, #12]
	.loc 1 236 0
	ldrh	r3, [sp, #20]	@ movhi
	ubfx	r3, r3, #0, #10
	strh	r3, [sp, #20]	@ movhi
	.loc 1 237 0
	ldrh	r2, [sp, #22]
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 238 0
	ldr	r3, [sp, #4]
	ldr	r3, [r3]
	lsls	r2, r3, #10
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 239 0
	ldr	r3, [sp, #4]
	ldr	r2, [r3]
	ldrh	r3, [sp, #20]
	orrs	r2, r2, r3
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 240 0
	ldr	r3, [sp, #4]
	ldr	r3, [r3]
	add	r2, r3, #65536
	ldr	r3, [sp, #4]
	str	r2, [r3]
	.loc 1 241 0
	ldr	r3, [sp, #12]
.L45:
	.loc 1 242 0
	mov	r0, r3
	add	sp, sp, #24
.LCFI7:
	@ sp needed
	bx	lr
.LFE3:
	.size	utf16DecodeRune, .-utf16DecodeRune
	.section	.text.utf8RuneCount,"ax",%progbits
	.align	1
	.global	utf8RuneCount
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf8RuneCount, %function
utf8RuneCount:
.LFB4:
	.loc 1 247 0
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{lr}
.LCFI8:
	sub	sp, sp, #28
.LCFI9:
	str	r0, [sp, #4]
	str	r1, [sp]
	.loc 1 251 0
	ldr	r3, [sp]
	cmp	r3, #0
	beq	.L51
.LBB2:
	.loc 1 254 0
	movs	r3, #0
	str	r3, [sp, #20]
	.loc 1 255 0
	ldr	r3, [sp, #4]
	str	r3, [sp, #16]
	.loc 1 256 0
	b	.L52
.L53:
	.loc 1 257 0
	add	r3, sp, #8
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #16]
	bl	utf8DecodeRune
	str	r0, [sp, #12]
	.loc 1 258 0
	ldr	r3, [sp, #20]
	adds	r3, r3, #1
	str	r3, [sp, #20]
	.loc 1 259 0
	ldr	r2, [sp, #12]
	ldr	r3, [sp, #16]
	subs	r3, r2, r3
	mov	r2, r3
	ldr	r3, [sp]
	subs	r3, r3, r2
	str	r3, [sp]
	.loc 1 260 0
	ldr	r3, [sp, #12]
	str	r3, [sp, #16]
.L52:
	.loc 1 256 0
	ldr	r3, [sp]
	cmp	r3, #0
	bne	.L53
	.loc 1 262 0
	ldr	r3, [sp, #20]
	b	.L57
.L51:
.LBE2:
	.loc 1 264 0
	movs	r3, #0
	str	r3, [sp, #20]
	.loc 1 265 0
	b	.L55
.L56:
	.loc 1 266 0
	add	r3, sp, #8
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #4]
	bl	utf8DecodeRune
	str	r0, [sp, #4]
	.loc 1 267 0
	ldr	r3, [sp, #20]
	adds	r3, r3, #1
	str	r3, [sp, #20]
.L55:
	.loc 1 265 0
	ldr	r3, [sp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L56
	.loc 1 269 0
	ldr	r3, [sp, #20]
.L57:
	.loc 1 270 0 discriminator 1
	mov	r0, r3
	add	sp, sp, #28
.LCFI10:
	@ sp needed
	ldr	pc, [sp], #4
.LFE4:
	.size	utf8RuneCount, .-utf8RuneCount
	.section	.text.utf8UTF16Count,"ax",%progbits
	.align	1
	.global	utf8UTF16Count
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf8UTF16Count, %function
utf8UTF16Count:
.LFB5:
	.loc 1 273 0
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{lr}
.LCFI11:
	sub	sp, sp, #36
.LCFI12:
	str	r0, [sp, #4]
	str	r1, [sp]
	.loc 1 278 0
	ldr	r3, [sp]
	cmp	r3, #0
	beq	.L59
.LBB3:
	.loc 1 281 0
	movs	r3, #0
	str	r3, [sp, #28]
	.loc 1 282 0
	ldr	r3, [sp, #4]
	str	r3, [sp, #24]
	.loc 1 283 0
	b	.L60
.L61:
	.loc 1 284 0
	add	r3, sp, #16
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #24]
	bl	utf8DecodeRune
	str	r0, [sp, #20]
	.loc 1 285 0
	ldr	r3, [sp, #16]
	add	r2, sp, #12
	mov	r1, r2
	mov	r0, r3
	bl	utf16EncodeRune
	mov	r2, r0
	ldr	r3, [sp, #28]
	add	r3, r3, r2
	str	r3, [sp, #28]
	.loc 1 286 0
	ldr	r2, [sp, #20]
	ldr	r3, [sp, #24]
	subs	r3, r2, r3
	mov	r2, r3
	ldr	r3, [sp]
	subs	r3, r3, r2
	str	r3, [sp]
	.loc 1 287 0
	ldr	r3, [sp, #20]
	str	r3, [sp, #24]
.L60:
	.loc 1 283 0
	ldr	r3, [sp]
	cmp	r3, #0
	bne	.L61
	.loc 1 289 0
	ldr	r3, [sp, #28]
	b	.L65
.L59:
.LBE3:
	.loc 1 291 0
	movs	r3, #0
	str	r3, [sp, #28]
	.loc 1 292 0
	b	.L63
.L64:
	.loc 1 293 0
	add	r3, sp, #16
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #4]
	bl	utf8DecodeRune
	str	r0, [sp, #4]
	.loc 1 294 0
	ldr	r3, [sp, #16]
	add	r2, sp, #12
	mov	r1, r2
	mov	r0, r3
	bl	utf16EncodeRune
	mov	r2, r0
	ldr	r3, [sp, #28]
	add	r3, r3, r2
	str	r3, [sp, #28]
.L63:
	.loc 1 292 0
	ldr	r3, [sp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L64
	.loc 1 296 0
	ldr	r3, [sp, #28]
.L65:
	.loc 1 297 0 discriminator 1
	mov	r0, r3
	add	sp, sp, #36
.LCFI13:
	@ sp needed
	ldr	pc, [sp], #4
.LFE5:
	.size	utf8UTF16Count, .-utf8UTF16Count
	.section	.text.utf16RuneCount,"ax",%progbits
	.align	1
	.global	utf16RuneCount
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf16RuneCount, %function
utf16RuneCount:
.LFB6:
	.loc 1 300 0
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{lr}
.LCFI14:
	sub	sp, sp, #28
.LCFI15:
	str	r0, [sp, #4]
	str	r1, [sp]
	.loc 1 304 0
	ldr	r3, [sp]
	cmp	r3, #0
	beq	.L67
.LBB4:
	.loc 1 307 0
	movs	r3, #0
	str	r3, [sp, #20]
	.loc 1 308 0
	ldr	r3, [sp, #4]
	str	r3, [sp, #16]
	.loc 1 309 0
	b	.L68
.L69:
	.loc 1 310 0
	add	r3, sp, #8
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #16]
	bl	utf16DecodeRune
	str	r0, [sp, #12]
	.loc 1 311 0
	ldr	r3, [sp, #20]
	adds	r3, r3, #1
	str	r3, [sp, #20]
	.loc 1 312 0
	ldr	r2, [sp, #12]
	ldr	r3, [sp, #16]
	subs	r3, r2, r3
	asrs	r3, r3, #1
	mov	r2, r3
	ldr	r3, [sp]
	subs	r3, r3, r2
	str	r3, [sp]
	.loc 1 313 0
	ldr	r3, [sp, #12]
	str	r3, [sp, #16]
.L68:
	.loc 1 309 0
	ldr	r3, [sp]
	cmp	r3, #0
	bne	.L69
	.loc 1 315 0
	ldr	r3, [sp, #20]
	b	.L73
.L67:
.LBE4:
	.loc 1 317 0
	movs	r3, #0
	str	r3, [sp, #20]
	.loc 1 318 0
	b	.L71
.L72:
	.loc 1 319 0
	add	r3, sp, #8
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #4]
	bl	utf16DecodeRune
	str	r0, [sp, #4]
	.loc 1 320 0
	ldr	r3, [sp, #20]
	adds	r3, r3, #1
	str	r3, [sp, #20]
.L71:
	.loc 1 318 0
	ldr	r3, [sp, #4]
	ldrh	r3, [r3]
	cmp	r3, #0
	bne	.L72
	.loc 1 322 0
	ldr	r3, [sp, #20]
.L73:
	.loc 1 323 0 discriminator 1
	mov	r0, r3
	add	sp, sp, #28
.LCFI16:
	@ sp needed
	ldr	pc, [sp], #4
.LFE6:
	.size	utf16RuneCount, .-utf16RuneCount
	.section	.text.utf16UTF8Count,"ax",%progbits
	.align	1
	.global	utf16UTF8Count
	.syntax unified
	.thumb
	.thumb_func
	.fpu fpv4-sp-d16
	.type	utf16UTF8Count, %function
utf16UTF8Count:
.LFB7:
	.loc 1 326 0
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{lr}
.LCFI17:
	sub	sp, sp, #36
.LCFI18:
	str	r0, [sp, #4]
	str	r1, [sp]
	.loc 1 331 0
	ldr	r3, [sp]
	cmp	r3, #0
	beq	.L75
.LBB5:
	.loc 1 334 0
	movs	r3, #0
	str	r3, [sp, #28]
	.loc 1 335 0
	ldr	r3, [sp, #4]
	str	r3, [sp, #24]
	.loc 1 336 0
	b	.L76
.L77:
	.loc 1 337 0
	add	r3, sp, #16
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #24]
	bl	utf16DecodeRune
	str	r0, [sp, #20]
	.loc 1 338 0
	ldr	r3, [sp, #16]
	add	r2, sp, #12
	mov	r1, r2
	mov	r0, r3
	bl	utf8EncodeRune
	mov	r2, r0
	ldr	r3, [sp, #28]
	add	r3, r3, r2
	str	r3, [sp, #28]
	.loc 1 339 0
	ldr	r2, [sp, #20]
	ldr	r3, [sp, #24]
	subs	r3, r2, r3
	asrs	r3, r3, #1
	mov	r2, r3
	ldr	r3, [sp]
	subs	r3, r3, r2
	str	r3, [sp]
	.loc 1 340 0
	ldr	r3, [sp, #20]
	str	r3, [sp, #24]
.L76:
	.loc 1 336 0
	ldr	r3, [sp]
	cmp	r3, #0
	bne	.L77
	.loc 1 342 0
	ldr	r3, [sp, #28]
	b	.L81
.L75:
.LBE5:
	.loc 1 344 0
	movs	r3, #0
	str	r3, [sp, #28]
	.loc 1 345 0
	b	.L79
.L80:
	.loc 1 346 0
	add	r3, sp, #16
	mov	r2, r3
	ldr	r1, [sp]
	ldr	r0, [sp, #4]
	bl	utf16DecodeRune
	str	r0, [sp, #4]
	.loc 1 347 0
	ldr	r3, [sp, #16]
	add	r2, sp, #12
	mov	r1, r2
	mov	r0, r3
	bl	utf8EncodeRune
	mov	r2, r0
	ldr	r3, [sp, #28]
	add	r3, r3, r2
	str	r3, [sp, #28]
.L79:
	.loc 1 345 0
	ldr	r3, [sp, #4]
	ldrh	r3, [r3]
	cmp	r3, #0
	bne	.L80
	.loc 1 349 0
	ldr	r3, [sp, #28]
.L81:
	.loc 1 350 0 discriminator 1
	mov	r0, r3
	add	sp, sp, #36
.LCFI19:
	@ sp needed
	ldr	pc, [sp], #4
.LFE7:
	.size	utf16UTF8Count, .-utf16UTF8Count
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x3
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.uleb128 0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.byte	0x4
	.4byte	.LCFI0-.LFB0
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0xe
	.uleb128 0
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.byte	0x4
	.4byte	.LCFI2-.LFB1
	.byte	0xe
	.uleb128 0x28
	.byte	0x4
	.4byte	.LCFI3-.LCFI2
	.byte	0xe
	.uleb128 0
	.align	2
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.byte	0x4
	.4byte	.LCFI4-.LFB2
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI5-.LCFI4
	.byte	0xe
	.uleb128 0
	.align	2
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.byte	0x4
	.4byte	.LCFI6-.LFB3
	.byte	0xe
	.uleb128 0x18
	.byte	0x4
	.4byte	.LCFI7-.LCFI6
	.byte	0xe
	.uleb128 0
	.align	2
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.byte	0x4
	.4byte	.LCFI8-.LFB4
	.byte	0xe
	.uleb128 0x4
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI9-.LCFI8
	.byte	0xe
	.uleb128 0x20
	.byte	0x4
	.4byte	.LCFI10-.LCFI9
	.byte	0xe
	.uleb128 0x4
	.align	2
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.byte	0x4
	.4byte	.LCFI11-.LFB5
	.byte	0xe
	.uleb128 0x4
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI12-.LCFI11
	.byte	0xe
	.uleb128 0x28
	.byte	0x4
	.4byte	.LCFI13-.LCFI12
	.byte	0xe
	.uleb128 0x4
	.align	2
.LEFDE10:
.LSFDE12:
	.4byte	.LEFDE12-.LASFDE12
.LASFDE12:
	.4byte	.Lframe0
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.byte	0x4
	.4byte	.LCFI14-.LFB6
	.byte	0xe
	.uleb128 0x4
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI15-.LCFI14
	.byte	0xe
	.uleb128 0x20
	.byte	0x4
	.4byte	.LCFI16-.LCFI15
	.byte	0xe
	.uleb128 0x4
	.align	2
.LEFDE12:
.LSFDE14:
	.4byte	.LEFDE14-.LASFDE14
.LASFDE14:
	.4byte	.Lframe0
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.byte	0x4
	.4byte	.LCFI17-.LFB7
	.byte	0xe
	.uleb128 0x4
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI18-.LCFI17
	.byte	0xe
	.uleb128 0x28
	.byte	0x4
	.4byte	.LCFI19-.LCFI18
	.byte	0xe
	.uleb128 0x4
	.align	2
.LEFDE14:
	.text
.Letext0:
	.file 2 "C:/Program Files/SEGGER/SEGGER Embedded Studio for ARM 4.10a/include/__crossworks.h"
	.file 3 "C:/Program Files/SEGGER/SEGGER Embedded Studio for ARM 4.10a/include/stddef.h"
	.file 4 "C:/Program Files/SEGGER/SEGGER Embedded Studio for ARM 4.10a/include/stdint.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x99d
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF101
	.byte	0xc
	.4byte	.LASF102
	.4byte	.LASF103
	.4byte	.Ldebug_ranges0+0
	.4byte	0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.4byte	.LASF52
	.byte	0x8
	.byte	0x2
	.byte	0x7e
	.4byte	0x4a
	.uleb128 0x3
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7f
	.4byte	0x4a
	.byte	0
	.uleb128 0x3
	.4byte	.LASF1
	.byte	0x2
	.byte	0x80
	.4byte	0x51
	.byte	0x4
	.byte	0
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x5
	.byte	0x4
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0x71
	.uleb128 0x7
	.4byte	0x71
	.uleb128 0x7
	.4byte	0x83
	.uleb128 0x7
	.4byte	0x8a
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x77
	.uleb128 0x5
	.byte	0x1
	.byte	0x8
	.4byte	.LASF3
	.uleb128 0x9
	.4byte	0x77
	.uleb128 0x5
	.byte	0x4
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x8
	.byte	0x4
	.4byte	0x25
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0xae
	.uleb128 0x7
	.4byte	0xae
	.uleb128 0x7
	.4byte	0xb4
	.uleb128 0x7
	.4byte	0x83
	.uleb128 0x7
	.4byte	0x8a
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x83
	.uleb128 0x8
	.byte	0x4
	.4byte	0x7e
	.uleb128 0xa
	.byte	0x58
	.byte	0x2
	.byte	0x86
	.4byte	0x243
	.uleb128 0x3
	.4byte	.LASF5
	.byte	0x2
	.byte	0x88
	.4byte	0xb4
	.byte	0
	.uleb128 0x3
	.4byte	.LASF6
	.byte	0x2
	.byte	0x89
	.4byte	0xb4
	.byte	0x4
	.uleb128 0x3
	.4byte	.LASF7
	.byte	0x2
	.byte	0x8a
	.4byte	0xb4
	.byte	0x8
	.uleb128 0x3
	.4byte	.LASF8
	.byte	0x2
	.byte	0x8c
	.4byte	0xb4
	.byte	0xc
	.uleb128 0x3
	.4byte	.LASF9
	.byte	0x2
	.byte	0x8d
	.4byte	0xb4
	.byte	0x10
	.uleb128 0x3
	.4byte	.LASF10
	.byte	0x2
	.byte	0x8e
	.4byte	0xb4
	.byte	0x14
	.uleb128 0x3
	.4byte	.LASF11
	.byte	0x2
	.byte	0x8f
	.4byte	0xb4
	.byte	0x18
	.uleb128 0x3
	.4byte	.LASF12
	.byte	0x2
	.byte	0x90
	.4byte	0xb4
	.byte	0x1c
	.uleb128 0x3
	.4byte	.LASF13
	.byte	0x2
	.byte	0x91
	.4byte	0xb4
	.byte	0x20
	.uleb128 0x3
	.4byte	.LASF14
	.byte	0x2
	.byte	0x92
	.4byte	0xb4
	.byte	0x24
	.uleb128 0x3
	.4byte	.LASF15
	.byte	0x2
	.byte	0x94
	.4byte	0x77
	.byte	0x28
	.uleb128 0x3
	.4byte	.LASF16
	.byte	0x2
	.byte	0x95
	.4byte	0x77
	.byte	0x29
	.uleb128 0x3
	.4byte	.LASF17
	.byte	0x2
	.byte	0x96
	.4byte	0x77
	.byte	0x2a
	.uleb128 0x3
	.4byte	.LASF18
	.byte	0x2
	.byte	0x97
	.4byte	0x77
	.byte	0x2b
	.uleb128 0x3
	.4byte	.LASF19
	.byte	0x2
	.byte	0x98
	.4byte	0x77
	.byte	0x2c
	.uleb128 0x3
	.4byte	.LASF20
	.byte	0x2
	.byte	0x99
	.4byte	0x77
	.byte	0x2d
	.uleb128 0x3
	.4byte	.LASF21
	.byte	0x2
	.byte	0x9a
	.4byte	0x77
	.byte	0x2e
	.uleb128 0x3
	.4byte	.LASF22
	.byte	0x2
	.byte	0x9b
	.4byte	0x77
	.byte	0x2f
	.uleb128 0x3
	.4byte	.LASF23
	.byte	0x2
	.byte	0x9c
	.4byte	0x77
	.byte	0x30
	.uleb128 0x3
	.4byte	.LASF24
	.byte	0x2
	.byte	0x9d
	.4byte	0x77
	.byte	0x31
	.uleb128 0x3
	.4byte	.LASF25
	.byte	0x2
	.byte	0x9e
	.4byte	0x77
	.byte	0x32
	.uleb128 0x3
	.4byte	.LASF26
	.byte	0x2
	.byte	0x9f
	.4byte	0x77
	.byte	0x33
	.uleb128 0x3
	.4byte	.LASF27
	.byte	0x2
	.byte	0xa0
	.4byte	0x77
	.byte	0x34
	.uleb128 0x3
	.4byte	.LASF28
	.byte	0x2
	.byte	0xa1
	.4byte	0x77
	.byte	0x35
	.uleb128 0x3
	.4byte	.LASF29
	.byte	0x2
	.byte	0xa6
	.4byte	0xb4
	.byte	0x38
	.uleb128 0x3
	.4byte	.LASF30
	.byte	0x2
	.byte	0xa7
	.4byte	0xb4
	.byte	0x3c
	.uleb128 0x3
	.4byte	.LASF31
	.byte	0x2
	.byte	0xa8
	.4byte	0xb4
	.byte	0x40
	.uleb128 0x3
	.4byte	.LASF32
	.byte	0x2
	.byte	0xa9
	.4byte	0xb4
	.byte	0x44
	.uleb128 0x3
	.4byte	.LASF33
	.byte	0x2
	.byte	0xaa
	.4byte	0xb4
	.byte	0x48
	.uleb128 0x3
	.4byte	.LASF34
	.byte	0x2
	.byte	0xab
	.4byte	0xb4
	.byte	0x4c
	.uleb128 0x3
	.4byte	.LASF35
	.byte	0x2
	.byte	0xac
	.4byte	0xb4
	.byte	0x50
	.uleb128 0x3
	.4byte	.LASF36
	.byte	0x2
	.byte	0xad
	.4byte	0xb4
	.byte	0x54
	.byte	0
	.uleb128 0xb
	.4byte	.LASF46
	.byte	0x2
	.byte	0xae
	.4byte	0xba
	.uleb128 0x9
	.4byte	0x243
	.uleb128 0x5
	.byte	0x1
	.byte	0x8
	.4byte	.LASF37
	.uleb128 0x9
	.4byte	0x253
	.uleb128 0xa
	.byte	0x20
	.byte	0x2
	.byte	0xc4
	.4byte	0x2c8
	.uleb128 0x3
	.4byte	.LASF38
	.byte	0x2
	.byte	0xc6
	.4byte	0x2dc
	.byte	0
	.uleb128 0x3
	.4byte	.LASF39
	.byte	0x2
	.byte	0xc7
	.4byte	0x2f1
	.byte	0x4
	.uleb128 0x3
	.4byte	.LASF40
	.byte	0x2
	.byte	0xc8
	.4byte	0x2f1
	.byte	0x8
	.uleb128 0x3
	.4byte	.LASF41
	.byte	0x2
	.byte	0xcb
	.4byte	0x30b
	.byte	0xc
	.uleb128 0x3
	.4byte	.LASF42
	.byte	0x2
	.byte	0xcc
	.4byte	0x320
	.byte	0x10
	.uleb128 0x3
	.4byte	.LASF43
	.byte	0x2
	.byte	0xcd
	.4byte	0x320
	.byte	0x14
	.uleb128 0x3
	.4byte	.LASF44
	.byte	0x2
	.byte	0xd0
	.4byte	0x326
	.byte	0x18
	.uleb128 0x3
	.4byte	.LASF45
	.byte	0x2
	.byte	0xd1
	.4byte	0x32c
	.byte	0x1c
	.byte	0
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0x2dc
	.uleb128 0x7
	.4byte	0x4a
	.uleb128 0x7
	.4byte	0x4a
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x2c8
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0x2f1
	.uleb128 0x7
	.4byte	0x4a
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x2e2
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0x30b
	.uleb128 0x7
	.4byte	0x51
	.uleb128 0x7
	.4byte	0x4a
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x2f7
	.uleb128 0x6
	.4byte	0x51
	.4byte	0x320
	.uleb128 0x7
	.4byte	0x51
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x311
	.uleb128 0x8
	.byte	0x4
	.4byte	0x58
	.uleb128 0x8
	.byte	0x4
	.4byte	0x90
	.uleb128 0xb
	.4byte	.LASF47
	.byte	0x2
	.byte	0xd2
	.4byte	0x25f
	.uleb128 0x9
	.4byte	0x332
	.uleb128 0xa
	.byte	0xc
	.byte	0x2
	.byte	0xd4
	.4byte	0x36f
	.uleb128 0x3
	.4byte	.LASF48
	.byte	0x2
	.byte	0xd5
	.4byte	0xb4
	.byte	0
	.uleb128 0x3
	.4byte	.LASF49
	.byte	0x2
	.byte	0xd6
	.4byte	0x36f
	.byte	0x4
	.uleb128 0x3
	.4byte	.LASF50
	.byte	0x2
	.byte	0xd7
	.4byte	0x375
	.byte	0x8
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x24e
	.uleb128 0x8
	.byte	0x4
	.4byte	0x33d
	.uleb128 0xb
	.4byte	.LASF51
	.byte	0x2
	.byte	0xd8
	.4byte	0x342
	.uleb128 0x9
	.4byte	0x37b
	.uleb128 0x2
	.4byte	.LASF53
	.byte	0x14
	.byte	0x2
	.byte	0xdc
	.4byte	0x3a4
	.uleb128 0x3
	.4byte	.LASF54
	.byte	0x2
	.byte	0xdd
	.4byte	0x3a4
	.byte	0
	.byte	0
	.uleb128 0xc
	.4byte	0x3b4
	.4byte	0x3b4
	.uleb128 0xd
	.4byte	0x83
	.byte	0x4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x386
	.uleb128 0xe
	.4byte	.LASF55
	.byte	0x2
	.2byte	0x106
	.4byte	0x38b
	.uleb128 0xe
	.4byte	.LASF56
	.byte	0x2
	.2byte	0x10d
	.4byte	0x386
	.uleb128 0xe
	.4byte	.LASF57
	.byte	0x2
	.2byte	0x110
	.4byte	0x33d
	.uleb128 0xe
	.4byte	.LASF58
	.byte	0x2
	.2byte	0x111
	.4byte	0x33d
	.uleb128 0xc
	.4byte	0x25a
	.4byte	0x3fa
	.uleb128 0xd
	.4byte	0x83
	.byte	0x7f
	.byte	0
	.uleb128 0x9
	.4byte	0x3ea
	.uleb128 0xe
	.4byte	.LASF59
	.byte	0x2
	.2byte	0x113
	.4byte	0x3fa
	.uleb128 0xc
	.4byte	0x7e
	.4byte	0x416
	.uleb128 0xf
	.byte	0
	.uleb128 0x9
	.4byte	0x40b
	.uleb128 0xe
	.4byte	.LASF60
	.byte	0x2
	.2byte	0x115
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF61
	.byte	0x2
	.2byte	0x116
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF62
	.byte	0x2
	.2byte	0x117
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF63
	.byte	0x2
	.2byte	0x118
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF64
	.byte	0x2
	.2byte	0x11a
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF65
	.byte	0x2
	.2byte	0x11b
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF66
	.byte	0x2
	.2byte	0x11c
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF67
	.byte	0x2
	.2byte	0x11d
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF68
	.byte	0x2
	.2byte	0x11e
	.4byte	0x416
	.uleb128 0xe
	.4byte	.LASF69
	.byte	0x2
	.2byte	0x11f
	.4byte	0x416
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0x4a2
	.uleb128 0x7
	.4byte	0x4a2
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x4ad
	.uleb128 0x10
	.4byte	.LASF104
	.uleb128 0x9
	.4byte	0x4a8
	.uleb128 0xe
	.4byte	.LASF70
	.byte	0x2
	.2byte	0x135
	.4byte	0x4be
	.uleb128 0x8
	.byte	0x4
	.4byte	0x493
	.uleb128 0x6
	.4byte	0x4a
	.4byte	0x4d3
	.uleb128 0x7
	.4byte	0x4d3
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x4a8
	.uleb128 0xe
	.4byte	.LASF71
	.byte	0x2
	.2byte	0x136
	.4byte	0x4e5
	.uleb128 0x8
	.byte	0x4
	.4byte	0x4c4
	.uleb128 0x5
	.byte	0x2
	.byte	0x7
	.4byte	.LASF72
	.uleb128 0x11
	.4byte	.LASF73
	.byte	0x2
	.2byte	0x14d
	.4byte	0x4fe
	.uleb128 0x8
	.byte	0x4
	.4byte	0x504
	.uleb128 0x6
	.4byte	0xb4
	.4byte	0x513
	.uleb128 0x7
	.4byte	0x4a
	.byte	0
	.uleb128 0x12
	.4byte	.LASF74
	.byte	0x8
	.byte	0x2
	.2byte	0x14f
	.4byte	0x53b
	.uleb128 0x13
	.4byte	.LASF75
	.byte	0x2
	.2byte	0x151
	.4byte	0x4f2
	.byte	0
	.uleb128 0x13
	.4byte	.LASF76
	.byte	0x2
	.2byte	0x152
	.4byte	0x53b
	.byte	0x4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x513
	.uleb128 0x11
	.4byte	.LASF77
	.byte	0x2
	.2byte	0x153
	.4byte	0x513
	.uleb128 0xe
	.4byte	.LASF78
	.byte	0x2
	.2byte	0x157
	.4byte	0x559
	.uleb128 0x8
	.byte	0x4
	.4byte	0x541
	.uleb128 0xb
	.4byte	.LASF79
	.byte	0x3
	.byte	0x37
	.4byte	0x83
	.uleb128 0x5
	.byte	0x1
	.byte	0x6
	.4byte	.LASF80
	.uleb128 0xb
	.4byte	.LASF81
	.byte	0x4
	.byte	0x30
	.4byte	0x253
	.uleb128 0x5
	.byte	0x2
	.byte	0x5
	.4byte	.LASF82
	.uleb128 0xb
	.4byte	.LASF83
	.byte	0x4
	.byte	0x36
	.4byte	0x4eb
	.uleb128 0x9
	.4byte	0x583
	.uleb128 0xb
	.4byte	.LASF84
	.byte	0x4
	.byte	0x38
	.4byte	0x83
	.uleb128 0x5
	.byte	0x8
	.byte	0x5
	.4byte	.LASF85
	.uleb128 0x5
	.byte	0x8
	.byte	0x7
	.4byte	.LASF86
	.uleb128 0x14
	.4byte	.LASF90
	.byte	0x1
	.2byte	0x145
	.4byte	0x55f
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x634
	.uleb128 0x15
	.ascii	"s\000"
	.byte	0x1
	.2byte	0x145
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x16
	.4byte	.LASF87
	.byte	0x1
	.2byte	0x145
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x17
	.ascii	"len\000"
	.byte	0x1
	.2byte	0x147
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x18
	.4byte	.LASF88
	.byte	0x1
	.2byte	0x148
	.4byte	0x593
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x18
	.4byte	.LASF89
	.byte	0x1
	.2byte	0x149
	.4byte	0x63a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x19
	.4byte	.LBB5
	.4byte	.LBE5-.LBB5
	.uleb128 0x17
	.ascii	"t\000"
	.byte	0x1
	.2byte	0x14c
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x17
	.ascii	"u\000"
	.byte	0x1
	.2byte	0x14c
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x58e
	.uleb128 0xc
	.4byte	0x77
	.4byte	0x64a
	.uleb128 0xd
	.4byte	0x83
	.byte	0x3
	.byte	0
	.uleb128 0x14
	.4byte	.LASF91
	.byte	0x1
	.2byte	0x12b
	.4byte	0x55f
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x6c3
	.uleb128 0x15
	.ascii	"s\000"
	.byte	0x1
	.2byte	0x12b
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x16
	.4byte	.LASF87
	.byte	0x1
	.2byte	0x12b
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x17
	.ascii	"len\000"
	.byte	0x1
	.2byte	0x12d
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x18
	.4byte	.LASF88
	.byte	0x1
	.2byte	0x12e
	.4byte	0x593
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x19
	.4byte	.LBB4
	.4byte	.LBE4-.LBB4
	.uleb128 0x17
	.ascii	"t\000"
	.byte	0x1
	.2byte	0x131
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x17
	.ascii	"u\000"
	.byte	0x1
	.2byte	0x131
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x14
	.4byte	.LASF92
	.byte	0x1
	.2byte	0x110
	.4byte	0x55f
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x74b
	.uleb128 0x15
	.ascii	"s\000"
	.byte	0x1
	.2byte	0x110
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x16
	.4byte	.LASF87
	.byte	0x1
	.2byte	0x110
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x17
	.ascii	"len\000"
	.byte	0x1
	.2byte	0x112
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x18
	.4byte	.LASF88
	.byte	0x1
	.2byte	0x113
	.4byte	0x593
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x18
	.4byte	.LASF89
	.byte	0x1
	.2byte	0x114
	.4byte	0x74b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x19
	.4byte	.LBB3
	.4byte	.LBE3-.LBB3
	.uleb128 0x17
	.ascii	"t\000"
	.byte	0x1
	.2byte	0x117
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x17
	.ascii	"u\000"
	.byte	0x1
	.2byte	0x117
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0xc
	.4byte	0x583
	.4byte	0x75b
	.uleb128 0xd
	.4byte	0x83
	.byte	0x1
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF93
	.byte	0x1
	.byte	0xf6
	.4byte	0x55f
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x7cd
	.uleb128 0x1b
	.ascii	"s\000"
	.byte	0x1
	.byte	0xf6
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x1c
	.4byte	.LASF87
	.byte	0x1
	.byte	0xf6
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x1d
	.ascii	"len\000"
	.byte	0x1
	.byte	0xf8
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1e
	.4byte	.LASF88
	.byte	0x1
	.byte	0xf9
	.4byte	0x593
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x19
	.4byte	.LBB2
	.4byte	.LBE2-.LBB2
	.uleb128 0x1d
	.ascii	"t\000"
	.byte	0x1
	.byte	0xfc
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x1d
	.ascii	"u\000"
	.byte	0x1
	.byte	0xfc
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.byte	0
	.uleb128 0x1f
	.4byte	.LASF94
	.byte	0x1
	.byte	0xcc
	.4byte	0x634
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x82b
	.uleb128 0x1b
	.ascii	"s\000"
	.byte	0x1
	.byte	0xcc
	.4byte	0x634
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1c
	.4byte	.LASF87
	.byte	0x1
	.byte	0xcc
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x1c
	.4byte	.LASF88
	.byte	0x1
	.byte	0xcc
	.4byte	0x82b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x1e
	.4byte	.LASF95
	.byte	0x1
	.byte	0xce
	.4byte	0x583
	.uleb128 0x2
	.byte	0x91
	.sleb128 -2
	.uleb128 0x1d
	.ascii	"low\000"
	.byte	0x1
	.byte	0xce
	.4byte	0x583
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x593
	.uleb128 0x1f
	.4byte	.LASF96
	.byte	0x1
	.byte	0xb2
	.4byte	0x55f
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x883
	.uleb128 0x1c
	.4byte	.LASF88
	.byte	0x1
	.byte	0xb2
	.4byte	0x593
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1c
	.4byte	.LASF89
	.byte	0x1
	.byte	0xb2
	.4byte	0x883
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x1d
	.ascii	"low\000"
	.byte	0x1
	.byte	0xb4
	.4byte	0x583
	.uleb128 0x2
	.byte	0x91
	.sleb128 -2
	.uleb128 0x1e
	.4byte	.LASF95
	.byte	0x1
	.byte	0xb4
	.4byte	0x583
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x583
	.uleb128 0x1f
	.4byte	.LASF97
	.byte	0x1
	.byte	0x4e
	.4byte	0xb4
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x927
	.uleb128 0x1b
	.ascii	"s\000"
	.byte	0x1
	.byte	0x4e
	.4byte	0xb4
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x1c
	.4byte	.LASF87
	.byte	0x1
	.byte	0x4e
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x1c
	.4byte	.LASF88
	.byte	0x1
	.byte	0x4e
	.4byte	0x82b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x1d
	.ascii	"b\000"
	.byte	0x1
	.byte	0x50
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -17
	.uleb128 0x1d
	.ascii	"c\000"
	.byte	0x1
	.byte	0x50
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -18
	.uleb128 0x1e
	.4byte	.LASF98
	.byte	0x1
	.byte	0x51
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -1
	.uleb128 0x1e
	.4byte	.LASF99
	.byte	0x1
	.byte	0x51
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -2
	.uleb128 0x1d
	.ascii	"i\000"
	.byte	0x1
	.byte	0x52
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x1e
	.4byte	.LASF100
	.byte	0x1
	.byte	0x52
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1d
	.ascii	"bad\000"
	.byte	0x1
	.byte	0x53
	.4byte	0x4a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x20
	.4byte	.LASF105
	.byte	0x1
	.byte	0xc
	.4byte	0x55f
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x1c
	.4byte	.LASF88
	.byte	0x1
	.byte	0xc
	.4byte	0x593
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1c
	.4byte	.LASF89
	.byte	0x1
	.byte	0xc
	.4byte	0x71
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x1d
	.ascii	"b\000"
	.byte	0x1
	.byte	0xe
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -1
	.uleb128 0x1d
	.ascii	"c\000"
	.byte	0x1
	.byte	0xf
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -2
	.uleb128 0x1d
	.ascii	"d\000"
	.byte	0x1
	.byte	0x10
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -3
	.uleb128 0x1d
	.ascii	"e\000"
	.byte	0x1
	.byte	0x11
	.4byte	0x571
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.uleb128 0x1d
	.ascii	"n\000"
	.byte	0x1
	.byte	0x12
	.4byte	0x55f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x21
	.4byte	.LASF106
	.byte	0x1
	.byte	0x43
	.4byte	.L5
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.uleb128 0x2134
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x21
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x13
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_pubnames,"",%progbits
	.4byte	0xa7
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x9a1
	.4byte	0x5ac
	.ascii	"utf16UTF8Count\000"
	.4byte	0x64a
	.ascii	"utf16RuneCount\000"
	.4byte	0x6c3
	.ascii	"utf8UTF16Count\000"
	.4byte	0x75b
	.ascii	"utf8RuneCount\000"
	.4byte	0x7cd
	.ascii	"utf16DecodeRune\000"
	.4byte	0x831
	.ascii	"utf16EncodeRune\000"
	.4byte	0x889
	.ascii	"utf8DecodeRune\000"
	.4byte	0x927
	.ascii	"utf8EncodeRune\000"
	.4byte	0
	.section	.debug_pubtypes,"",%progbits
	.4byte	0x198
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x9a1
	.4byte	0x4a
	.ascii	"int\000"
	.4byte	0x51
	.ascii	"long int\000"
	.4byte	0x25
	.ascii	"__mbstate_s\000"
	.4byte	0x77
	.ascii	"char\000"
	.4byte	0x83
	.ascii	"unsigned int\000"
	.4byte	0x243
	.ascii	"__RAL_locale_data_t\000"
	.4byte	0x253
	.ascii	"unsigned char\000"
	.4byte	0x332
	.ascii	"__RAL_locale_codeset_t\000"
	.4byte	0x37b
	.ascii	"__RAL_locale_t\000"
	.4byte	0x38b
	.ascii	"__locale_s\000"
	.4byte	0x4eb
	.ascii	"short unsigned int\000"
	.4byte	0x4f2
	.ascii	"__RAL_error_decoder_fn_t\000"
	.4byte	0x513
	.ascii	"__RAL_error_decoder_s\000"
	.4byte	0x541
	.ascii	"__RAL_error_decoder_t\000"
	.4byte	0x55f
	.ascii	"size_t\000"
	.4byte	0x56a
	.ascii	"signed char\000"
	.4byte	0x571
	.ascii	"uint8_t\000"
	.4byte	0x57c
	.ascii	"short int\000"
	.4byte	0x583
	.ascii	"uint16_t\000"
	.4byte	0x593
	.ascii	"uint32_t\000"
	.4byte	0x59e
	.ascii	"long long int\000"
	.4byte	0x5a5
	.ascii	"long long unsigned int\000"
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x54
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.4byte	0
	.4byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB0
	.4byte	.LFE0
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	.LFB2
	.4byte	.LFE2
	.4byte	.LFB3
	.4byte	.LFE3
	.4byte	.LFB4
	.4byte	.LFE4
	.4byte	.LFB5
	.4byte	.LFE5
	.4byte	.LFB6
	.4byte	.LFE6
	.4byte	.LFB7
	.4byte	.LFE7
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF52:
	.ascii	"__mbstate_s\000"
.LASF94:
	.ascii	"utf16DecodeRune\000"
.LASF41:
	.ascii	"__iswctype\000"
.LASF27:
	.ascii	"int_p_sign_posn\000"
.LASF78:
	.ascii	"__RAL_error_decoder_head\000"
.LASF92:
	.ascii	"utf8UTF16Count\000"
.LASF69:
	.ascii	"__RAL_data_empty_string\000"
.LASF26:
	.ascii	"int_n_sep_by_space\000"
.LASF82:
	.ascii	"short int\000"
.LASF53:
	.ascii	"__locale_s\000"
.LASF31:
	.ascii	"month_names\000"
.LASF1:
	.ascii	"__wchar\000"
.LASF61:
	.ascii	"__RAL_c_locale_abbrev_day_names\000"
.LASF20:
	.ascii	"n_sep_by_space\000"
.LASF40:
	.ascii	"__tolower\000"
.LASF22:
	.ascii	"n_sign_posn\000"
.LASF85:
	.ascii	"long long int\000"
.LASF35:
	.ascii	"time_format\000"
.LASF46:
	.ascii	"__RAL_locale_data_t\000"
.LASF58:
	.ascii	"__RAL_codeset_utf8\000"
.LASF62:
	.ascii	"__RAL_c_locale_month_names\000"
.LASF17:
	.ascii	"p_cs_precedes\000"
.LASF81:
	.ascii	"uint8_t\000"
.LASF102:
	.ascii	"C:\\Nordic SDK\\tes\\nRF5_SDK_15.2.0_9412b96\\exter"
	.ascii	"nal\\utf_converter\\utf.c\000"
.LASF28:
	.ascii	"int_n_sign_posn\000"
.LASF101:
	.ascii	"GNU C99 7.3.1 20180622 (release) [ARM/embedded-7-br"
	.ascii	"anch revision 261907] -fmessage-length=0 -mcpu=cort"
	.ascii	"ex-m4 -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-s"
	.ascii	"p-d16 -mthumb -mtp=soft -munaligned-access -std=gnu"
	.ascii	"99 -g2 -gpubnames -fomit-frame-pointer -fno-dwarf2-"
	.ascii	"cfi-asm -fno-builtin -ffunction-sections -fdata-sec"
	.ascii	"tions -fshort-enums -fno-common\000"
.LASF105:
	.ascii	"utf8EncodeRune\000"
.LASF91:
	.ascii	"utf16RuneCount\000"
.LASF64:
	.ascii	"__RAL_data_utf8_period\000"
.LASF25:
	.ascii	"int_p_sep_by_space\000"
.LASF74:
	.ascii	"__RAL_error_decoder_s\000"
.LASF77:
	.ascii	"__RAL_error_decoder_t\000"
.LASF10:
	.ascii	"mon_decimal_point\000"
.LASF5:
	.ascii	"decimal_point\000"
.LASF15:
	.ascii	"int_frac_digits\000"
.LASF50:
	.ascii	"codeset\000"
.LASF18:
	.ascii	"p_sep_by_space\000"
.LASF54:
	.ascii	"__category\000"
.LASF7:
	.ascii	"grouping\000"
.LASF39:
	.ascii	"__toupper\000"
.LASF2:
	.ascii	"long int\000"
.LASF95:
	.ascii	"high\000"
.LASF96:
	.ascii	"utf16EncodeRune\000"
.LASF43:
	.ascii	"__towlower\000"
.LASF87:
	.ascii	"nElem\000"
.LASF14:
	.ascii	"negative_sign\000"
.LASF75:
	.ascii	"decode\000"
.LASF48:
	.ascii	"name\000"
.LASF63:
	.ascii	"__RAL_c_locale_abbrev_month_names\000"
.LASF21:
	.ascii	"p_sign_posn\000"
.LASF73:
	.ascii	"__RAL_error_decoder_fn_t\000"
.LASF70:
	.ascii	"__user_set_time_of_day\000"
.LASF37:
	.ascii	"unsigned char\000"
.LASF56:
	.ascii	"__RAL_c_locale\000"
.LASF57:
	.ascii	"__RAL_codeset_ascii\000"
.LASF106:
	.ascii	"done\000"
.LASF9:
	.ascii	"currency_symbol\000"
.LASF90:
	.ascii	"utf16UTF8Count\000"
.LASF80:
	.ascii	"signed char\000"
.LASF86:
	.ascii	"long long unsigned int\000"
.LASF84:
	.ascii	"uint32_t\000"
.LASF12:
	.ascii	"mon_grouping\000"
.LASF4:
	.ascii	"unsigned int\000"
.LASF29:
	.ascii	"day_names\000"
.LASF100:
	.ascii	"expected\000"
.LASF33:
	.ascii	"am_pm_indicator\000"
.LASF42:
	.ascii	"__towupper\000"
.LASF8:
	.ascii	"int_curr_symbol\000"
.LASF67:
	.ascii	"__RAL_data_utf8_plus\000"
.LASF72:
	.ascii	"short unsigned int\000"
.LASF3:
	.ascii	"char\000"
.LASF83:
	.ascii	"uint16_t\000"
.LASF71:
	.ascii	"__user_get_time_of_day\000"
.LASF19:
	.ascii	"n_cs_precedes\000"
.LASF36:
	.ascii	"date_time_format\000"
.LASF79:
	.ascii	"size_t\000"
.LASF66:
	.ascii	"__RAL_data_utf8_space\000"
.LASF49:
	.ascii	"data\000"
.LASF16:
	.ascii	"frac_digits\000"
.LASF59:
	.ascii	"__RAL_ascii_ctype_map\000"
.LASF55:
	.ascii	"__RAL_global_locale\000"
.LASF99:
	.ascii	"highestAllowed\000"
.LASF89:
	.ascii	"encoded\000"
.LASF104:
	.ascii	"timeval\000"
.LASF60:
	.ascii	"__RAL_c_locale_day_names\000"
.LASF32:
	.ascii	"abbrev_month_names\000"
.LASF47:
	.ascii	"__RAL_locale_codeset_t\000"
.LASF93:
	.ascii	"utf8RuneCount\000"
.LASF13:
	.ascii	"positive_sign\000"
.LASF45:
	.ascii	"__mbtowc\000"
.LASF103:
	.ascii	"C:\\Nordic SDK\\tes\\nRF5_SDK_15.2.0_9412b96\\examp"
	.ascii	"les\\ble_peripheral\\ble_app_template\\pca10040\\s1"
	.ascii	"32\\arm5_no_packs\000"
.LASF98:
	.ascii	"lowestAllowed\000"
.LASF51:
	.ascii	"__RAL_locale_t\000"
.LASF34:
	.ascii	"date_format\000"
.LASF65:
	.ascii	"__RAL_data_utf8_comma\000"
.LASF11:
	.ascii	"mon_thousands_sep\000"
.LASF6:
	.ascii	"thousands_sep\000"
.LASF24:
	.ascii	"int_n_cs_precedes\000"
.LASF38:
	.ascii	"__isctype\000"
.LASF0:
	.ascii	"__state\000"
.LASF88:
	.ascii	"rune\000"
.LASF68:
	.ascii	"__RAL_data_utf8_minus\000"
.LASF23:
	.ascii	"int_p_cs_precedes\000"
.LASF30:
	.ascii	"abbrev_day_names\000"
.LASF44:
	.ascii	"__wctomb\000"
.LASF76:
	.ascii	"next\000"
.LASF97:
	.ascii	"utf8DecodeRune\000"
	.ident	"GCC: (GNU) 7.3.1 20180622 (release) [ARM/embedded-7-branch revision 261907]"
