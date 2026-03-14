//Masking 2 or 3 textures in triangle
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "multipl.h"
#include "fog.h"
#include "walls.h"
#include "Nature.h"
#include <time.h>
#include "Nucl.h"

#include "Megapolis.h"
#include "dialogs.h"
#include <assert.h>
#include <math.h>
#include "Masks.h"
#include <string.h>
byte ResultMask[MaskLx * 256];
extern byte trans4[65536];
extern byte trans8[65536];
void CopyMaskedBitmap64(byte* Bits, int x, int y, void* Mask) {
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		mov		esi, Mask
		mov		edi, Bits
		xor ebx, ebx
		mov		bl, byte ptr x
		mov		bh, byte ptr y
		mov		cl, [esi + 2]    //Number of strings
		add		esi, 4
		xor edx, edx     //ResulMask offset
		rlclop1 :
		mov		ch, [esi]    //number of RLE segments
			inc		esi
			or ch, ch
			jz		endrlclop
			rllpstrt :
		mov		ax, [esi]
			add		bl, al
			add		dl, al
			lpp1 : and bx, 0011111100111111b
			mov		al, [edi + ebx]
			inc		ebx
			mov[ResultMask + edx], al
			inc		edx
			dec		ah
			jnz		lpp1
			add		esi, 2
			dec		ch
			jnz		rllpstrt
			endrlclop :
		xor dl, dl
			inc		bh
			inc		dh
			mov		bl, byte ptr x
			dec		cl
			jnz		rlclop1
			popf
			pop		edi
			pop		esi
	};
	#else
	// Portable C equivalent of the RLE mask copy
	byte* src = (byte*)Mask;
	byte bx_lo = (byte)x;
	byte bx_hi = (byte)y;
	byte nstr = src[2]; // Number of strings (rows)
	src += 4;
	byte dh = 0; // destination row in ResultMask
	for (int row = 0; row < nstr; row++) {
		byte nsegs = *src++; // number of RLE segments
		byte dl = 0; // destination column
		byte bl = bx_lo;
		for (int s = 0; s < nsegs; s++) {
			byte skip = src[0];
			byte count = src[1];
			bl += skip;
			dl += skip;
			for (int p = 0; p < count; p++) {
				int bx_idx = ((bx_hi & 0x3F) << 8) | (bl & 0x3F);
				ResultMask[dh * 256 + dl] = Bits[bx_idx];
				bl++;
				dl++;
			}
			src += 2;
		}
		bx_hi++;
		dh++;
	}
	#endif
};
void CopyMaskedTransparentBitmap_8(byte* Bits, int x, int y, void* Mask) {
	byte nstr;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		mov		esi, Mask
		mov		edi, Bits
		xor ebx, ebx
		mov		bl, byte ptr x
		mov		bh, byte ptr y
		mov		cl, [esi + 2]    //Number of strings
		mov		nstr, cl
		add		esi, 4
		xor eax, eax
		xor edx, edx     //ResulMask offset
		rlclop1 :
		mov		ch, [esi]    //number of RLE segments
			inc		esi
			or ch, ch
			jz		endrlclop
			rllpstrt :
		mov		ax, [esi]
			add		bl, al
			add		dl, al
			mov		cl, ah
			lpp1 : and bx, 0011111100111111b
			mov		ah, [edi + ebx]
			mov		al, [ResultMask + edx]
			mov		al, [trans8 + eax]
			inc		ebx
			mov[ResultMask + edx], al
			inc		edx
			dec		cl
			jnz		lpp1
			add		esi, 2
			dec		ch
			jnz		rllpstrt
			endrlclop :
		xor dl, dl
			inc		bh
			inc		dh
			mov		bl, byte ptr x
			dec		nstr
			jnz		rlclop1
			popf
			pop		edi
			pop		esi
	};
	#else
	// Portable C: blend bitmap pixel (high byte) with existing ResultMask pixel (low byte) via trans8
	byte* src = (byte*)Mask;
	byte bx_lo = (byte)x;
	byte bx_hi = (byte)y;
	nstr = src[2];
	src += 4;
	byte dh_val = 0;
	for (int row = 0; row < nstr; row++) {
		byte nsegs = *src++;
		byte dl = 0;
		byte bl = bx_lo;
		for (int s = 0; s < nsegs; s++) {
			byte skip = src[0];
			byte count = src[1];
			bl += skip;
			dl += skip;
			for (int p = 0; p < count; p++) {
				int bx_idx = ((bx_hi & 0x3F) << 8) | (bl & 0x3F);
				byte bitmap_pixel = Bits[bx_idx]; // goes into ah
				byte result_pixel = ResultMask[dh_val * 256 + dl]; // goes into al
				// eax = (ah << 8) | al = (bitmap_pixel << 8) | result_pixel
				ResultMask[dh_val * 256 + dl] = trans8[(bitmap_pixel << 8) | result_pixel];
				bl++;
				dl++;
			}
			src += 2;
		}
		bx_hi++;
		dh_val++;
	}
	#endif
};
void CopyMaskedTransparentBitmap_4(byte* Bits, int x, int y, void* Mask) {
	byte nstr;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		mov		esi, Mask
		mov		edi, Bits
		xor ebx, ebx
		mov		bl, byte ptr x
		mov		bh, byte ptr y
		mov		cl, [esi + 2]    //Number of strings
		mov		nstr, cl
		add		esi, 4
		xor eax, eax
		xor edx, edx     //ResulMask offset
		rlclop1 :
		mov		ch, [esi]    //number of RLE segments
			inc		esi
			or ch, ch
			jz		endrlclop
			rllpstrt :
		mov		ax, [esi]
			add		bl, al
			add		dl, al
			mov		cl, ah
			lpp1 : and bx, 0011111100111111b
			mov		ah, [edi + ebx]
			mov		al, [ResultMask + edx]
			mov		al, [trans4 + eax]
			inc		ebx
			mov[ResultMask + edx], al
			inc		edx
			dec		cl
			jnz		lpp1
			add		esi, 2
			dec		ch
			jnz		rllpstrt
			endrlclop :
		xor dl, dl
			inc		bh
			inc		dh
			mov		bl, byte ptr x
			dec		nstr
			jnz		rlclop1
			popf
			pop		edi
			pop		esi
	};
	#else
	// Portable C: blend bitmap pixel (high byte) with existing ResultMask pixel (low byte) via trans4
	byte* src = (byte*)Mask;
	byte bx_lo = (byte)x;
	byte bx_hi = (byte)y;
	nstr = src[2];
	src += 4;
	byte dh_val = 0;
	for (int row = 0; row < nstr; row++) {
		byte nsegs = *src++;
		byte dl = 0;
		byte bl = bx_lo;
		for (int s = 0; s < nsegs; s++) {
			byte skip = src[0];
			byte count = src[1];
			bl += skip;
			dl += skip;
			for (int p = 0; p < count; p++) {
				int bx_idx = ((bx_hi & 0x3F) << 8) | (bl & 0x3F);
				byte bitmap_pixel = Bits[bx_idx]; // ah
				byte result_pixel = ResultMask[dh_val * 256 + dl]; // al
				// eax = (bitmap_pixel << 8) | result_pixel
				ResultMask[dh_val * 256 + dl] = trans4[(bitmap_pixel << 8) | result_pixel];
				bl++;
				dl++;
			}
			src += 2;
		}
		bx_hi++;
		dh_val++;
	}
	#endif
};
void CopyMaskedTransparentBitmap_12(byte* Bits, int x, int y, void* Mask) {
	byte nstr;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		mov		esi, Mask
		mov		edi, Bits
		xor ebx, ebx
		mov		bl, byte ptr x
		mov		bh, byte ptr y
		mov		cl, [esi + 2]    //Number of strings
		mov		nstr, cl
		add		esi, 4
		xor eax, eax
		xor edx, edx     //ResulMask offset
		rlclop1 :
		mov		ch, [esi]    //number of RLE segments
			inc		esi
			or ch, ch
			jz		endrlclop
			rllpstrt :
		mov		ax, [esi]
			add		bl, al
			add		dl, al
			mov		cl, ah
			lpp1 : and bx, 0011111100111111b
			mov		al, [edi + ebx]
			mov		ah, [ResultMask + edx]
			mov		al, [trans4 + eax]
			inc		ebx
			mov[ResultMask + edx], al
			inc		edx
			dec		cl
			jnz		lpp1
			add		esi, 2
			dec		ch
			jnz		rllpstrt
			endrlclop :
		xor dl, dl
			inc		bh
			inc		dh
			mov		bl, byte ptr x
			dec		nstr
			jnz		rlclop1
			popf
			pop		edi
			pop		esi
	};
	#else
	// Portable C: blend with swapped operand order compared to _4 and _8
	// Here: al = Bits[bx_idx] (bitmap pixel), ah = ResultMask[edx] (existing pixel)
	// eax = (result_pixel << 8) | bitmap_pixel -- note swapped vs _4/_8
	byte* src = (byte*)Mask;
	byte bx_lo = (byte)x;
	byte bx_hi = (byte)y;
	nstr = src[2];
	src += 4;
	byte dh_val = 0;
	for (int row = 0; row < nstr; row++) {
		byte nsegs = *src++;
		byte dl = 0;
		byte bl = bx_lo;
		for (int s = 0; s < nsegs; s++) {
			byte skip = src[0];
			byte count = src[1];
			bl += skip;
			dl += skip;
			for (int p = 0; p < count; p++) {
				int bx_idx = ((bx_hi & 0x3F) << 8) | (bl & 0x3F);
				byte bitmap_pixel = Bits[bx_idx]; // al
				byte result_pixel = ResultMask[dh_val * 256 + dl]; // ah
				// eax = (result_pixel << 8) | bitmap_pixel
				ResultMask[dh_val * 256 + dl] = trans4[(result_pixel << 8) | bitmap_pixel];
				bl++;
				dl++;
			}
			src += 2;
		}
		bx_hi++;
		dh_val++;
	}
	#endif
};
extern RLCTable SimpleMaskA;
extern RLCTable SimpleMaskB;
extern RLCTable SimpleMaskC;
extern RLCTable SimpleMaskD;
void CopyMaskedBitmap(byte* Bits, int x, int y, int MaskID) {
	CopyMaskedBitmap64(Bits, x, y, (void*)(SimpleMaskA->OfsTable[MaskID]));
	CopyMaskedTransparentBitmap_4(Bits, x, y, (void*)(SimpleMaskB->OfsTable[MaskID]));
	CopyMaskedTransparentBitmap_8(Bits, x, y, (void*)(SimpleMaskC->OfsTable[MaskID]));
	CopyMaskedTransparentBitmap_12(Bits, x, y, (void*)(SimpleMaskD->OfsTable[MaskID]));
};
//  0
//	|\
//	|  \
//	|    \
//	|    /
//	|  /
//	|/
//Creates triangle (Type1) with bitmap
void FastCreateMaskedBitmap64_1(byte* Bits, int x, int y) {
	int tmedi;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		mov		al, 64
		xor ebx, ebx
		mov		bl, byte ptr x
		mov		bh, byte ptr y
		and ebx, 0011111100111111b
		sub		al, bl       //Max x in trapezoid
		xor ecx, ecx
		mov		ah, 16//32
		mov		edi, offset ResultMask
		mov		dl, 2
		cld
		mov		tmedi, edi
		StartTrapezoid1 :
		and ebx, 0011111100111111b
			mov		esi, Bits
			add		esi, ebx
			cmp		dl, al
			ja		RightPart1
			mov		cl, dl
			shr		cl, 2
			rep		movsd
			mov		cl, dl
			and cl, 3
			rep     movsb
			add		tmedi, 256
			mov		edi, tmedi
			add		dl, 2
			inc		bh
			dec		ah
			jnz		StartTrapezoid1
			jmp		Trap2
			RightPart1 :
		mov		cl, al
			shr		cl, 2
			rep		movsd
			mov		cl, al
			and cl, 3
			rep		movsb
			sub		esi, 64
			mov		cl, dl
			sub		cl, al
			mov		dh, cl
			shr		cl, 2
			rep		movsd
			mov		cl, dh
			and cl, 3
			rep		movsb
			add		dl, 2
			add		tmedi, 256
			mov		edi, tmedi
			inc		bh
			dec		ah
			jnz		StartTrapezoid1
			Trap2 : sub		dl, 2
			mov		ah, 16
			DoTrapezoid2 :
			and ebx, 0011111100111111b
			mov		esi, Bits
			add		esi, ebx
			cmp		dl, al
			ja		RightPart2
			mov		cl, dl
			shr		cl, 2
			rep		movsd
			mov		cl, dl
			and cl, 3
			rep     movsb
			add		tmedi, 256
			mov		edi, tmedi
			sub		dl, 2
			inc		bh
			dec		ah
			jnz		DoTrapezoid2
			jmp		EndTrapezoid
			RightPart2 :
		mov		cl, al
			shr		cl, 2
			rep		movsd
			mov		cl, al
			and cl, 3
			rep		movsb
			sub		esi, 64
			mov		cl, dl
			sub		cl, al
			mov		dh, cl
			shr		cl, 2
			rep		movsd
			mov		cl, dh
			and cl, 3
			rep		movsb
			sub		dl, 2
			add		tmedi, 256
			mov		edi, tmedi
			inc		bh
			dec		ah
			jnz		DoTrapezoid2
			EndTrapezoid :
		popf
			pop		edi
			pop		esi
	};
	#else
	// Portable C: creates a left-pointing diamond shape in ResultMask
	// Two trapezoids: first expands width by 2 each row (16 rows),
	// then contracts width by 2 each row (16 rows).
	// When width exceeds remaining space to edge (64-bl), wraps source around.
	byte bl = (byte)x & 0x3F;
	byte bh_val = (byte)y & 0x3F;
	byte maxX = 64 - bl; // al = max pixels before source wraps
	byte width = 2; // dl starts at 2
	int dstRow = 0;

	// First trapezoid: expanding, 16 rows
	for (int i = 0; i < 16; i++) {
		byte srcY = bh_val & 0x3F;
		byte srcX = bl & 0x3F;
		int srcBase = (srcY << 8) | srcX;
		byte* dst = &ResultMask[dstRow * 256];
		if (width <= maxX) {
			// Simple copy: width bytes from Bits[srcBase]
			memcpy(dst, &Bits[srcBase], width);
		} else {
			// Copy maxX bytes, then wrap source back by 64 and copy remainder
			memcpy(dst, &Bits[srcBase], maxX);
			memcpy(dst + maxX, &Bits[srcBase + maxX - 64], width - maxX);
		}
		dstRow++;
		width += 2;
		bh_val++;
	}

	// Second trapezoid: contracting, 16 rows
	width -= 2;
	for (int i = 0; i < 16; i++) {
		byte srcY = bh_val & 0x3F;
		byte srcX = bl & 0x3F;
		int srcBase = (srcY << 8) | srcX;
		byte* dst = &ResultMask[dstRow * 256];
		if (width <= maxX) {
			memcpy(dst, &Bits[srcBase], width);
		} else {
			memcpy(dst, &Bits[srcBase], maxX);
			memcpy(dst + maxX, &Bits[srcBase + maxX - 64], width - maxX);
		}
		dstRow++;
		width -= 2;
		bh_val++;
	}
	#endif
};
//      /|
//    /  |
// 0/    |
//  \    |
//    \  |
//      \|
void FastCreateMaskedBitmap64_2(byte* Bits, int x, int y) {
	int tmedi;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		xor ebx, ebx
		mov		bl, byte ptr x
		mov		bh, byte ptr y
		add		bl, 32//64
		sub		bh, 16//31
		and ebx, 0011111100111111b
		mov		al, bl       //Max x in trapezoid
		xor ecx, ecx
		mov		ah, 16//32
		mov		edi, offset ResultMask
		add		edi, 32//64
		mov		dl, 2
		std
		mov		tmedi, edi
		StartTrapezoid1 :
		and ebx, 0011111100111111b
			mov		esi, Bits
			add		esi, ebx
			cmp		dl, al
			ja		RightPart1
			mov		cl, dl
			dec		esi
			dec		edi
			cmp		cl, 4
			jb		uue1
			shr		cl, 2
			sub		esi, 3
			sub		edi, 3
			rep		movsd
			add		esi, 3
			add		edi, 3
			uue1:
		mov		cl, dl
			and cl, 3
			rep     movsb
			add		tmedi, 256
			mov		edi, tmedi
			inc		bh
			add		dl, 2
			dec		ah
			jnz		StartTrapezoid1
			jmp		Trap2
			RightPart1 :
		mov		cl, al
			dec		esi
			dec		edi
			cmp		cl, 4
			jb		uu3
			sub		esi, 3
			sub		edi, 3
			shr		cl, 2
			rep		movsd
			add		esi, 3
			add		edi, 3
			uu3:
		mov		cl, al
			and cl, 3
			rep		movsb
			add		esi, 64
			mov		cl, dl
			sub		cl, al
			mov		dh, cl
			cmp		cl, 4
			jb		uu31
			shr		cl, 2
			sub		esi, 3
			sub		edi, 3
			rep		movsd
			add		esi, 3
			add		edi, 3
			uu31:
		mov		cl, dh
			and cl, 3
			rep		movsb
			add		dl, 2
			add		tmedi, 256
			mov		edi, tmedi
			inc		bh
			dec		ah
			jnz		StartTrapezoid1
			Trap2 : sub		dl, 2
			mov		ah, 16
			DoTrapezoid2 :
			and ebx, 0011111100111111b
			mov		esi, Bits
			add		esi, ebx
			cmp		dl, al
			ja		RightPart2
			mov		cl, dl
			dec		esi
			dec		edi
			cmp		cl, 4
			jb		uue1x
			shr		cl, 2
			sub		esi, 3
			sub		edi, 3
			rep		movsd
			add		esi, 3
			add		edi, 3
			uue1x:
		mov		cl, dl
			and cl, 3
			rep     movsb
			add		tmedi, 256
			mov		edi, tmedi
			inc		bh
			sub		dl, 2
			dec		ah
			jnz		DoTrapezoid2
			jmp		EndTrapezoid
			RightPart2 :
		mov		cl, al
			dec		esi
			dec		edi
			cmp		cl, 4
			jb		uu3x
			sub		esi, 3
			sub		edi, 3
			shr		cl, 2
			rep		movsd
			add		esi, 3
			add		edi, 3
			uu3x:
		mov		cl, al
			and cl, 3
			rep		movsb
			add		esi, 64
			mov		cl, dl
			sub		cl, al
			mov		dh, cl
			cmp		cl, 4
			jb		uu31x
			shr		cl, 2
			sub		esi, 3
			sub		edi, 3
			rep		movsd
			add		esi, 3
			add		edi, 3
			uu31x:
		mov		cl, dh
			and cl, 3
			rep		movsb
			sub		dl, 2
			add		tmedi, 256
			mov		edi, tmedi
			inc		bh
			dec		ah
			jnz		DoTrapezoid2
			EndTrapezoid :
		popf
			pop		edi
			pop		esi
	};
	#else
	// Portable C: creates a right-pointing diamond shape in ResultMask
	// The assembly copies bytes in reverse (STD) from source position leftward into
	// ResultMask starting at column 32. It's a mirror of FastCreateMaskedBitmap64_1.
	// bl = x+32, bh = y-16 (both masked to 6 bits)
	// Destination starts at ResultMask + 32 and copies leftward (descending addresses).
	byte bl = ((byte)x + 32) & 0x3F;
	byte bh_val = ((byte)y - 16) & 0x3F;
	byte maxX = bl; // al = bl after masking; max pixels before source wraps left
	byte width = 2;
	int dstRow = 0;
	int dstBase = 32; // edi starts at ResultMask + 32

	// First trapezoid: expanding, 16 rows
	for (int i = 0; i < 16; i++) {
		byte srcY = bh_val & 0x3F;
		byte srcX = bl & 0x3F;
		// The STD copy goes from Bits[srcBase] downward, writing to ResultMask[dstBase] downward
		// Effectively copies 'width' bytes ending at source position (srcY*256+srcX) and
		// ending at dest position (dstRow*256+dstBase), going leftward.
		// This means it copies bytes at positions (srcX-width+1..srcX) to (dstBase-width+1..dstBase)
		// But with wrapping when width > maxX (bl)
		int srcBaseAddr = (srcY << 8) | srcX;
		byte* dst = &ResultMask[dstRow * 256];
		if (width <= maxX) {
			// Simple reverse copy: width bytes ending at srcX, ending at dstBase
			for (int j = 0; j < width; j++) {
				dst[dstBase - 1 - j] = Bits[srcBaseAddr - 1 - j];
			}
		} else {
			// First copy maxX bytes from the current position leftward
			for (int j = 0; j < maxX; j++) {
				dst[dstBase - 1 - j] = Bits[srcBaseAddr - 1 - j];
			}
			// Then wrap: source advances by 64 (add esi,64 in asm means wrap to end of row)
			// After copying maxX bytes leftward, remaining = width - maxX
			// Source wraps: esi had gone to srcBaseAddr - maxX, then +64
			int wrappedSrc = srcBaseAddr - maxX + 64;
			for (int j = 0; j < (width - maxX); j++) {
				dst[dstBase - 1 - maxX - j] = Bits[wrappedSrc - 1 - j];
			}
		}
		dstRow++;
		width += 2;
		bh_val++;
	}

	// Second trapezoid: contracting, 16 rows
	width -= 2;
	for (int i = 0; i < 16; i++) {
		byte srcY = bh_val & 0x3F;
		byte srcX = bl & 0x3F;
		int srcBaseAddr = (srcY << 8) | srcX;
		byte* dst = &ResultMask[dstRow * 256];
		if (width <= maxX) {
			for (int j = 0; j < width; j++) {
				dst[dstBase - 1 - j] = Bits[srcBaseAddr - 1 - j];
			}
		} else {
			for (int j = 0; j < maxX; j++) {
				dst[dstBase - 1 - j] = Bits[srcBaseAddr - 1 - j];
			}
			int wrappedSrc = srcBaseAddr - maxX + 64;
			for (int j = 0; j < (width - maxX); j++) {
				dst[dstBase - 1 - maxX - j] = Bits[wrappedSrc - 1 - j];
			}
		}
		dstRow++;
		width -= 2;
		bh_val++;
	}
	#endif
};
int GetBmOfst(int i) {
	return ((i & 3) << 6) + ((i >> 2) << (8 + 6));
};
// 1(x0,y0)
//  |\
//  |  \ s1
//s3|    \ 2
//  |    /
//  |  / s2
//  |/
// 3
//
// bm1,bm2,bm3-numbers of bitmaps in BitmapArray
// if bm1<bm2 then bm2 is over bm1
// s1 (0..2) - section of 1-2
// s2 (0..2) - section of 2-3
// s3 (0..2) - section of 3-1
/*void PrepareIntersection1(int bm1,int bm2,int bm3,
						  int x0,int y0,
						  int s1,int s2,int s3,
						  RLCTable Masks,byte* BitmapsArray){
	if(bm1==bm2){
		if(bm3<bm2){
			//1,2 over 3 - inverse mask
			FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm1),x0,y0);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[27+(2-s2)*3+s3]));
			return;
		}else{
			//3 over 1,2 - normal mask
			FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm1),x0,y0);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[18+(2-s2)*3+s3]));
			return;
		};
	};
	if(bm2==bm3){
		if(bm1<bm3){
			//2,3 over 1 - inverse mask
			FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm3),x0,y0);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
				(void*)(Masks->OfsTable[45+(2-s3)*3+s1]));
			return;
		}else{
			//1 over 2,3 - normal mask
			FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm3),x0,y0);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
				(void*)(Masks->OfsTable[36+(2-s3)*3+s1]));
			return;
		};
	};
	if(bm1==bm3){
		if(bm2<bm3){
			//1,3 over 2 - inverse mask
			FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm3),x0,y0);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
				(void*)(Masks->OfsTable[9+(2-s1)*3+s2]));
			return;
		}else{
			//2 over 1,3 - normal mask
			FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm3),x0,y0);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
				(void*)(Masks->OfsTable[(2-s1)*3+s2]));
			return;
		};
	};
	//Triple intersections
	if(bm1<bm2&&bm1<bm3){
		FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm1),x0,y0);
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
				(void*)(Masks->OfsTable[(2-s1)*3+s2]));
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[18+(2-s2)*3+s3]));
		return;
	};
	if(bm2<bm1&&bm2<bm3){
		FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm2),x0,y0);
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
				(void*)(Masks->OfsTable[36+(2-s3)*3+s1]));
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[18+(2-s2)*3+s3]));
		return;
	};
	FastCreateMaskedBitmap64_1(BitmapsArray+GetBmOfst(bm3),x0,y0);
	CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
			(void*)(Masks->OfsTable[36+(2-s3)*3+s1]));
	CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
			(void*)(Masks->OfsTable[(2-s1)*3+s2]));
};
//
//                1
//         s1   /|
//            /  |
//        2 /    | s3
// (x01,y0) \    |
//            \  |
//         s2   \|
//                3
void PrepareIntersection2(int bm1,int bm2,int bm3,
						  int x0,int y01,
						  int s1,int s2,int s3,
						  RLCTable Masks,byte* BitmapsArray){
	int y0=y01-31;
	if(bm1==bm2){
		if(bm3<bm2){
			//1,2 over 3 - inverse mask
			FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm1),x0,y01);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[54+27+(2-s2)*3+s3]));
			return;
		}else{
			//3 over 1,2 - normal mask
			FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm1),x0,y01);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[54+18+(2-s2)*3+s3]));
			return;
		};
	};
	if(bm2==bm3){
		if(bm1<bm3){
			//2,3 over 1 - inverse mask
			FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm3),x0,y01);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
				(void*)(Masks->OfsTable[54+45+(2-s3)*3+s1]));
			return;
		}else{
			//1 over 2,3 - normal mask
			FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm3),x0,y01);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
				(void*)(Masks->OfsTable[54+36+(2-s3)*3+s1]));
			return;
		};
	};
	if(bm1==bm3){
		if(bm2<bm3){
			//1,3 over 2 - inverse mask
			FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm3),x0,y01);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
				(void*)(Masks->OfsTable[54+9+(2-s1)*3+s2]));
			return;
		}else{
			//2 over 1,3 - normal mask
			FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm3),x0,y01);
			CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
				(void*)(Masks->OfsTable[54+(2-s1)*3+s2]));
			return;
		};
	};
	//Triple intersections
	if(bm1<bm2&&bm1<bm3){
		FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm1),x0,y01);
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
				(void*)(Masks->OfsTable[54+(2-s1)*3+s2]));
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[54+18+(2-s2)*3+s3]));
		return;
	};
	if(bm2<bm1&&bm2<bm3){
		FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm2),x0,y01);
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
				(void*)(Masks->OfsTable[54+36+(2-s3)*3+s1]));
		CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm3),x0,y0,
				(void*)(Masks->OfsTable[54+18+(2-s2)*3+s3]));
		return;
	};
	FastCreateMaskedBitmap64_2(BitmapsArray+GetBmOfst(bm3),x0,y01);
	CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm1),x0,y0,
			(void*)(Masks->OfsTable[54+36+(2-s3)*3+s1]));
	CopyMaskedBitmap64(BitmapsArray+GetBmOfst(bm2),x0,y0,
			(void*)(Masks->OfsTable[54+(2-s1)*3+s2]));
};*/
// 1(x0,y0)
//  |\
//  |  \ s1
//s3|    \ 2
//  |    /
//  |  / s2
//  |/
// 3
//
// bm1,bm2,bm3-numbers of bitmaps in BitmapArray
// if bm1<bm2 then bm2 is over bm1
// s1 (0..2) - section of 1-2
// s2 (0..2) - section of 2-3
// s3 (0..2) - section of 3-1
void PrepareIntersection1(int bm1, int bm2, int bm3,
	int x0, int y0,
	int s1, int s2, int s3,
	RLCTable Masks, byte* BitmapsArray) {
	if (bm1 == bm2) {
		if (bm3 < bm2) {
			//1,2 over 3 - inverse mask
			FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm1), x0, y0);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 27 + (2 - s2) * 3 + s3);
			return;
		}
		else {
			//3 over 1,2 - normal mask
			FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm1), x0, y0);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 18 + (2 - s2) * 3 + s3);
			return;
		};
	};
	if (bm2 == bm3) {
		if (bm1 < bm3) {
			//2,3 over 1 - inverse mask
			FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm3), x0, y0);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 45 + (2 - s3) * 3 + s1);
			return;
		}
		else {
			//1 over 2,3 - normal mask
			FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm3), x0, y0);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 36 + (2 - s3) * 3 + s1);
			return;
		};
	};
	if (bm1 == bm3) {
		if (bm2 < bm3) {
			//1,3 over 2 - inverse mask
			FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm3), x0, y0);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, 9 + (2 - s1) * 3 + s2);
			return;
		}
		else {
			//2 over 1,3 - normal mask
			FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm3), x0, y0);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, (2 - s1) * 3 + s2);
			return;
		};
	};
	//Triple intersections
	if (bm1 < bm2 && bm1 < bm3) {
		FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm1), x0, y0);
		CopyMaskedBitmap64(BitmapsArray + GetBmOfst(bm2), x0, y0,
			(void*)(Masks->OfsTable[(2 - s1) * 3 + s2]));
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 18 + (2 - s2) * 3 + s3);
		return;
	};
	if (bm2 < bm1 && bm2 < bm3) {
		FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm2), x0, y0);
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 36 + (2 - s3) * 3 + s1);
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 18 + (2 - s2) * 3 + s3);
		return;
	};
	FastCreateMaskedBitmap64_1(BitmapsArray + GetBmOfst(bm3), x0, y0);
	CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 36 + (2 - s3) * 3 + s1);
	CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, (2 - s1) * 3 + s2);
};
//
//                1
//         s1   /|
//            /  |
//        2 /    | s3
// (x01,y0) \    |
//            \  |
//         s2   \|
//                3
void PrepareIntersection2(int bm1, int bm2, int bm3,
	int x0, int y01,
	int s1, int s2, int s3,
	RLCTable Masks, byte* BitmapsArray) {
	int y0 = y01 - 31;
	if (bm1 == bm2) {
		if (bm3 < bm2) {
			//1,2 over 3 - inverse mask
			FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm1), x0, y01);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 54 + 27 + (2 - s2) * 3 + s3);
			return;
		}
		else {
			//3 over 1,2 - normal mask
			FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm1), x0, y01);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 54 + 18 + (2 - s2) * 3 + s3);
			return;
		};
	};
	if (bm2 == bm3) {
		if (bm1 < bm3) {
			//2,3 over 1 - inverse mask
			FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm3), x0, y01);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 54 + 45 + (2 - s3) * 3 + s1);
			return;
		}
		else {
			//1 over 2,3 - normal mask
			FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm3), x0, y01);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 54 + 36 + (2 - s3) * 3 + s1);
			return;
		};
	};
	if (bm1 == bm3) {
		if (bm2 < bm3) {
			//1,3 over 2 - inverse mask
			FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm3), x0, y01);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, 54 + 9 + (2 - s1) * 3 + s2);
			return;
		}
		else {
			//2 over 1,3 - normal mask
			FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm3), x0, y01);
			CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, 54 + (2 - s1) * 3 + s2);
			return;
		};
	};
	//Triple intersections
	if (bm1 < bm2 && bm1 < bm3) {
		FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm1), x0, y01);
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, 54 + (2 - s1) * 3 + s2);
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 54 + 18 + (2 - s2) * 3 + s3);
		return;
	};
	if (bm2 < bm1 && bm2 < bm3) {
		FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm2), x0, y01);
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 54 + 36 + (2 - s3) * 3 + s1);
		CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm3), x0, y0, 54 + 18 + (2 - s2) * 3 + s3);
		return;
	};
	FastCreateMaskedBitmap64_2(BitmapsArray + GetBmOfst(bm3), x0, y01);
	CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm1), x0, y0, 54 + 36 + (2 - s3) * 3 + s1);
	CopyMaskedBitmap(BitmapsArray + GetBmOfst(bm2), x0, y0, 54 + (2 - s1) * 3 + s2);
};
void ClearIntersectionBuffer() {
	memset(ResultMask, 0, sizeof(ResultMask));
};
void ShowIntersectionBuffer() {
	if (!bActive)return;
	int SCROF = int(ScreenPtr) + 256 * ScrWidth + 256;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		pushf
		mov		esi, offset ResultMask
		mov		edi, SCROF
		mov		edx, 64
		cld
		lpp1 : mov		ecx, 64
		rep		movsd
		add		edi, ScrWidth
		sub		edi, 256
		dec		edx
		jnz		lpp1
		popf
		pop		edi
		pop		esi
	};
	#else
	// Portable C: copy 64 rows of 256 bytes (64 dwords) from ResultMask to screen
	byte* src = ResultMask;
	byte* dst = (byte*)(intptr_t)SCROF;
	for (int row = 0; row < 64; row++) {
		memcpy(dst, src, 256); // 64 dwords = 256 bytes
		src += 256;
		dst += ScrWidth; // advance by screen width, net effect: +ScrWidth-256+256 = +ScrWidth
	}
	#endif
	//memset(ResultMask,0,sizeof(ResultMask));
};
