#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "mode.h"
#include "3DGraph.h"
#include "AntiBug.h"

#include <cstring>

//Offset for fog coordinates calculation
const int kFogOffset = 3;

int FMSX;
int FMSX2;

word* fmap;
word* fmap1;

#define MaxShad 4528

byte fog[8192 + 1024];
byte wfog[8192];
byte yfog1[8192];
byte rfog[8192];

byte Optional1[8192];
byte Optional2[8192];
byte Optional3[8192];

byte darkfog[40960];
byte yfog[8192];
byte trans4[65536];
byte trans8[65536];
byte AlphaR[65536];
byte AlphaW[65536];
byte refl[3072];
byte WaterCost[65536];
byte GraySet[256];
byte Bright[8192];

void ClearFog()
{
	memset(fmap, 0, LX_fmap * LX_fmap * 2);
	memset(fmap1, 0, LX_fmap * LX_fmap * 2);
}

void LoadOptionalTable(int n, char* Name)
{
	ResFile F = RReset(Name);
	switch (n)
	{
	case 3:
		RBlockRead(F, Optional1, 8192);
		break;
	case 4:
		RBlockRead(F, Optional2, 8192);
		break;
	case 5:
		RBlockRead(F, Optional3, 8192);
		break;
	};
	RClose(F);
}

int mul3(int);

int FogMode;

void ShowSuperFog();

// Helper: ProcessFog blur kernel used by ProcessFog1_1, _2, _3
// Reads from src, writes to dst, using the given FMSX2_C stride.
// cols = (mlx & mask) - 1, rows = (mly & mask) - 1 for byte variants;
// for _3 variant, cols/rows use full 16-bit values.
static void ProcessFogBlur(byte* src, byte* dst, int cols, int rows,
                           int fDH, int fmsx2c)
{
	int offset = 2 + fmsx2c;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			word sum = *(word*)(src + offset - fmsx2c)
			         + *(word*)(src + offset + fmsx2c)
			         + *(word*)(src + offset - 2)
			         + *(word*)(src + offset + 2);
			word result = (word)((sum >> 2) - (sum >> 10));
			*(word*)(dst + offset) = result;
			offset += 2;
		}
		offset = offset - fDH + fmsx2c;
	}
}

#define FMSX_C 134
#define FMSX2_C (2*FMSX_C)

void ProcessFog1_1()
{
	memcpy(fmap1, fmap, LX_fmap * LX_fmap * 2);

	int mlx = (msx >> 2) + kFogOffset + kFogOffset;
	int mly = (msy >> 2) + kFogOffset + kFogOffset;
	int fDV = (mlx * mly) << 1;
	int fDH = (mlx << 1) - 2;

	byte z = 1;

	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm
	{
		push	esi
		push	edi
		mov		esi, fmap1
		mov		edi, fmap
		mov		ebx, fDV
		mov		ecx, mlx
		inc		ecx

		xor ebx, ebx
		mov		bl, byte ptr mlx
		mov		bh, byte ptr mly
		dec		bl
		dec		bh
		mov		ecx, 2 + FMSX2_C
		lab4 : mov		dx, [esi + ecx - FMSX2_C]
		add		dx, [esi + ecx + FMSX2_C]
		add		dx, [esi + ecx - 2]
		add		dx, [esi + ecx + 2]
		mov		ax, dx
		shr		ax, 2
		shr		dx, 10
		sub		ax, dx
		mov		word ptr[edi + ecx], ax
		add		ecx, 2
		dec		bl
		jnz		lab4
		sub		ecx, fDH
		add		ecx, FMSX2_C
		mov		bl, byte ptr mlx
		dec		bl
		dec		bh
		jnz		lab4
		pop		edi
		pop		esi
	}
	#else
	{
		int cols = (mlx & 0xFF) - 1;
		int rows = (mly & 0xFF) - 1;
		ProcessFogBlur((byte*)fmap1, (byte*)fmap, cols, rows, fDH, FMSX2_C);
	}
	#endif
}

#undef FMSX_C
#undef FMSX2_C

#define FMSX_C (134<<1)
#define FMSX2_C (2*FMSX_C)

