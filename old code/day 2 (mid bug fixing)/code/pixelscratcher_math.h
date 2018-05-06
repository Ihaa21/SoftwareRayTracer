#if !defined(PIXELSCRATCHER_MATH_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

// TODO: Remove
#include <cmath>

#define Square(a) ((a)*(a))
#define Min(a, b) (((a) > (b)) ? (b) : (a))
#define Max(a, b) (((a) < (b)) ? (b) : (a))

inline u32 RoundToNearest(u32 Nearest, u32 Value)
{
    u32 Result = 0;

    if (Nearest != 0)
    {
        Result = Value;
        u32 Remainder = Value % Nearest;
        if (Remainder != 0)
        {
            Result += Nearest - Remainder;
        }
    }

    return Result;
}

inline u32 Round(f32 Value)
{
    u32 Result = (u32)(Value + 0.5f);
    return Result;
}

inline i32 Abs(i32 A)
{
    i32 Result = A < 0 ? -A : A;
    return Result;
}

inline f32 SquareRoot(f32 A)
{
    f32 Result = sqrt(A);
    return Result;
}

struct v2
{
    f32 x, y;
};

struct v3
{
    union
    {
        struct
        {
            f32 x, y, z;        
        };
        
        struct
        {
            v2 xy;
            f32 z;
        };

        struct
        {
            v2 xz;
            f32 y;
        };

        struct
        {
            v2 yz;
            f32 x;
        };
    };    
};

struct v4
{
    union
    {
        struct
        {
            v3 xyz;
            f32 w;
        };
        
        struct
        {
            f32 x, y, z, w;
        };

        struct
        {
            f32 r, g, b, a;
        };
    };
};

inline v2 V2(f32 X, f32 Y)
{
    v2 Result;

    Result.x = X;
    Result.y = Y;
    
    return Result;
}

inline v2 V2i(i32 X, i32 Y)
{
    v2 Result;

    Result.x = (f32)X;
    Result.y = (f32)Y;
    
    return Result;
}

inline v3 V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    
    return Result;
}

inline v3 V3i(i32 X, i32 Y, i32 Z)
{
    v3 Result;

    Result.x = (f32)X;
    Result.y = (f32)Y;
    Result.z = (f32)Z;
    
    return Result;
}

inline v4 V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;
    
    return Result;
}

inline v4 V4i(i32 X, i32 Y, i32 Z, i32 W)
{
    v4 Result;

    Result.x = (f32)X;
    Result.y = (f32)Y;
    Result.z = (f32)Z;
    Result.w = (f32)W;
    
    return Result;
}

inline v2 operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return Result;
}

inline v2 operator-(v2 A)
{
    v2 Result = {};

    Result.x = -A.x;
    Result.y = -A.y;

    return Result;
}

inline v2 operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return Result;
}

inline v2 operator*(f32 B, v2 A)
{
    v2 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;

    return Result;
}

inline v2 operator*(v2 A, f32 B)
{
    v2 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;

    return Result;
}

inline v2 operator/(v2 A, f32 B)
{
    v2 Result;
    
    Result.x = A.x / B;
    Result.y = A.y / B;

    return Result;
}

inline v2 operator/(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    
    return Result;
}

inline v2& operator+=(v2& A, v2 B)
{
    A = A + B;

    return A;
}

inline v2& operator-=(v2& A, v2 B)
{
    A = A - B;

    return A;
}

inline v2& operator*=(v2& A, f32 B)
{
    A = A * B;

    return A;
}

inline v2& operator/=(v2& A, f32 B)
{
    A = A / B;
    
    return A;
}

inline v2& operator/=(v2& A, v2 B)
{
    A = A / B;
    
    return A;
}

inline v3 operator+(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return Result;
}

inline v3 operator-(v3 A)
{
    v3 Result = {};

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;

    return Result;
}

inline v3 operator-(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return Result;
}

inline v3 operator*(f32 B, v3 A)
{
    v3 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;

    return Result;
}

inline v3 operator*(v3 A, f32 B)
{
    v3 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;

    return Result;
}

inline v3 operator/(v3 A, f32 B)
{
    v3 Result;
    
    Result.x = A.x / B;
    Result.y = A.y / B;
    Result.z = A.z / B;

    return Result;
}

inline v3 operator/(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;
    
    return Result;
}

inline v3& operator+=(v3& A, v3 B)
{
    A = A + B;

    return A;
}

inline v3& operator-=(v3& A, v3 B)
{
    A = A - B;

    return A;
}

inline v3& operator*=(v3& A, f32 B)
{
    A = A * B;

    return A;
}

inline v3& operator/=(v3& A, f32 B)
{
    A = A / B;
    
    return A;
}

