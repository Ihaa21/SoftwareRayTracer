/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

inline b32 IsEndOfLine(char C)
{
    b32 Result = ((C == '\n') ||
                  (C == '\r'));

    return Result;
}

inline b32 IsWhiteSpace(char C)
{
    b32 Result = ((C == ' ') ||
                  (C == '\t') ||
                  (C == '\v') ||
                  (C == '\f') ||
                  IsEndOfLine(C));

    return Result;
}
    
inline b32 StringsAreEqual(char* A, char* B)
{
    // NOTE: If both are null, they are equal
    b32 Result = (A == B);

    if (A && B)
    {
        while(*A && *B && *A == *B)
        {
            ++A;
            ++B;
        }
        
        Result = (*A == 0) && (*B == 0);
    }
    
    return Result;
}

inline b32 StringsAreEqual(umm ALength, char* A, char* B)
{
    b32 Result = false;
    
    if (B)
    {
        char* At = B;
        for (umm Index = 0; Index < ALength; ++Index, ++At)
        {
            if ((*At == 0) || (A[Index] != *At))
            {
                return false;
            }
        }

        // NOTE: Return true if both strings ended
        Result = (*At == 0);
    }
    else
    {
        Result = ALength == 0;
    }

    return Result;
}