void ProcessFog1_2()
{
	memcpy(fmap1, fmap, LX_fmap * LX_fmap * 2);
	int mlx = (msx >> 2) + kFogOffset + kFogOffset;
	int mly = (msy >> 2) + kFogOffset + kFogOffset;
	int fDV = (mlx * mly) << 1;
	int fDH = (mlx << 1) - 2;
	byte z = 1;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		mov		esi, fmap1
		mov		edi, fmap
		mov		ebx, fDV
		mov		ecx, mlx
		inc		ecx

		xor ebx, ebx
		mov		bl, byte ptr mlx
		mov		bh, byte ptr mly
		dec		bl
		dec		bh
		mov		ecx, 2 + FMSX2_C
		lab4 : mov		dx, [esi + ecx - FMSX2_C]
		add		dx, [esi + ecx + FMSX2_C]
		add		dx, [esi + ecx - 2]
		add		dx, [esi + ecx + 2]
		mov		ax, dx
		shr		ax, 2
		shr		dx, 10
		sub		ax, dx

		mov		word ptr[edi + ecx], ax
		add		ecx, 2
		dec		bl
		jnz		lab4
		sub		ecx, fDH
		add		ecx, FMSX2_C
		mov		bl, byte ptr mlx
		dec		bl
		dec		bh
		jnz		lab4

		pop		edi
		pop		esi
	};
	#else
	{
		int cols = (mlx & 0xFF) - 1;
		int rows = (mly & 0xFF) - 1;
		ProcessFogBlur((byte*)fmap1, (byte*)fmap, cols, rows, fDH, FMSX2_C);
	}
	#endif
};


#undef FMSX_C
#undef FMSX2_C

#define FMSX_C (134<<2)
#define FMSX2_C (2*FMSX_C)

void ProcessFog1_3()
{
	memcpy(fmap1, fmap, LX_fmap * LX_fmap * 2);
	int mlx = (msx >> 2) + kFogOffset + kFogOffset;
	int mly = (msy >> 2) + kFogOffset + kFogOffset;
	int fDV = (mlx * mly) << 1;
	int fDH = (mlx << 1) - 2;
	byte z = 1;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		push	esi
		push	edi
		mov		esi, fmap1
		mov		edi, fmap
		mov		ebx, fDV
		mov		ecx, mlx
		inc		ecx

		xor ebx, ebx
		mov		bx, word ptr mlx
		dec		bx
		dec		mly
		mov		ecx, 2 + FMSX2_C
		lab4 : mov		dx, [esi + ecx - FMSX2_C]
		add		dx, [esi + ecx + FMSX2_C]
		add		dx, [esi + ecx - 2]
		add		dx, [esi + ecx + 2]
		mov		ax, dx
		shr		ax, 2
		shr		dx, 10
		sub		ax, dx

		mov		word ptr[edi + ecx], ax
		add		ecx, 2
		dec		bx
		jnz		lab4
		sub		ecx, fDH
		add		ecx, FMSX2_C
		mov		bx, word ptr mlx
		dec		bx
		dec		mly
		jnz		lab4

		pop		edi
		pop		esi
	};
	#else
	{
		// _3 variant uses 16-bit counters (word), not byte
		int cols = (mlx & 0xFFFF) - 1;
		int rows = (mly & 0xFFFF) - 1;
		ProcessFogBlur((byte*)fmap1, (byte*)fmap, cols, rows, fDH, FMSX2_C);
	}
	#endif
};

void ProcessFog1()
{
	switch (ADDSH)
	{
	case 1:
		ProcessFog1_1();
		break;
	case 2:
		ProcessFog1_2();
		break;
	case 3:
		ProcessFog1_3();
		break;
	}
}

int FOGOFFS[1024];
void LoadBlobs();

__declspec(dllexport) int CurPalette = 0;
extern int CurrentCursorGP;
byte graysc[256];

__declspec(dllexport) void LoadFog(int set)
{
	CurrentCursorGP = set + 5;
	CurPalette = set;
	char cc[128];
	sprintf(cc, "%d\\agew_1d.grd", set);
	ResFile fx = RReset(cc);
	RBlockRead(fx, &fog[1024], 8192);
	memcpy(&fog[0], &fog[1024], 256);
	memcpy(&fog[256], &fog[1024], 256);
	memcpy(&fog[512], &fog[1024], 256);
	memcpy(&fog[512 + 256], &fog[1024], 256);
	RClose(fx);
	fx = RReset("0\\gray.set");
	RBlockRead(fx, &graysc, 256);
	RClose(fx);
	sprintf(cc, "%d\\orange.grd", set);
	fx = RReset(cc);
	RBlockRead(fx, &Optional1, 8192);
	RClose(fx);
	sprintf(cc, "%d\\agew_1w.grd", set);
	fx = RReset(cc);
	RBlockRead(fx, &wfog, 8192);
	RClose(fx);
	sprintf(cc, "%d\\agew_1br.grd", set);
	fx = RReset(cc);
	RBlockRead(fx, &Bright, 8192);
	RClose(fx);
	fx = RReset("ageb.grd");
	RBlockRead(fx, &rfog, 8192);
	RClose(fx);
	sprintf(cc, "%d\\agew_1r.grd", set);
	fx = RReset(cc);
	RBlockRead(fx, &yfog, 8192);
	RClose(fx);
	fx = RReset("0\\agew_1dr.grd");
	RBlockRead(fx, &darkfog, 40960);
	RClose(fx);
	sprintf(cc, "%d\\agew_tr.grd", set);
	fx = RReset(cc);
	RBlockRead(fx, &trans8, 65536);
	RClose(fx);
	sprintf(cc, "%d\\agew_tr4.grd", set);
	fx = RReset(cc);
	RBlockRead(fx, &trans4, 65536);
	RClose(fx);
	fx = RReset("wcost.grd");
	RBlockRead(fx, &WaterCost, 65536);
	RClose(fx);
	fx = RReset("refl.grd");
	RBlockRead(fx, &refl, 3072);
	RClose(fx);
	fx = RReset("AlphaYR.grd");
	RBlockRead(fx, &AlphaR, 65536);
	RClose(fx);
	fx = RReset("AlphaW.grd");
	RBlockRead(fx, &AlphaW, 65536);
	RClose(fx);
	fx = RReset("agew_1gc.pal");
	RBlockRead(fx, &GraySet, 256);
	RClose(fx);
	for (int i = 0; i < 1024; i++)
	{
		FOGOFFS[i] = i * FMSX;
	}
	LoadBlobs();
}

