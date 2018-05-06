#include "pixelscratcher_math.h"
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

inline f32 ToDegree(f32 Radians)
{
    f32 Result = Radians * 180.0f / Pi32;
    return Result;
}

inline f32 ToRadians(f32 Degrees)
{
    f32 Result = Degrees * Pi32 / 180.0f;
    return Result;
}

inline f32 Abs(f32 A)
{
    f32 Result = A < 0 ? -A : A;
    return Result;
}

inline f32 SquareRoot(f32 A)
{
    f32 Result = sqrt(A);
    return Result;
}

inline f32 Sin(f32 A)
{
    f32 Result = sinf(A);
    return Result;
}

inline f32 ArcSin(f32 A)
{
    f32 Result = asinf(A);
    return Result;
}

inline f32 Cos(f32 A)
{
    f32 Result = cosf(A);
    return Result;
}

inline f32 ArcCos(f32 A)
{
    f32 Result = acosf(A);
    return Result;
}

inline f32 LinearInterop(f32 Start, f32 End, f32 t)
{
    f32 Result = Start*t + End*(1.0f - t);

    return Result;
}

inline f32 Pow(f32 Base, u32 Exp)
{
    f32 Result = Base;
    for (u32 ExpIndex = 0; ExpIndex < Exp; ++ExpIndex)
    {
        Result *= Base;
    }

    return Result;
}

//
// NOTE: Vectors 2, 3, 4
//

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

inline v4 V4(v3 V, f32 W)
{
    v4 Result = {};

    Result.xyz = V;
    Result.w = W;

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

inline f32 Length(v2 A)
{
    f32 Result = Square(A.x) + Square(A.y);
    return Result;
}

inline f32 LengthSq(v2 A)
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

    f32 VecLength = LengthSq(A);
    if (VecLength != 0.0f)
    {
        Result /= VecLength;
    }
    Assert(VecLength >= 0.0f);
    
    return Result;
}

inline v2 LinearInterop(v2 Start, v2 End, f32 t)
{
    v2 Result = Start*t + End*(1.0f - t);

    return Result;
}

inline f32 Length(v3 A)
{
    f32 Result = Square(A.x) + Square(A.y) + Square(A.z);
    return Result;
}

inline f32 LengthSq(v3 A)
{
    f32 Result = SquareRoot(Length(A));
    return Result;
}

inline v3 Normalize(v3 A)
{
    v3 Result = A;
    
    f32 VecLength = LengthSq(A);
    if (VecLength != 0.0f)
    {
        Result /= VecLength;
    }
    Assert(VecLength >= 0.0f);

    return Result;
}

inline f32 Inner(v3 A, v3 B)
{
    f32 Result = Inner(A.xy, B.xy) + A.z*B.z;
    return Result;
}

inline v3 Cross(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.y*B.z - A.z*B.y;
    Result.y = A.z*B.x - A.x*B.z;
    Result.z = A.x*B.y - A.y*B.x;

    return Result;
}

inline v3 GetReflection(v3 Normal, v3 Dir)
{
    v3 Result = Dir - 2*(Inner(Dir, Normal))*Normal;
    return Result;
}

inline v3 Lerp(v3 Start, v3 End, f32 t)
{
    v3 Result = Start*t + End*(1.0f - t);

    return Result;
}

inline f32 Length(v4 A)
{
    f32 Result = Square(A.x) + Square(A.y) + Square(A.z) + Square(A.w);
    return Result;
}

inline f32 LengthSq(v4 A)
{
    f32 Result = SquareRoot(Length(A));
    return Result;
}

inline v4 Normalize(v4 A)
{
    v4 Result = A;
        
    f32 VecLength = LengthSq(A);
    if (VecLength != 0.0f)
    {
        Result /= VecLength;
    }
    Assert(VecLength >= 0.0f);

    return Result;
}

inline f32 Inner(v4 A, v4 B)
{
    f32 Result = Inner(A.xyz, B.xyz) + A.w*B.w;
    return Result;
}

