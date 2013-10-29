// Scale2x project, GPLv2, (C) Andrea Mazzoleni 
// http://scale2x.sourceforge.net/
//
// this 'stripped down' version only does 16bpp at 2x scaling using mmx
// this file was created by aqrit


// Used internally.
#pragma warning( disable : 4799 ) // emms instruction is issued at end of scale2x()
__declspec(naked) void __cdecl scale2x_16_mmx_border( void* dst, void* src0, void* src1, void* src2, unsigned long count )
{
	__asm
	{
		PUSH ESI
		PUSH EDI
		MOV EDI, [ESP+0x0C] // dst
		MOV EAX, [ESP+0x10] // src0
		MOV ESI, [ESP+0x14] // src1
		MOV EDX, [ESP+0x18] // src2
		MOV ECX, [ESP+0x1C] // count

		/* always do the first and last run */
		SUB ECX, 0x10

	/* first run */
		/* set the current, current_pre, current_next registers */
		MOVQ MM0,QWORD PTR DS:[ESI]
		MOVQ MM7,QWORD PTR DS:[ESI]
		MOVQ MM1,QWORD PTR DS:[ESI+0x08]
		PSLLQ MM0,0x30
		PSLLQ MM1,0x30
		PSRLQ MM0,0x30
		MOVQ MM2,MM7
		MOVQ MM3,MM7
		PSLLQ MM2,0x10
		PSRLQ MM3,0x10
		POR MM0,MM2
		POR MM1,MM3
		
		/* current_upper */
		MOVQ MM6,QWORD PTR DS:[EAX]
		
		/* compute the upper-left pixel for dst on %%mm2 */
		/* compute the upper-right pixel for dst on %%mm4 */
		MOVQ MM2,MM0
		MOVQ MM4,MM1
		MOVQ MM3,MM0
		MOVQ MM5,MM1
		PCMPEQW MM2,MM6
		PCMPEQW MM4,MM6
		PCMPEQW MM3,QWORD PTR SS:[EDX]
		PCMPEQW MM5,QWORD PTR SS:[EDX]
		PANDN MM3,MM2
		PANDN MM5,MM4
		MOVQ MM2,MM0
		MOVQ MM4,MM1
		PCMPEQW MM2,MM1
		PCMPEQW MM4,MM0
		PANDN MM2,MM3
		PANDN MM4,MM5
		MOVQ MM3,MM2
		MOVQ MM5,MM4
		PAND MM2,MM6
		PAND MM4,MM6
		PANDN MM3,MM7
		PANDN MM5,MM7
		POR MM2,MM3
		POR MM4,MM5

		/* set *dst */
		MOVQ MM3,MM2
		PUNPCKLWD MM2,MM4
		PUNPCKHWD MM3,MM4
		MOVQ QWORD PTR DS:[EDI],MM2
		MOVQ QWORD PTR DS:[EDI+0x08],MM3

		/* next */
		ADD EAX,0x08
		ADD ESI,0x08
		ADD EDX,0x08
		ADD EDI,0x010
		SHR ECX,0x02
		JZ lbl_1

		align 0x10
	lbl_0: /* central runs */
		/* set the current, current_pre, current_next registers */
		MOVQ MM0,QWORD PTR DS:[ESI-0x08]
		MOVQ MM7,QWORD PTR DS:[ESI]
		MOVQ MM1,QWORD PTR DS:[ESI+0x08]
		PSRLQ MM0,0x30
		PSLLQ MM1,0x30
		MOVQ MM2,MM7
		MOVQ MM3,MM7
		PSLLQ MM2,0x10
		PSRLQ MM3,0x10
		POR MM0,MM2
		POR MM1,MM3

		/* current_upper */
		MOVQ MM6,QWORD PTR DS:[EAX]

		/* compute the upper-left pixel for dst on %%mm2 */
		/* compute the upper-right pixel for dst on %%mm4 */
		MOVQ MM2,MM0
		MOVQ MM4,MM1
		MOVQ MM3,MM0
		MOVQ MM5,MM1
		PCMPEQW MM2,MM6
		PCMPEQW MM4,MM6
		PCMPEQW MM3,QWORD PTR SS:[EDX]
		PCMPEQW MM5,QWORD PTR SS:[EDX]
		PANDN MM3,MM2
		PANDN MM5,MM4
		MOVQ MM2,MM0
		MOVQ MM4,MM1
		PCMPEQW MM2,MM1
		PCMPEQW MM4,MM0
		PANDN MM2,MM3
		PANDN MM4,MM5
		MOVQ MM3,MM2
		MOVQ MM5,MM4
		PAND MM2,MM6
		PAND MM4,MM6
		PANDN MM3,MM7
		PANDN MM5,MM7
		POR MM2,MM3
		POR MM4,MM5

		/* set *dst */
		MOVQ MM3,MM2
		PUNPCKLWD MM2,MM4
		PUNPCKHWD MM3,MM4
		MOVQ QWORD PTR DS:[EDI],MM2
		MOVQ QWORD PTR DS:[EDI+0x08],MM3

		/* next */
		ADD EAX,0x08
		ADD ESI,0x08
		ADD EDX,0x08
		ADD EDI,0x10
		DEC ECX
		JNZ lbl_0

		align 0x10
	lbl_1: /* final run */
		/* set the current, current_pre, current_next registers */
		MOVQ MM1,QWORD PTR DS:[ESI]
		MOVQ MM7,QWORD PTR DS:[ESI]
		MOVQ MM0,QWORD PTR DS:[ESI-0x08]
		PSRLQ MM1,0x30
		PSRLQ MM0,0x30
		PSLLQ MM1,0x30
		MOVQ MM2,MM7
		MOVQ MM3,MM7
		PSLLQ MM2,0x10
		PSRLQ MM3,0x10
		POR MM0,MM2
		POR MM1,MM3

		/* current_upper */
		MOVQ MM6,QWORD PTR DS:[EAX]

		/* compute the upper-left pixel for dst on %%mm2 */
		/* compute the upper-right pixel for dst on %%mm4 */
		MOVQ MM2,MM0
		MOVQ MM4,MM1
		MOVQ MM3,MM0
		MOVQ MM5,MM1
		PCMPEQW MM2,MM6
		PCMPEQW MM4,MM6
		PCMPEQW MM3,QWORD PTR SS:[EDX]
		PCMPEQW MM5,QWORD PTR SS:[EDX]
		PANDN MM3,MM2
		PANDN MM5,MM4
		MOVQ MM2,MM0
		MOVQ MM4,MM1
		PCMPEQW MM2,MM1
		PCMPEQW MM4,MM0
		PANDN MM2,MM3
		PANDN MM4,MM5
		MOVQ MM3,MM2
		MOVQ MM5,MM4
		PAND MM2,MM6
		PAND MM4,MM6
		PANDN MM3,MM7
		PANDN MM5,MM7
		POR MM2,MM3
		POR MM4,MM5
		
		/* set *dst */
		MOVQ MM3,MM2
		PUNPCKLWD MM2,MM4
		PUNPCKHWD MM3,MM4
		MOVQ QWORD PTR DS:[EDI],MM2
		MOVQ QWORD PTR DS:[EDI+0x08],MM3

		POP EDI
		POP ESI
		RETN
	}
}
#pragma warning( default : 4799 ) // restore

/**
 * Apply the Scale2x effect on a (16-bit) bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x2 times the size of the source bitmap.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
void scale2x( void* void_dst, unsigned long dst_pitch, void* void_src, unsigned long src_pitch, unsigned long width, unsigned long height )
{
	unsigned char* dst = (unsigned char*)void_dst;
	unsigned char* src0 = (unsigned char*)void_src;
	unsigned char* src1 = src0 + src_pitch;
	unsigned char* src2 = src1 + src_pitch;

	scale2x_16_mmx_border( dst, src0, src0, src1, width );
	dst += dst_pitch;
	scale2x_16_mmx_border( dst, src1, src0, src0, width );
	dst += dst_pitch;
	for( int count = height - 2; count != 0; count-- )
	{
		scale2x_16_mmx_border( dst, src0, src1, src2, width );
		dst += dst_pitch;
		scale2x_16_mmx_border( dst, src2, src1, src0, width );
		dst += dst_pitch;
		
		src0 = src1;
		src1 = src2;
		src2 += src_pitch;
	}
	scale2x_16_mmx_border( dst, src0, src1, src1, width );
	dst += dst_pitch;
	scale2x_16_mmx_border( dst, src1, src1, src0, width );
	__asm emms
}