void SetupFog()
{
	for (int i = 0; i < 1024; i++)
	{
		FOGOFFS[i] = i * FMSX;
	}
}

#define Shifter 7

static byte BFog[64][64];

void SetScreenFog16x16()
{
	smaplx++;
	smaply++;
	intptr_t fofs = (intptr_t)(fmap) + ((((mapy - 1) << 8) + (mapx - 1)) << 1);
	int Saddy = (256 - smaplx) << 1;
	int Daddy = (64 - smaplx) << 1;
	word MinShad = (MaxShad - (32 << Shifter));
	//filling
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm
	{
		push	esi
		push	edi
		mov		cl, byte ptr smaplx
		mov		ch, byte ptr smaply
		mov		esi, fofs
		mov		edi, offset BFog + 64 + 2
		cld
		LP1 : lodsw
		cmp		ax, MaxShad
		jae		FullColor
		cmp		ax, MinShad
		jbe		AbsBlack
		sub		ax, MaxShad
		neg		ax
		shr		ax, Shifter
		jmp		asgn1
		AbsBlack :
		mov		ax, 32
			jmp		asgn1
			FullColor :
		xor ax, ax
			asgn1 : mov		ah, al
			mov[edi], ax
			mov[edi + 64], ax
			add		edi, 2
			dec		cl
			jnz		LP1
			mov		cl, byte ptr smaplx
			add		esi, Saddy
			add		edi, Daddy
			dec		ch
			jnz		LP1
			pop		edi
			pop		esi
	}
	#else
	{
		word* srcPtr = (word*)(intptr_t)fofs;
		byte* dstBase = &BFog[0][0] + 64 + 2;
		int colCount = smaplx & 0xFF;
		int rowCount = smaply & 0xFF;
		byte* dstPtr = dstBase;
		for (int row = 0; row < rowCount; row++) {
			for (int col = 0; col < colCount; col++) {
				word val = *srcPtr++;
				byte fogVal;
				if (val >= MaxShad) {
					fogVal = 0;
				} else if (val <= MinShad) {
					fogVal = 32;
				} else {
					fogVal = (byte)((MaxShad - val) >> Shifter);
				}
				// ah=al means both bytes are the same value
				dstPtr[0] = fogVal;
				dstPtr[1] = fogVal;
				// also write to next row (edi+64)
				dstPtr[64] = fogVal;
				dstPtr[64 + 1] = fogVal;
				dstPtr += 2;
			}
			srcPtr += (256 - smaplx); // Saddy is in bytes, srcPtr steps by words
			dstPtr += (64 - smaplx) * 2; // Daddy is in bytes
		}
	}
	#endif

	smaplx--;
	smaply--;
}

void ProcessScreenFog16x16()
{
	smaplx++;
	smaply++;
	int	ads = 64 - (smaplx << 1);
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm
	{
		push	esi
		mov		esi, offset BFog + 64 + 2
		mov		ch, byte ptr smaply
		shl		ch, 1
		gt0:	mov		cl, byte ptr smaplx
		gt1 : mov		ax, [esi - 1]
		add		ax, [esi + 1]
		add		ax, [esi - 64]
		add		ax, [esi + 64]
		shr		ax, 2
		and ax, 0x1F1F
		mov[esi], ax
		add		esi, 2
		dec		cl
		jnz		gt1
		add		esi, ads
		dec		ch
		jnz		gt0
		pop		esi
	}
	#else
	{
		byte* base = &BFog[0][0] + 64 + 2;
		int rowCount = (smaply & 0xFF) * 2;
		int colCount = smaplx & 0xFF;
		byte* ptr = base;
		for (int row = 0; row < rowCount; row++) {
			for (int col = 0; col < colCount; col++) {
				// Read 16-bit values at offsets -1, +1, -64, +64
				word v1 = *(word*)(ptr - 1);
				word v2 = *(word*)(ptr + 1);
				word v3 = *(word*)(ptr - 64);
				word v4 = *(word*)(ptr + 64);
				word avg = (word)(((v1 + v2 + v3 + v4) >> 2) & 0x1F1F);
				*(word*)ptr = avg;
				ptr += 2;
			}
			ptr += ads;
		}
	}
	#endif
	smaplx--;
	smaply--;
}