inline v4 LinearInterop(v4 Start, v4 End, f32 t)
{
    v4 Result = Start*t + End*(1.0f - t);

    return Result;
}

//
// NOTE: Rect 2 and 3
//

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

inline b32 RaySphereIntersect(ray Ray, sphere S, f32* tReturn)
{
    Ray.Dir = Normalize(Ray.Dir);
        
    v3 m = Ray.Origin - S.Center; 
    f32 b = Inner(m, Ray.Dir); 
    f32 c = Inner(m, m) - S.Radius * S.Radius; 

    // NOTE: Exit if the rays origin is outside the sphere (c > 0) and pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
    {
        return false;
    }

    // NOTE: A negative discriminant corresponds to ray missing the sphere
    f32 Discriminant = b*b - c; 
    if (Discriminant < 0.0f)
    {
        return false; 
    }
    
    *tReturn = Max(0.0f, -b - SquareRoot(Discriminant));
    
    return 1;
}

inline b32 RayPlaneIntersect(ray Ray, plane P, f32* tReturn)
{
    b32 Result = false;

    f32 Denominator = Inner(Ray.Dir, P.Normal);
    if (Denominator != 0.0f)
    {
        f32 t = Inner((P.Pos - Ray.Origin), P.Normal) / Denominator;
        if (t >= 0.0f)
        {
            *tReturn = t;
            Result = true;
        }
    }
    
    return Result;
}

inline b32 RayRectIntersect(ray Ray, rect3 B, f32* tReturn)
{
    b32 Result = false;
    
    f32 MinT = F32_MIN;
    f32 MaxT = F32_MAX;

    if (Ray.Dir.x != 0.0f)
    {
        f32 t0 = (B.Min.x - Ray.Origin.x) / Ray.Dir.x;
        f32 t1 = (B.Max.x - Ray.Origin.x) / Ray.Dir.x;

        MinT = Max(MinT, Min(t0, t1));
        MaxT = Min(MaxT, Max(t0, t1));
    }

    if (Ray.Dir.y != 0.0f)
    {
        f32 t0 = (B.Min.y - Ray.Origin.y) / Ray.Dir.y;
        f32 t1 = (B.Max.y - Ray.Origin.y) / Ray.Dir.y;

        MinT = Max(MinT, Min(t0, t1));
        MaxT = Min(MaxT, Max(t0, t1));
    }

    if (Ray.Dir.z != 0.0f)
    {
        f32 t0 = (B.Min.z - Ray.Origin.z) / Ray.Dir.z;
        f32 t1 = (B.Max.z - Ray.Origin.z) / Ray.Dir.z;

        MinT = Max(MinT, Min(t0, t1));
        MaxT = Min(MaxT, Max(t0, t1));
    }    

    Result = MaxT >= MinT;
    if (Result)
    {
        if (MinT < 0.0f)
        {
            *tReturn = MaxT;
        }
        else
        {
            *tReturn = MinT;
        }
    }
        
    return Result;
}

//
// NOTE: Matrix 4
//

inline v4 operator*(m4 A, v4 B)
{
    v4 Result = {};
    Result = B.x*A.v[0] + B.y*A.v[1] + B.z*A.v[2] + B.w*A.v[3];

    return Result;
}

inline v4 operator*(v4 A, m4 B)
{
    v4 Result = B*A;
    
    return Result;
}

inline m4 operator*(m4 A, m4 B)
{
    m4 Result = {};

    Result.v[0] = A*B.v[0];
    Result.v[1] = A*B.v[1];
    Result.v[2] = A*B.v[2];
    Result.v[3] = A*B.v[3];
    
    return Result;
}

inline m4 Identity()
{
    m4 Result = {};
    Result.v[0].x = 1.0f;
    Result.v[1].y = 1.0f;
    Result.v[2].z = 1.0f;
    Result.v[3].w = 1.0f;

    return Result;
}

