
inline v2 operator+(v2 A, v2 B)
{
    v2 Result = {};

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return Result;
}

inline v3 operator+(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return Result;
}

inline v4 operator+(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;
    
    return Result;
}

inline v2 operator-(v2 A, v2 B)
{
    v2 Result = {};

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return Result;
}

inline v3 operator-(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return Result;
}

inline v4 operator-(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;
    
    return Result;
}