#define zmin 0
#define zmax 32

// Helper: apply darkfog lookup across a scanline of pixels.
// fogAcc is the fog accumulator (fixed-point 8.8 in upper bits),
// fogInc is the per-pixel fog increment.
// Pixels are 1-byte indexed color values.
// width is the number of pixels per scanline (e.g. 16 or 32).
static inline void ApplyDarkfogScanline(byte* scanline, int width, int& fogAcc, int fogInc)
{
	for (int i = 0; i < width; i++) {
		// fog level is in the upper bytes; mask to get the table page index
		int fogLevel = (fogAcc >> 8) & 0xFF00;
		// Clamp fogLevel to darkfog table bounds (40960 = 160*256)
		// The high byte selects the fog table page, the low byte is the pixel value
		scanline[i] = darkfog[fogLevel | scanline[i]];
		fogAcc += fogInc;
	}
}

// Helper: zero-fill a rectangular block of bytes
static inline void ZeroFillBlock(byte* dst, int width, int height, int pitch)
{
	for (int row = 0; row < height; row++) {
		memset(dst, 0, width);
		dst += pitch;
	}
}

void ShowSuperFluentFog32_160_16(int x, int y, int z1x, int z2x, int z3x, int z4x)
{
	if (MiniMode)
	{
		return;
	}

	int z1 = z1x * 65536;
	int	z2 = z2x * 65536;
	int z3 = z3x * 65536;
	int	z4 = z4x * 65536;

	intptr_t scrof = (intptr_t)(ScreenPtr) + x + y * SCRSizeX;
	int adds = SCRSizeX - 32;

	if (z1x <= 63 && z2x <= 63 && z3x <= 63 && z4x <= 63)
	{
		return;
	}

	if (z1x >= 96 && z2x >= 96 && z3x >= 96 && z4x >= 96)
	{
		#if defined(_MSC_VER) && defined(_M_IX86)
		__asm
		{
			push	edi
			mov		ebx, adds
			mov		edi, scrof
			mov		dl, 16
			xor eax, eax
			cld
			iug : mov		ecx, 8
			rep		stosd
			add		edi, ebx
			dec		dl
			jnz		iug
			pop		edi
		}
		#else
		{
			// Fill 32 bytes (8 dwords) per row, 16 rows, with zeros
			byte* dst = (byte*)(intptr_t)scrof;
			ZeroFillBlock(dst, 32, 16, SCRSizeX);
		}
		#endif
		return;
	}
	else
	{
		int a, b, p;
		int c = (z3 - z1) >> 4;
		int d = (z1 + z4 - z3 - z2) >> 9;
		#if defined(_MSC_VER) && defined(_M_IX86)
		__asm
		{
			push	edi
			push	esi
			pushf
			mov		eax, z1
			mov		ebx, eax
			mov		a, ebx
			mov		ecx, z2
			sub		ecx, eax
			sar		ecx, 5
			mov		b, ecx
			//coefficients are now calculated
			mov		ecx, 0x1004
			mov		ebx, a		//bh=fogging value
			mov		edx, b		//dx=fog incrementor
			xor eax, eax
			mov		esi, scrof
			mov		p, ebx
			qqw1 : mov		eax, ebx
			sar		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			sar		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			sar		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			sar		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			//jnz		qqw1
			add		esi, adds
			add		edx, d
			mov		ebx, p
			add		ebx, c
			//mov		cl,4
			mov		p, ebx
			dec		ch
			jnz		qqw1
			popf
			pop		esi
			pop		edi
		}
		#else
		{
			// Bilinear fog: 32 pixels wide, 16 rows
			// a = z1, b = (z2-z1)/32, c = (z3-z1)/16, d = (z1+z4-z3-z2)/512
			a = z1;
			b = (z2 - z1) >> 5;
			p = a;
			byte* scanline = (byte*)(intptr_t)scrof;
			int fogInc = b;
			for (int row = 0; row < 16; row++) {
				int fogAcc = p;
				ApplyDarkfogScanline(scanline, 32, fogAcc, fogInc);
				scanline += SCRSizeX;
				fogInc += d;
				p += c;
			}
		}
		#endif
	}
}