inline v3& operator/=(v3& A, v3 B)
{
    A = A / B;
    
    return A;
}

inline v4 operator+(v4 A, v4 B)
{
    v4 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;
    
    return Result;
}

inline v4 operator-(v4 A)
{
    v4 Result = {};

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    Result.w = -A.w;

    return Result;
}

inline v4 operator-(v4 A, v4 B)
{
    v4 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;
    
    return Result;
}

inline v4 operator*(f32 B, v4 A)
{
    v4 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;

    return Result;
}

inline v4 operator*(v4 A, f32 B)
{
    v4 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;

    return Result;
}

inline v4 operator/(v4 A, f32 B)
{
    v4 Result;
    
    Result.x = A.x / B;
    Result.y = A.y / B;
    Result.z = A.z / B;
    Result.w = A.w / B;

    return Result;
}

inline v4 operator/(v4 A, v4 B)
{
    v4 Result;
    
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;
    Result.w = A.w / B.w;
    
    return Result;
}

inline v4& operator+=(v4& A, v4 B)
{
    A = A + B;

    return A;
}

inline v4& operator-=(v4& A, v4 B)
{
    A = A - B;

    return A;
}

inline v4& operator*=(v4& A, f32 B)
{
    A = A * B;

    return A;
}

inline v4& operator/=(v4& A, f32 B)
{
    A = A / B;
    
    return A;
}

inline v4& operator/=(v4& A, v4 B)
{
    A = A / B;
    
    return A;
}

inline f32 LengthSq(v2 A)
{
    f32 Result = Square(A.x) + Square(A.y);
    return Result;
}

inline f32 Length(v2 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return Result;
}

inline f32 Inner(v2 A, v2 B)
{
    f32 Result = A.x*B.x + A.y*B.y;
    return Result;
}

inline v2 Normalize(v2 A)
{
    v2 Result = A;
    f32 VecLength = Length(A);
    Result /= VecLength;

    return Result;
}

inline f32 LengthSq(v3 A)
{
    f32 Result = Square(A.x) + Square(A.y) + Square(A.z);
    return Result;
}

inline f32 Length(v3 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return Result;
}

inline f32 Inner(v3 A, v3 B)
{
    f32 Result = Inner(A.xy, B.xy) + A.z*B.z;
    return Result;
}

inline v3 Normalize(v3 A)
{
    v3 Result = A;
    f32 VecLength = Length(A);
    Result /= VecLength;

    return Result;
}

inline f32 LengthSq(v4 A)
{
    f32 Result = Square(A.x) + Square(A.y) + Square(A.z) + Square(A.w);
    return Result;
}

inline f32 Length(v4 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return Result;
}

inline f32 Inner(v4 A, v4 B)
{
    f32 Result = Inner(A.xyz, B.xyz) + A.w*B.w;
    return Result;
}

inline v4 Normalize(v4 A)
{
    v4 Result = A;
    f32 VecLength = Length(A);
    Result /= VecLength;

    return Result;
}

struct rect2
{
    v2 Min;
    v2 Max;
};

struct rect3
{
    v3 Min;
    v3 Max;
};

inline rect2 RectMinMax(v2 Min, v2 Max)
{
    rect2 Result = {};

    Result.Min = Min;
    Result.Max = Max;

    return Result;
}

inline rect2 RectCenterDim(v2 Center, v2 Dim)
{
    rect2 Result = {};

    Result.Min = Center - Dim;
    Result.Max = Center + Dim;

    return Result;
}

inline rect3 RectMinMax(v3 Min, v3 Max)
{
    rect3 Result = {};

    Result.Min = Min;
    Result.Max = Max;

    return Result;
}

inline rect3 RectCenterDim(v3 Center, v3 Dim)
{
    rect3 Result = {};

    Result.Min = Center - Dim;
    Result.Max = Center + Dim;

    return Result;
}

inline rect2 Translate(rect2 Rect, v2 Displacement)
{
    rect2 Result = Rect;

    Result.Min += Displacement;
    Result.Max += Displacement;

    return Result;
}

inline rect3 Translate(rect3 Rect, v3 Displacement)
{
    rect3 Result = Rect;

    Result.Min += Displacement;
    Result.Max += Displacement;

    return Result;
}

struct ray
{
    v3 Origin;
    v3 Dir;
};

#if 0
// NOTE: Get reflection vector
IndirectRay.Dir = Ray.Dir - 2*Result.Normal*(Inner(Ray.Dir, Result.Normal));
#endif

struct plane
{
    v3 Normal;
    v3 Pos;
};

struct sphere
{
    v3 Center;
    f32 Radius;
};

#define PIXELSCRATCHER_MATH_H
#endif
