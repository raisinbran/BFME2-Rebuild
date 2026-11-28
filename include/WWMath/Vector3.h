// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Vector3.h: WWMath's three-float vector and the fast inverse square root.
// Target facts (computeApproachPosition 0x00589B7D): Vector3 is X +0, Y +4, Z +8;
// WWMath::Inv_Sqrt is 0x0004233A and takes its float in xmm0 (__fastcall).
#pragma once
#ifndef BFME2_VECTOR3_H
#define BFME2_VECTOR3_H

#include "Common/GameType.h"
#include "WWMath/wwinline.h"

class WWMath
{
public:
	static Real __fastcall Inv_Sqrt(Real value);		// 0x0004233A
	// A second, stack-argument copy (PhysicsBehavior::rva00390E61 0x00390E61 calls it).
	static Real __fastcall Inv_Sqrt_000423D3(Real value);			// 0x000423D3
	static Real Random_Float(void);					// 0x00711B90
	static Real Random_Float(Real min, Real max);			// 0x0006598A (inline in Zero Hour)
	static Real Sin(Real val);		// inline x87 (WWMath/wwmath.h); out-of-line copy 0x000659C5
	static Real Cos(Real val);		// inline x87 (WWMath/wwmath.h)
	static Real Acos(Real val);		// inline (WWMath/wwmath.h)
	static Real Sqrt(Real val);		// inline x87 (WWMath/wwmath.h); out-of-line copy 0x000659D4
	static Real Max(Real a, Real b);								// 0x00154A60 (comdat copy in render2dsentence)
	static Real Min(Real a, Real b);								// 0x00154A80
	// Zero Hour's text. Plain inline: mapper.cpp's Calculate_Texture_Matrix rows (0x001822E0) expand it, while dxwrapper.cpp's unit
	// emits the out-of-line copy 0x0011F680 that LightEnvironmentClass::Pre_Render_Update 0x00140A20 calls.
	static Real Clamp(Real val, Real min, Real max)
	{
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}
	static Real Floor(Real val);		// inline (WWMath/wwmath.h): retail calls the CRT floor with the float widened (mapper.cpp 0x001822E0)

	// Westwood's integer-mask fabs: retail clears the sign bit of the float's
	// stack copy (CollisionMath::Overlap_Test 0x0071AB30: `and dword [esp+8],7FFFFFFFh`).
	static Real Fabs(Real val)
	{
		int value = *(int *)&val;
		value &= 0x7FFFFFFF;
		return *(Real *)&value;
	}
	// Zero Hour's wwmath.h; CameraClass::Cull_Box 0x00135990 tests the sign bit of each plane normal component
	// (`shr 1Fh; not; test 1`).
	static WWINLINE bool Fast_Is_Float_Positive(const float &val)
	{
		return !((*(int *)(&val)) & 0x80000000);
	}
};

class Vector3
{
public:
	Vector3() {}
	Vector3(Real ax, Real ay, Real az) : X(ax), Y(ay), Z(az) {}
	// User-written copy: LightClass(const LightClass &) 0x00130C00 copies its
	// Vector3 members field by field through alternating registers, which the
	// implicit copy does not produce.
	Vector3(const Vector3 &v) { X = v.X; Y = v.Y; Z = v.Z; }
	__forceinline Vector3 &operator=(const Vector3 &v) { X = v.X; Y = v.Y; Z = v.Z; return *this; }