void ShowSuperFluentFog16_160(int x, int y, int z1x, int z2x, int z3x, int z4x)
{
	int z1 = z1x << 16;
	int	z2 = z2x << 16;
	int z3 = z3x << 16;
	int	z4 = z4x << 16;
	intptr_t scrof = (intptr_t)(ScreenPtr) + x + y * SCRSizeX;
	int adds = SCRSizeX - 16;

	if (z1x <= 63 && z2x <= 63 && z3x <= 63 && z4x <= 63)
	{
		return;
	}

	if (z1x >= 96 && z2x >= 96 && z3x >= 96 && z4x >= 96)
	{
		#if defined(_MSC_VER) && defined(_M_IX86)
		__asm
		{
			push	edi
			mov		ebx, adds
			mov		edi, scrof
			mov		dl, 16
			xor eax, eax
			cld
			iug : mov		ecx, 4
			rep		stosd
			add		edi, ebx
			dec		dl
			jnz		iug
			pop		edi
		}
		#else
		{
			// Fill 16 bytes (4 dwords) per row, 16 rows, with zeros
			byte* dst = (byte*)(intptr_t)scrof;
			ZeroFillBlock(dst, 16, 16, SCRSizeX);
		}
		#endif
		return;
	}
	else
	{
		int a, b, c, d, p;
		#if defined(_MSC_VER) && defined(_M_IX86)
		__asm
		{
			push	edi
			push	esi
			pushf
			mov		eax, z1
			mov		ebx, eax
			//shl		ebx,8
			mov		a, ebx
			mov		ecx, z2
			sub		ecx, eax
			sar		ecx, 4
			mov		b, ecx
			mov		ecx, z3
			sub		ecx, eax
			sar		ecx, 4
			mov		c, ecx
			mov		ecx, z4
			add		ecx, z1
			sub		ecx, z2
			sub		ecx, z3
			sar		ecx, 8
			mov		d, ecx
			//coefficients are now calculated
			mov		ecx, 0x1002
			mov		ebx, a		//bh=fogging value
			//add		ebx,256*256*3
			mov		edx, b		//dx=fog incrementor
			xor eax, eax
			mov		esi, scrof
			mov		p, ebx
			qqw1 : mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			//jnz		qqw1
			add		esi, adds
			add		edx, d
			mov		ebx, p
			add		ebx, c
			//mov		cl,4
			mov		p, ebx
			dec		ch
			jnz		qqw1
			popf
			pop		esi
			pop		edi
		}
		#else
		{
			// Bilinear fog: 16 pixels wide, 16 rows
			a = z1;
			b = (z2 - z1) >> 4;
			c = (z3 - z1) >> 4;
			d = (z1 + z4 - z3 - z2) >> 8;
			p = a;
			byte* scanline = (byte*)(intptr_t)scrof;
			int fogInc = b;
			for (int row = 0; row < 16; row++) {
				int fogAcc = p;
				ApplyDarkfogScanline(scanline, 16, fogAcc, fogInc);
				scanline += SCRSizeX;
				fogInc += d;
				p += c;
			}
		}
		#endif
	}
}

void ShowSuperFluentFog12_160(int x, int y, int z1x, int z2x, int z3x, int z4x)
{
	int z1 = z1x << 16;
	int	z2 = z2x << 16;
	int z3 = z3x << 16;
	int	z4 = z4x << 16;
	intptr_t scrof = (intptr_t)(ScreenPtr) + x + y * SCRSizeX;
	int adds = SCRSizeX - 16;
	if (z1x <= 63 && z2x <= 63 && z3x <= 63 && z4x <= 63)return;
	if (z1x >= 96 && z2x >= 96 && z3x >= 96 && z4x >= 96)
	{
		#if defined(_MSC_VER) && defined(_M_IX86)
		__asm {
			push	edi
			mov		ebx, adds
			mov		edi, scrof
			mov		dl, 12
			xor eax, eax
			cld
			iug : mov		ecx, 4
			rep		stosd
			add		edi, ebx
			dec		dl
			jnz		iug
			pop		edi
		};
		#else
		{
			// Fill 16 bytes (4 dwords) per row, 12 rows, with zeros
			byte* dst = (byte*)(intptr_t)scrof;
			ZeroFillBlock(dst, 16, 12, SCRSizeX);
		}
		#endif
		return;
	}
	else
	{
		int a, b;
		int c = div(z3 - z1, 12).quot;
		int d = div(z1 + z4 - z3 - z2, 192).quot;
		int p;
		#if defined(_MSC_VER) && defined(_M_IX86)
		__asm {
			push	edi
			push	esi
			pushf
			mov		eax, z1
			mov		ebx, eax
			//shl		ebx,8
			mov		a, ebx
			mov		ecx, z2
			sub		ecx, eax
			sar		ecx, 4
			mov		b, ecx
			/*mov		ecx,z3
			sub		ecx,eax
			sar		ecx,4
			mov		c,ecx
			mov		ecx,z4
			add		ecx,z1
			sub		ecx,z2
			sub		ecx,z3
			sar		ecx,8
			mov		d,ecx*/
			//coefficients are now calculated
			mov		ecx, 0x0C02
			mov		ebx, a		//bh=fogging value
			//add		ebx,256*256*3
			mov		edx, b		//dx=fog incrementor
			xor eax, eax
			mov		esi, scrof
			mov		p, ebx
			qqw1 : mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi]
			mov		al, [darkfog + eax]
			mov[esi], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 1]
			mov		al, [darkfog + eax]
			mov[esi + 1], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 2]
			mov		al, [darkfog + eax]
			mov[esi + 2], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 3]
			mov		al, [darkfog + eax]
			mov[esi + 3], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 4]
			mov		al, [darkfog + eax]
			mov[esi + 4], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 5]
			mov		al, [darkfog + eax]
			mov[esi + 5], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 6]
			mov		al, [darkfog + eax]
			mov[esi + 6], al
			//inc		esi
			add		ebx, edx
			mov		eax, ebx
			shr		eax, 8
			mov		al, [esi + 7]
			mov		al, [darkfog + eax]
			mov[esi + 7], al
			add		esi, 8
			add		ebx, edx
			//jnz		qqw1
			add		esi, adds
			add		edx, d
			mov		ebx, p
			add		ebx, c
			//mov		cl,4
			mov		p, ebx
			dec		ch
			jnz		qqw1
			popf
			pop		esi
			pop		edi
		};
		#else
		{
			// Bilinear fog: 16 pixels wide, 12 rows
			a = z1;
			b = (z2 - z1) >> 4;
			p = a;
			byte* scanline = (byte*)(intptr_t)scrof;
			int fogInc = b;
			for (int row = 0; row < 12; row++) {
				int fogAcc = p;
				ApplyDarkfogScanline(scanline, 16, fogAcc, fogInc);
				scanline += SCRSizeX;
				fogInc += d;
				p += c;
			}
		}
		#endif
	};
};
#define shf 300
#define fmin 1500
byte fden[8192];