inline m4 Transpose(m4 A)
{
    m4 Result = {};

    Result.v[0] = V4(A.v[0].x, A.v[1].x, A.v[2].x, A.v[3].x);
    Result.v[1] = V4(A.v[0].y, A.v[1].y, A.v[2].y, A.v[3].y);
    Result.v[2] = V4(A.v[0].z, A.v[1].z, A.v[2].z, A.v[3].z);
    Result.v[3] = V4(A.v[0].w, A.v[1].w, A.v[2].w, A.v[3].w);

    return Result;
}

inline m4 Inverse(m4 A)
{
    m4 Result = {};
    f32 Inv[16];

    Inv[0] = A.e[5]  * A.e[10] * A.e[15] - 
             A.e[5]  * A.e[11] * A.e[14] - 
             A.e[9]  * A.e[6]  * A.e[15] + 
             A.e[9]  * A.e[7]  * A.e[14] +
             A.e[13] * A.e[6]  * A.e[11] - 
             A.e[13] * A.e[7]  * A.e[10];

    Inv[4] = -A.e[4]  * A.e[10] * A.e[15] + 
              A.e[4]  * A.e[11] * A.e[14] + 
              A.e[8]  * A.e[6]  * A.e[15] - 
              A.e[8]  * A.e[7]  * A.e[14] - 
              A.e[12] * A.e[6]  * A.e[11] + 
              A.e[12] * A.e[7]  * A.e[10];

    Inv[8] = A.e[4]  * A.e[9] * A.e[15] - 
             A.e[4]  * A.e[11] * A.e[13] - 
             A.e[8]  * A.e[5] * A.e[15] + 
             A.e[8]  * A.e[7] * A.e[13] + 
             A.e[12] * A.e[5] * A.e[11] - 
             A.e[12] * A.e[7] * A.e[9];

    Inv[12] = -A.e[4]  * A.e[9] * A.e[14] + 
               A.e[4]  * A.e[10] * A.e[13] +
               A.e[8]  * A.e[5] * A.e[14] - 
               A.e[8]  * A.e[6] * A.e[13] - 
               A.e[12] * A.e[5] * A.e[10] + 
               A.e[12] * A.e[6] * A.e[9];

    Inv[1] = -A.e[1]  * A.e[10] * A.e[15] + 
              A.e[1]  * A.e[11] * A.e[14] + 
              A.e[9]  * A.e[2] * A.e[15] - 
              A.e[9]  * A.e[3] * A.e[14] - 
              A.e[13] * A.e[2] * A.e[11] + 
              A.e[13] * A.e[3] * A.e[10];

    Inv[5] = A.e[0]  * A.e[10] * A.e[15] - 
             A.e[0]  * A.e[11] * A.e[14] - 
             A.e[8]  * A.e[2] * A.e[15] + 
             A.e[8]  * A.e[3] * A.e[14] + 
             A.e[12] * A.e[2] * A.e[11] - 
             A.e[12] * A.e[3] * A.e[10];

    Inv[9] = -A.e[0]  * A.e[9] * A.e[15] + 
              A.e[0]  * A.e[11] * A.e[13] + 
              A.e[8]  * A.e[1] * A.e[15] - 
              A.e[8]  * A.e[3] * A.e[13] - 
              A.e[12] * A.e[1] * A.e[11] + 
              A.e[12] * A.e[3] * A.e[9];

    Inv[13] = A.e[0]  * A.e[9] * A.e[14] - 
              A.e[0]  * A.e[10] * A.e[13] - 
              A.e[8]  * A.e[1] * A.e[14] + 
              A.e[8]  * A.e[2] * A.e[13] + 
              A.e[12] * A.e[1] * A.e[10] - 
              A.e[12] * A.e[2] * A.e[9];

    Inv[2] = A.e[1]  * A.e[6] * A.e[15] - 
             A.e[1]  * A.e[7] * A.e[14] - 
             A.e[5]  * A.e[2] * A.e[15] + 
             A.e[5]  * A.e[3] * A.e[14] + 
             A.e[13] * A.e[2] * A.e[7] - 
             A.e[13] * A.e[3] * A.e[6];

    Inv[6] = -A.e[0]  * A.e[6] * A.e[15] + 
              A.e[0]  * A.e[7] * A.e[14] + 
              A.e[4]  * A.e[2] * A.e[15] - 
              A.e[4]  * A.e[3] * A.e[14] - 
              A.e[12] * A.e[2] * A.e[7] + 
              A.e[12] * A.e[3] * A.e[6];

    Inv[10] = A.e[0]  * A.e[5] * A.e[15] - 
              A.e[0]  * A.e[7] * A.e[13] - 
              A.e[4]  * A.e[1] * A.e[15] + 
              A.e[4]  * A.e[3] * A.e[13] + 
              A.e[12] * A.e[1] * A.e[7] - 
              A.e[12] * A.e[3] * A.e[5];

    Inv[14] = -A.e[0]  * A.e[5] * A.e[14] + 
               A.e[0]  * A.e[6] * A.e[13] + 
               A.e[4]  * A.e[1] * A.e[14] - 
               A.e[4]  * A.e[2] * A.e[13] - 
               A.e[12] * A.e[1] * A.e[6] + 
               A.e[12] * A.e[2] * A.e[5];

    Inv[3] = -A.e[1] * A.e[6] * A.e[11] + 
              A.e[1] * A.e[7] * A.e[10] + 
              A.e[5] * A.e[2] * A.e[11] - 
              A.e[5] * A.e[3] * A.e[10] - 
              A.e[9] * A.e[2] * A.e[7] + 
              A.e[9] * A.e[3] * A.e[6];

    Inv[7] = A.e[0] * A.e[6] * A.e[11] - 
             A.e[0] * A.e[7] * A.e[10] - 
             A.e[4] * A.e[2] * A.e[11] + 
             A.e[4] * A.e[3] * A.e[10] + 
             A.e[8] * A.e[2] * A.e[7] - 
             A.e[8] * A.e[3] * A.e[6];

    Inv[11] = -A.e[0] * A.e[5] * A.e[11] + 
               A.e[0] * A.e[7] * A.e[9] + 
               A.e[4] * A.e[1] * A.e[11] - 
               A.e[4] * A.e[3] * A.e[9] - 
               A.e[8] * A.e[1] * A.e[7] + 
               A.e[8] * A.e[3] * A.e[5];

    Inv[15] = A.e[0] * A.e[5] * A.e[10] - 
              A.e[0] * A.e[6] * A.e[9] - 
              A.e[4] * A.e[1] * A.e[10] + 
              A.e[4] * A.e[2] * A.e[9] + 
              A.e[8] * A.e[1] * A.e[6] - 
              A.e[8] * A.e[2] * A.e[5];

    f32 Determinant = A.e[0]*Inv[0] + A.e[1]*Inv[4] + A.e[2]*Inv[8] + A.e[3]*Inv[12];
    Assert(Determinant != 0.0f);
    Determinant = 1.0f / Determinant;

    for (u32 ElementIndex = 0; ElementIndex < 16; ++ElementIndex)
    {
        Result.e[ElementIndex] = Inv[ElementIndex] * Determinant;
    }
    
    return Result;
}