	__forceinline Real &operator[](int i) { return (&X)[i]; }
	__forceinline const Real &operator[](int i) const { return (&X)[i]; }
	void Set(Real x,Real y,Real z) { X=x;Y=y;Z=z; }
	// Westwood's WWMath forms (EA's vector3.h). TintEnvelope::setAttackFrames 0x0027192B
	// subtracts through the result pointer (SSE, stored) and then scales in place (x87).
	void Set(const Vector3 &that) { X = that.X; Y = that.Y; Z = that.Z; }
	// Zero Hour's Update_Min / Update_Max (the box helpers expand them).
	void Update_Min(const Vector3 &a) { if (a.X < X) X = a.X; if (a.Y < Y) Y = a.Y; if (a.Z < Z) Z = a.Z; }
	void Update_Max(const Vector3 &a) { if (a.X > X) X = a.X; if (a.Y > Y) Y = a.Y; if (a.Z > Z) Z = a.Z; }
	void Scale(const Vector3 &scale) { X *= scale.X; Y *= scale.Y; Z *= scale.Z; }
	static void Add(const Vector3 &a, const Vector3 &b, Vector3 *set_result)
	{
		set_result->X = a.X + b.X;
		set_result->Y = a.Y + b.Y;
		set_result->Z = a.Z + b.Z;
	}
	static void Subtract(const Vector3 &a, const Vector3 &b, Vector3 *set_result)
	{
		set_result->X = a.X - b.X;
		set_result->Y = a.Y - b.Y;
		set_result->Z = a.Z - b.Z;
	}
	__forceinline Real Length2() const { return X * X + Y * Y + Z * Z; }
	__forceinline Real Length() const { return WWMath::Sqrt(Length2()); }
	Real Quick_Length(void) const;
	__forceinline Vector3 &operator*=(Real k) { X *= k; Y *= k; Z *= k; return *this; }
	WWINLINE Vector3 &operator+=(const Vector3 &v) { X += v.X; Y += v.Y; Z += v.Z; return *this; }
	// Zero Hour's vector3.h line-intersection inlines (W3DView::calcCameraConstraints 0x00089A21 expands them).
	static __forceinline Real Find_X_At_Z(Real z, const Vector3 &p1, const Vector3 &p2) { return (p1.X + ((z - p1.Z) * ((p2.X - p1.X) / (p2.Z - p1.Z)))); }
	static __forceinline Real Find_Y_At_Z(Real z, const Vector3 &p1, const Vector3 &p2) { return (p1.Y + ((z - p1.Z) * ((p2.Y - p1.Y) / (p2.Z - p1.Z)))); }
	static __forceinline Real Dot_Product(const Vector3 &a, const Vector3 &b) { return a.X * b.X + a.Y * b.Y + a.Z * b.Z; }
	static void Cross_Product(const Vector3 &a, const Vector3 &b, Vector3 *result)
	{
		result->X = a.Y * b.Z - a.Z * b.Y;
		result->Y = a.Z * b.X - a.X * b.Z;
		result->Z = a.X * b.Y - a.Y * b.X;
	}
	// WWMath vector3.h (Zero Hour's text); retail's out-of-line copy is 0x000D46CE (W3DRoadBuffer's unit).
	static WWINLINE Real Cross_Product_Z(const Vector3 &a, const Vector3 &b) { return a.X * b.Y - a.Y * b.X; }
	// Zero Hour's vector3.h inline (below the operators): W3DRoadBuffer::findCrossTypeJoinVector 0x000DAE48 expands it. Retail's one
	// out-of-line copy, 0x000CFF7F in W3DStreakDraw's unit, has no caller in .text (an unreferenced COMDAT).
	static WWINLINE Real Distance(const Vector3 &a, const Vector3 &b);
	// Normalize through the stack-argument inverse square root (0x000423D3).
	__forceinline void Normalize_000423D3()
	{
		Real len2 = Length2();
		if (len2 != 0.0f)
		{
			Real oolen = WWMath::Inv_Sqrt_000423D3(len2);
			X *= oolen;
			Y *= oolen;
			Z *= oolen;
		}
	}
	__forceinline void Normalize()
	{
		Real len2 = Length2();
		if (len2 != 0.0f)
		{
			Real oolen = WWMath::Inv_Sqrt(len2);
			X *= oolen;
			Y *= oolen;
			Z *= oolen;
		}
	}

	// WWMath vector3.h's two-argument rotate (FireWeaponUpdate::update 0x0048BEE8 computes
	// sin then cos itself; this header stays free of <math.h>).
	__forceinline void Rotate_Z(Real angle);
	__forceinline void Rotate_Z(Real s_angle, Real c_angle)
	{
		Real tmp_x = X;
		Real tmp_y = Y;
		X = c_angle * tmp_x - s_angle * tmp_y;
		Y = s_angle * tmp_x + c_angle * tmp_y;
	}

	Real X;		// 0x0
	Real Y;		// 0x4
	Real Z;		// 0x8
};
// SIZE 0xC

inline Vector3 operator*(const Vector3 &a, Real k)
{
	return Vector3(a.X * k, a.Y * k, a.Z * k);
}

// Zero Hour's text: the scalar is the right operand here too (ParticleBufferClass::Get_New_Particles 0x001AB340
// schedules its position update only this way).
inline Vector3 operator*(Real k, const Vector3 &a)
{
	return Vector3((a.X * k), (a.Y * k), (a.Z * k));
}

__forceinline bool operator!=(const Vector3 &a, const Vector3 &b)
{
	return (a.X != b.X) || (a.Y != b.Y) || (a.Z != b.Z);
}

// Componentwise sum and difference (gridcull.cpp's box bounds, Center -/+ Extent).
__forceinline Vector3 operator+(const Vector3 &a, const Vector3 &b)
{
	return Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
}

// Zero Hour's unary minus (colmathplane.h negates the far corner with it).
WWINLINE Vector3 operator-(const Vector3 &a)
{
	return Vector3(-a.X, -a.Y, -a.Z);
}

__forceinline Vector3 operator-(const Vector3 &a, const Vector3 &b)
{
	return Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
}

WWINLINE Real Vector3::Distance(const Vector3 &a, const Vector3 &b)
{
	Vector3 temp;
	temp = a - b;
	return temp.Length();
}

// Componentwise equality (DynamicVectorClass<Vector3>::ID needs it: HLodClass's snap points).
inline bool operator==(const Vector3 &a, const Vector3 &b)
{
	return (a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z);
}

#endif