void makeFden()
{
	for (int i = 0; i < 8192; i++)
	{
		if (i > shf)
			fden[i] = 158;
		else
			fden[i] = 5 + div(i * 153, shf).quot;
	}
}

int GetF(word k)
{
	if (k >= fmin)
	{
		return 5;
	}

	return fden[fmin - k];
}

void TurnFogOn()
{
	FogMode = 1;
}

void TurnFogOff()
{
	FogMode = 0;
}

void SetLightPoint(int x, int y)
{
	int xx1 = x >> 5;
	int yy1 = y >> 5;
	if (xx1 < 0)xx1 = 0;
	if (xx1 >= msx)xx1 = msx - 1;
	if (yy1 >= msy)yy1 = msy - 1;
	if (Mode3D)y -= THMap[xx1 + yy1 + (yy1 << MTHShift)] << 1;
	yy1 = y >> 5;
	if (yy1 < 0)yy1 = 0;
	if (yy1 >= msy)yy1 = msy - 1;
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm
	{
		xor eax, eax
		mov		al, byte ptr xx1
		mov		ah, byte ptr yy1
		shl		eax, 1
		mov		word ptr[fmap + eax], 16383;
	}
	#else
	{
		// Compose index from low bytes of xx1 and yy1:
		// al = xx1 & 0xFF, ah = yy1 & 0xFF, then shift left by 1
		// This gives a word offset into fmap
		int idx = ((xx1 & 0xFF) | ((yy1 & 0xFF) << 8));
		fmap[idx] = 16383;
	}
	#endif
}

void FogSpot(int x, int y);

int GetHeight(int x, int y);