//
// NOTE: Quaternion
//

inline quat Quat(f32 x, f32 y, f32 z, f32 w)
{
    quat Result = {};

    Result.x = x;
    Result.y = y;
    Result.z = z;
    Result.w = w;

    return Result;
}

inline quat Quat(v3 V)
{
    quat Result = {};

    Result.v = V;
    Result.w = 0.0f;

    return Result;
}

inline quat operator*(quat A, quat B)
{
    quat Result = {};

    Result.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;
    Result.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;
    Result.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;
    Result.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;
    
    return Result;
}

inline quat operator*=(quat A, quat B)
{
    quat Result = A * B;

    return Result;
}

inline f32 LengthSq(quat Q)
{
    f32 Length = SquareRoot(Square(Q.x) + Square(Q.y) + Square(Q.z) + Square(Q.w));

    return Length;
}

inline quat Normalize(quat Q)
{
    quat Result = Q;

    f32 LengthQ = LengthSq(Q);
    Q.x /= LengthQ;
    Q.y /= LengthQ;
    Q.z /= LengthQ;
    Q.w /= LengthQ;

    return Result;
}

inline quat Conjugate(quat Q)
{
    quat Result = Q;
    Result.v = -Q.v;

    return Result;
}

//
// NOTE: Camera functions
//

