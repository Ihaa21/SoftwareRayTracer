#if !defined(PIXELSCRATCHER_MATH_H)

struct v2
{
    union
    {
        struct
        {
            f32 x, y;
        };

        f32 e[2];
    };
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

        struct
        {
            f32 r, g, b;
        };

        f32 e[3];
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

        f32 e[4];
    };
};

// NOTE: Matrices are stored column order
struct m4
{
    union
    {
        f32 e[16];
        v4 v[4];
    };
};

struct quat
{
    union
    {
        struct
        {
            f32 x, y, z, w;
        };

        struct
        {
            v3 v;
            f32 w;
        };
    };
};

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

struct ray
{
    v3 Origin;
    v3 Dir;
};

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