void UnitLight(OneObject* OB)
{
	if (!OB->Ready)
	{
		return;
	}

	int xx = OB->RealX >> 4;
	int yy = OB->RealY >> 4;
	int rr = 200;
	int rr1 = rr - (rr >> 2);
	yy -= (GetHeight(xx, yy) << 1);
	NewMonster* NM = OB->newMons;
	switch (NM->VisionType)
	{
	case 0:
		//Default unit line of view
		FogSpot(xx, yy);
		break;
	case 1:
		FogSpot(xx + 128, yy);
		FogSpot(xx - 128, yy);
		FogSpot(xx, yy + 128);
		FogSpot(xx, yy - 128);
		break;
	case 2:
		FogSpot(xx + 128, yy + 128);
		FogSpot(xx - 128, yy + 128);
		FogSpot(xx + 128, yy - 128);
		FogSpot(xx - 128, yy - 128);
		break;
	case 3:
		FogSpot(xx + 2 * 128, yy);
		FogSpot(xx - 2 * 128, yy);
		FogSpot(xx, yy - 2 * 128);
		FogSpot(xx, yy + 2 * 128);
		FogSpot(xx + 128, yy + 128);
		FogSpot(xx - 128, yy + 128);
		FogSpot(xx + 128, yy - 128);
		FogSpot(xx - 128, yy - 128);
		break;
	case 4:
		FogSpot(xx + 3 * 128, yy);
		FogSpot(xx - 3 * 128, yy);
		FogSpot(xx, yy + 3 * 128);
		FogSpot(xx, yy - 3 * 128);
		FogSpot(xx + 2 * 128, yy + 2 * 128);
		FogSpot(xx - 2 * 128, yy + 2 * 128);
		FogSpot(xx + 2 * 128, yy - 2 * 128);
		FogSpot(xx - 2 * 128, yy - 2 * 128);
		break;
	case 5:
		FogSpot(xx + 4 * 128, yy);
		FogSpot(xx - 4 * 128, yy);
		FogSpot(xx, yy + 4 * 128);
		FogSpot(xx, yy - 4 * 128);
		FogSpot(xx + 3 * 128, yy + 3 * 128);
		FogSpot(xx - 3 * 128, yy + 3 * 128);
		FogSpot(xx + 3 * 128, yy - 3 * 128);
		FogSpot(xx - 3 * 128, yy - 3 * 128);
		break;
	case 6:
		FogSpot(xx + 5 * 128, yy);
		FogSpot(xx - 5 * 128, yy);
		FogSpot(xx, yy + 5 * 128);
		FogSpot(xx, yy - 5 * 128);
		FogSpot(xx + 4 * 128, yy + 4 * 128);
		FogSpot(xx - 4 * 128, yy + 4 * 128);
		FogSpot(xx + 4 * 128, yy - 4 * 128);
		FogSpot(xx - 4 * 128, yy - 4 * 128);
		break;
	case 7:
		FogSpot(xx + 6 * 128, yy);
		FogSpot(xx - 6 * 128, yy);
		FogSpot(xx, yy + 6 * 128);
		FogSpot(xx, yy - 6 * 128);
		FogSpot(xx + 4 * 128, yy + 4);
		FogSpot(xx - 4 * 128, yy + 4 * 128);
		FogSpot(xx + 4 * 128, yy - 4 * 128);
		FogSpot(xx - 4 * 128, yy - 4 * 128);
		break;
	case 8:
		//Ukrainian peasant line of view
		FogSpot(xx, yy);//BUGFIX: Make peasants visible immediately at game start
		FogSpot(xx + 7 * 128, yy);
		FogSpot(xx - 7 * 128, yy);
		FogSpot(xx, yy + 7 * 128);
		FogSpot(xx, yy - 7 * 128);
		FogSpot(xx + 5 * 128, yy + 3);
		FogSpot(xx - 5 * 128, yy + 5 * 128);
		FogSpot(xx + 5 * 128, yy - 5 * 128);
		FogSpot(xx - 5 * 128, yy - 5 * 128);
		break;
	}
}

//New fog of war
word WFSC[128 * 128];
int maxWFX;
int maxWFY;

void SetWF(int x, int y, word w)
{
	if (x<0 || y<0 || x>maxWFX || y>maxWFY)
	{
		return;
	}

	WFSC[x + (y * 128)] = w;
}

int maxFX, maxFY;

void CreateFogImage()
{
	maxWFX = smaplx;
	maxWFY = smaply;
	maxFX = (msx / 4) + kFogOffset;
	maxFY = (msy / 4) + kFogOffset;

	int mx0 = mapx / 4;
	int my0 = mapy / 4;
	int mx1 = mapx + smaplx;

	if (mx1 % 4)
	{
		mx1 = (mx1 / 4) + 1;
	}
	else
	{
		mx1 /= 4;
	}

	int my1 = mapy + smaply;

	if (my1 % 4)
	{
		my1 = (my1 / 4) + 1;
	}
	else
	{
		my1 /= 4;
	}

	word* FMAP = (word*)fmap;
	int fogof0 = (my0 + kFogOffset) * FMSX + mx0 + kFogOffset;

	for (int myy = my0; myy <= my1; myy++)
	{
		int fogof = fogof0;

		for (int mxx = mx0; mxx <= mx1; mxx++)
		{
			int mcx = (mxx * 4) - mapx;
			int mcy = (myy * 4) - mapy;

			int F[5][5];

			F[0][0] = FMAP[fogof];
			F[0][4] = FMAP[fogof + 1];
			F[4][0] = FMAP[fogof + FMSX];
			F[4][4] = FMAP[fogof + FMSX + 1];

			F[2][2] = (F[0][0] + F[4][4] + F[0][4] + F[4][0]) >> 2;
			F[0][2] = (F[0][0] + F[0][4]) / 2;
			F[2][0] = (F[0][0] + F[4][0]) / 2;
			F[2][4] = (F[0][4] + F[4][4]) / 2;
			F[4][2] = (F[4][0] + F[4][4]) / 2;

			F[0][1] = (F[0][0] + F[0][2]) / 2;
			F[0][3] = (F[0][2] + F[0][4]) / 2;
			F[1][0] = (F[0][0] + F[2][0]) / 2;
			F[1][1] = (F[0][0] + F[2][0] + F[0][2] + F[2][2]) / 4;
			F[1][2] = (F[0][2] + F[2][2]) / 2;
			F[1][3] = (F[0][2] + F[0][4] + F[2][2] + F[2][4]) / 4;
			F[1][4] = (F[0][4] + F[2][4]) / 2;

			F[2][1] = (F[2][0] + F[2][2]) / 2;
			F[2][3] = (F[2][2] + F[2][4]) / 2;

			F[3][0] = (F[2][0] + F[4][0]) / 2;
			F[3][1] = (F[2][0] + F[2][2] + F[4][0] + F[4][2]) / 4;
			F[3][2] = (F[2][2] + F[4][2]) / 2;
			F[3][3] = (F[2][2] + F[2][4] + F[4][2] + F[4][4]) / 4;
			F[3][4] = (F[2][4] + F[4][4]) / 2;

			F[4][1] = (F[4][0] + F[4][2]) / 2;
			F[4][3] = (F[4][2] + F[4][4]) / 2;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					int xx = mcx + i;
					int yy = mcy + j;

					SetWF(xx, yy, F[j][i]);
					SetWF(xx + 1, yy, F[j + 1][i]);
					SetWF(xx, yy + 1, F[j][i + 1]);
					SetWF(xx + 1, yy + 1, F[j + 1][i + 1]);
				}
			}
			fogof++;
		}
		fogof0 += FMSX;
	}
}