inline v3 RotateAroundAxis(v3 Vec, v3 Axis, f32 Angle)
{
    // NOTE: We rotate clockwise but the unit circle is flipped vertically
    v3 Result = {};

    quat VecQuat = Quat(Vec);

    quat RotateQuat = {};
    RotateQuat.v = Axis*Sin(Angle * 0.5f);
    RotateQuat.w = Cos(Angle * 0.5f);
    
    quat RotateConj = Conjugate(RotateQuat);
    quat OutputQuat = (RotateQuat*VecQuat)*RotateConj;

    Assert(LengthSq(RotateQuat) < 1.001f);
    Assert(LengthSq(RotateConj) < 1.001f);
    Assert(LengthSq(OutputQuat) < 1.001f);

    Result = OutputQuat.v;
    
    return Result;
}

//
// NOTE: Object position functions
// 

inline m4 TranslationMat(v3 Pos)
{
    m4 Result = Identity();
    Result.v[3].xyz = -Pos;

    return Result;
}

//
// NOTE: Ray tracing functions
//

inline v3 Refract(v3 RayDir, v3 Normal, f32 SolidRefractionIndex)
{
    v3 Result = {};

    f32 Cosi = Inner(RayDir, Normal);
    f32 RefractionIndex1 = 1.0f;
    f32 RefractionIndex2 = SolidRefractionIndex;

    if (Cosi < 0.0f)
    {
        // NOTE: We are outside the surface, we want cosi to be positive
        Cosi = -Cosi;
    }
    else
    {
        // NOTE: We are inside the surface, cosi is positive but we need to invert the normal
        Normal = -Normal;

        // NOTE: Swap refraction indicies
        f32 Temp = RefractionIndex1;
        RefractionIndex1 = RefractionIndex2;
        RefractionIndex2 = Temp;
    }

    f32 RefractionFraction = RefractionIndex1 / RefractionIndex2;
    f32 InnerSqrt = 1.0f - Square(RefractionFraction)*(1.0f - Square(Cosi));

    if (InnerSqrt >= 0.0f)
    {
        Result = RefractionFraction*RayDir + ((RefractionFraction*Cosi - SquareRoot(InnerSqrt))*Normal);
    }

    return Result;
}

inline f32 Fresnel(v3 RayDir, v3 Normal, f32 SolidRefractionIndex)
{
    f32 ReflectionIndex = 0.0f;
    
    f32 Cosi = Inner(RayDir, Normal);
    f32 RefractionIndex1 = 1.0f;
    f32 RefractionIndex2 = SolidRefractionIndex;

    if (Cosi > 0.0f)
    {        
        // NOTE: Swap refraction indicies
        f32 Temp = RefractionIndex1;
        RefractionIndex1 = RefractionIndex2;
        RefractionIndex2 = Temp;
    }

    f32 Sint = (RefractionIndex1 / RefractionIndex2) * SquareRoot(Max(0.0f, 1.0f - Square(Cosi)));

    if (Sint >= 1.0f)
    {
        // NOTE: We have a total internal reflection
        ReflectionIndex = 1.0f;
    }
    else
    {
        f32 Cost = SquareRoot(Max(0.0f, 1.0f - Square(Sint)));
        Cosi = Abs(Cosi);

        f32 t0 = RefractionIndex2*Cosi;
        f32 t1 = RefractionIndex1*Cost;
        f32 Rs = ((t0 - t1) / (t0 + t1));

        f32 t2 = RefractionIndex1*Cost;
        f32 t3 = RefractionIndex2*Cosi;
        f32 Rp = ((t2 - t3) / (t2 + t3));

        ReflectionIndex = 0.5f*(Square(Rs) + Square(Rp));
    }

    return ReflectionIndex;
}