void DrawFog()
{
	for (int y = 0; y < smaply; y++)
	{
		for (int x = 0; x < smaplx; x++)
		{
			int xx = (x * 32) + smapx;
			int yy = (y * 16) + smapy;
			int sof = x + (y * 128);

			ShowSuperFluentFog32_160_16(
				xx, yy,
				GetF(WFSC[sof]),
				GetF(WFSC[sof + 1]),
				GetF(WFSC[sof + 128]),
				GetF(WFSC[sof + 129])
			);
		}
	}
}

void FogSpot(int x, int y)
{
	int fx = ((x + 64) / 128) + kFogOffset;
	int fy = ((y + 64) / 128) + kFogOffset;

	if (fx < 1)
	{
		fx = 1;
	}

	if (fy < 1)
	{
		fy = 1;
	}

	if (fx > maxFX)
	{
		fx = maxFX;
	}

	if (fy > maxFY)
	{
		fy = maxFY;
	}

	fmap[FOGOFFS[fy] + fx] = 8000;
}

//Checks if the given coordinates are concealed through fog of war
//Return value < 900 means 'fogged', everything above - 'visible'
//Beware: y coordinate must be multiplied by 2?
//Example: if (GetFog(x, y << 1) < 900 && FogMode) //true if fogged
word GetFog(int x, int y)
{
	int fx = ((x + 64) / 128) + kFogOffset;
	int fy = ((y + 64) / 128) + kFogOffset;

	if (fx < 1)
	{
		fx = 1;
	}

	if (fy < 1)
	{
		fy = 1;
	}

	if (fx > maxFX)
	{
		fx = maxFX;
	}

	if (fy > maxFY)
	{
		fy = maxFY;
	}

	return fmap[FOGOFFS[fy] + fx];
}

extern int MiniLx;
extern int MiniLy;
extern int MiniX, MiniY;

void DrawMiniFog()
{
	intptr_t sofs = (intptr_t)(ScreenPtr) + minix + miniy * ScrWidth;
	intptr_t fofs = (intptr_t)(fmap) + ((((MiniX >> 1) << (ADDSH - 1)) + kFogOffset + 1 + FMSX * (((MiniY >> 1) << (ADDSH - 1)) + kFogOffset + 1)) << 1);


	int MMSX = (MiniLx / 2);
	int MMSY = (MiniLy / 2);
	int addscr = ScrWidth + ScrWidth - MMSX - MMSX;
	int F_add = (FMSX2 << (ADDSH - 1)) - (MMSX << ADDSH);
	int DDDX = 1 << ADDSH;

	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm
	{
		push	esi
		push	edi
		pushf
		mov		esi, fofs
		mov		edi, sofs
		mov		edx, ScrWidth
		mov		ebx, DDDX
		lopx1 :
		mov		ecx, MMSX
			lopx3 : mov     ax, [esi]
			add		esi, ebx

			cmp		ax, 1300
			ja		lopx2
			mov		word ptr[edi], 0
			mov		word ptr[edi + edx], 0
			lopx2:	add		edi, 2
			dec		ecx
			jnz		lopx3
			add		esi, F_add
			add		edi, addscr
			dec		MMSY
			jnz		lopx1
			popf
			pop		edi
			pop		esi
	}
	#else
	{
		byte* fogPtr = (byte*)(intptr_t)fofs;
		byte* scrPtr = (byte*)(intptr_t)sofs;
		int scrPitch = ScrWidth;
		for (int row = 0; row < MMSY; row++) {
			byte* fp = fogPtr;
			byte* sp = scrPtr;
			for (int col = 0; col < MMSX; col++) {
				word fogVal = *(word*)fp;
				fp += DDDX;
				if (fogVal <= 1300) {
					*(word*)sp = 0;
					*(word*)(sp + scrPitch) = 0;
				}
				sp += 2;
			}
			fogPtr += F_add + MMSX * DDDX; // total fog row advance
			scrPtr += addscr + MMSX * 2;    // total screen row advance
		}
	}
	#endif
}
