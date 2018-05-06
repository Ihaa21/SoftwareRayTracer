#include "pixelscratcher_platform.h"
#include "pixelscratcher.h"

#include "pixelscratcher_render.cpp"

inline void AddLightToScene(game_state* GameState, v3 Pos, f32 Intensity, v3 Color)
{
    Assert(GameState->CurrLightArrayCount < GameState->MaxLightArrayCount);
    
    world_object* CurrLight = GameState->LightArray + GameState->CurrLightArrayCount++;
    CurrLight->ModelType = ModelType_PointLight;
    CurrLight->PointLight.Pos = Pos;
    CurrLight->PointLight.Color = Color;
    CurrLight->PointLight.Intensity = Intensity;
}

inline void AddSphereToScene(game_state* GameState, v3 Center, f32 Radius, material Material)
{
    Assert(GameState->CurrObjectArrayCount < GameState->MaxObjectArrayCount);

    world_object* CurrObject = GameState->ObjectArray + GameState->CurrObjectArrayCount++;
    CurrObject->ModelType = ModelType_Sphere;
    CurrObject->Sphere.Center = Center;
    CurrObject->Sphere.Radius = Radius;
    CurrObject->Material = Material;
}

inline void AddPlaneToScene(game_state* GameState, v3 Normal, v3 Pos, material Material)
{
    Assert(GameState->CurrObjectArrayCount < GameState->MaxObjectArrayCount);

    world_object* CurrObject = GameState->ObjectArray + GameState->CurrObjectArrayCount++;
    CurrObject->ModelType = ModelType_Plane;
    CurrObject->Plane.Normal = Normal;
    CurrObject->Plane.Pos = Pos;
    CurrObject->Material = Material;
}

internal void ClearScreen(u32* Buffer, u32 Width, u32 Height)
{    
    u32* PixelRow = Buffer;
    for (u32 Y = 0; Y < Height; ++Y)
    {
        u32* Pixel = PixelRow;
        for (u32 X = 0; X < Width; ++X)
        {
            *Pixel = 0xFFAAFF00;
            ++Pixel;
        }

        PixelRow += Width;
    }
}

struct ray_collision_result
{
    f32 MinT;
    v3 Normal;
};

struct trace_ray_result
{
    material Material;
    f32 MinT;
    f32 Z;
    v3 Normal;
};

inline ray_collision_result CheckRayCollision(ray Ray, world_object* Object)
{
    ray_collision_result Result = {};
    Result.MinT = F32_MAX;

    f32 t = 0.0f;
    switch (Object->ModelType)
    {
        case ModelType_Plane:
        {
            if (RayPlaneIntersect(Ray, Object->Plane, &t))
            {
                if (t > 0.0f)
                {
                    Result.MinT = t;
                    Result.Normal = Object->Plane.Normal;
                }
            }                        
        } break;

        case ModelType_Sphere:
        {
            if (RaySphereIntersect(Ray, Object->Sphere, &t))
            {
                if (t > 0.0f)
                {
                    Result.MinT = t;
                    Result.Normal = Normalize((Ray.Origin + t*Ray.Dir) - Object->Sphere.Center);
                }
            }   
        } break;
    }

    return Result;
}

inline b32 TraceShadowRayOpaque(game_state* GameState, ray Ray, f32 LightT)
{
    b32 Result = true;
    
    for (u32 ObjectIndex = 0; ObjectIndex < GameState->CurrObjectArrayCount; ++ObjectIndex)
    {
        world_object* Object = GameState->TransformedObjectArray + ObjectIndex;

        ray_collision_result RayCollision = CheckRayCollision(Ray, Object);
        if (RayCollision.MinT <= LightT)
        {
            Result = false;
            break;
        }
    }
    
    return Result;
}

inline trace_ray_result FindFirstRayCollision(game_state* GameState, ray Ray)
{
    trace_ray_result Result = {};
    Result.MinT = F32_MAX;

    for (u32 ObjectIndex = 0; ObjectIndex < GameState->CurrObjectArrayCount; ++ObjectIndex)
    {
        world_object* Object = GameState->TransformedObjectArray + ObjectIndex;
        ray_collision_result RayCollision = CheckRayCollision(Ray, Object);

        if (RayCollision.MinT < Result.MinT)
        {
            Result.MinT = RayCollision.MinT;
            Result.Normal = RayCollision.Normal;
            switch (Object->ModelType)
            {
                case ModelType_Plane:
                case ModelType_Sphere:
                {
                    Result.Material = Object->Material;
                } break;
            }
        }
    }

    return Result;
}

internal trace_ray_result TraceRay(game_state* GameState, ray Ray, u32 Recursion = 0)
{
    trace_ray_result Result = {};

    if (Recursion == MAX_RECURSION)
    {
        return Result;
    }
    ++Recursion;
    
    Result = FindFirstRayCollision(GameState, Ray);
    if (!(Result.MinT < F32_MAX && Result.MinT >= 0.0f))
    {
        // NOTE: We didn't find a collision
        return Result;
    }
    
    v3 IntersectionPoint = (Ray.Origin + Result.MinT*Ray.Dir) + RAY_HIT_BIAS*Result.Normal;
    Result.Z = IntersectionPoint.z;
    material Material = Result.Material;

    switch (Material.Type)
    {
        case MaterialType_Reflective:
        {
            Ray.Dir = Normalize(Ray.Dir);
            
            ray ReflectedRay = {};
            ReflectedRay.Origin = IntersectionPoint;
            ReflectedRay.Dir = Normalize(GetReflection(Result.Normal, Ray.Dir));

            trace_ray_result ReflectedResult = TraceRay(GameState, ReflectedRay, Recursion);
            Result.Material.Color = ReflectedResult.Material.Color * Material.ReflectionPercent;
        } break;
            
        case MaterialType_ReflectiveRefractive:
        {
            Ray.Dir = Normalize(Ray.Dir);
            v3 OriginBias = 2.0f*RAY_HIT_BIAS*Result.Normal;
            
            f32 RefractionIndex = Material.RefractionIndex;
            f32 ReflectionIndex = Fresnel(Ray.Dir, Result.Normal, RefractionIndex);
            b32 Outside = Inner(Ray.Dir, Result.Normal) < 0.0f;
            
            v3 RefractColor = {};
            if (ReflectionIndex < 1.0f)
            {
                ray RefractedRay = {};
                RefractedRay.Origin = Outside ? IntersectionPoint - OriginBias : IntersectionPoint;
                RefractedRay.Dir = Normalize(Refract(Ray.Dir, Result.Normal, RefractionIndex));
                trace_ray_result RefractResult = TraceRay(GameState, RefractedRay, Recursion);

                RefractColor = RefractResult.Material.Color;
            }

            v3 ReflectColor = {};
            ray ReflectedRay = {};
            ReflectedRay.Origin = Outside ? IntersectionPoint - OriginBias : IntersectionPoint;
            ReflectedRay.Dir = Normalize(GetReflection(Result.Normal, Ray.Dir));
            trace_ray_result ReflectResult = TraceRay(GameState, ReflectedRay, Recursion);

            ReflectColor = ReflectResult.Material.Color;
            Result.Material.Color = Lerp(ReflectColor, RefractColor, ReflectionIndex);
        }

        case MaterialType_Diffuse:
        {
            // NOTE: Calculate shadow rays
            f32 TotalDiffuse = 0.0f;
            v3 LightColor = {};
            for (u32 LightIndex = 0; LightIndex < GameState->CurrLightArrayCount; ++LightIndex)
            {
                world_object* Light = GameState->TransformedLightArray + LightIndex;
        
                ray ShadowRay = {};      
                ShadowRay.Origin = IntersectionPoint;
                ShadowRay.Dir = Light->PointLight.Pos - IntersectionPoint;
                f32 LightT = LengthSq(ShadowRay.Dir);
                if (LightT != 0.0f)
                {
                    ShadowRay.Dir /= LightT;
                }
                
                b32 NotBlocked = TraceShadowRayOpaque(GameState, ShadowRay, LightT);
                if (NotBlocked)
                {
                    // NOTE: We use a blinn-phong shading model
                    v3 LightIntensity = (Light->PointLight.Color*
                                         (Light->PointLight.Intensity / (4.0f*Pi32*LightT)));

                    f32 Direction = Inner(Result.Normal, ShadowRay.Dir);

                    v3 PointToEye = -Ray.Dir;
                    v3 LightDirection = -ShadowRay.Dir;
                    v3 ReflectionVector = GetReflection(Result.Normal, LightDirection);
                    f32 Specular = Pow(Max(0.0f, Inner(ReflectionVector, PointToEye)), 10);

                    f32 DiffuseP = Material.DiffusePercent;
                    f32 SpecularP = Material.SpecularPercent;
                    
                    v3 DiffuseColor = 0.18f*LightIntensity*Max(Direction, 0.0f);
                    v3 SpecularColor = LightIntensity*Specular;
                    LightColor +=  DiffuseP*DiffuseColor + SpecularP*SpecularColor;
                }
            }
    
            Result.Material.Color.r = Min(LightColor.r*Result.Material.Color.r, 1.0f);
            Result.Material.Color.g = Min(LightColor.g*Result.Material.Color.g, 1.0f);
            Result.Material.Color.b = Min(LightColor.b*Result.Material.Color.b, 1.0f);
        }
    }
        
    return Result;
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state* GameState = (game_state*)GameMem->PermanentMem;
    
    if (!GameMem->IsInitialized)
    {
        GameState->Arena = InitArena((u8*)GameState + sizeof(game_state),
                                     GameMem->PermanentMemSize - sizeof(game_state));

        mm ScreenBufferSize = Display->Width * Display->Height;
        GameState->ColorBuffer = PushArray(&GameState->Arena, u32, ScreenBufferSize);
        GameState->DepthBuffer = PushArray(&GameState->Arena, u32, ScreenBufferSize);
        GameState->NormalBuffer = PushArray(&GameState->Arena, u32, ScreenBufferSize);
        
        // NOTE: Sets up the angles given a target vector (check if it works for all angles)
        camera* Camera = &GameState->Camera;
        v3 Target = V3(0, 0, 1); // NOTE: Input target vector
        v3 HTarget = Normalize(V3(Target.x, 0, Target.z));

        if (HTarget.z >= 0.0f)
        {
            if (HTarget.x >= 0.0f)
            {
                Camera->CurrentRotation.x = (2.0f*Pi32) - ArcSin(HTarget.z);
            }
            else
            {
                Camera->CurrentRotation.x = (2.0f*Pi32) - ArcSin(HTarget.z);
            }
        }
        else
        {
            if (HTarget.x >= 0.0f)
            {
                Camera->CurrentRotation.x = ArcSin(-HTarget.z);
            }
            else
            {
                Camera->CurrentRotation.x = Pi32 - ArcSin(-HTarget.z);
            }
        }

        Camera->CurrentRotation.y = -ArcSin(Target.y);
        
        // NOTE: Init array of lights in our world
        GameState->CurrLightArrayCount = 0;
        GameState->MaxLightArrayCount = 100;
        GameState->LightArray = PushArray(&GameState->Arena, world_object, GameState->MaxLightArrayCount);
        GameState->TransformedLightArray = PushArray(&GameState->Arena, world_object, GameState->MaxLightArrayCount);

        // NOTE: Init array of objects in our world
        GameState->CurrObjectArrayCount = 0;
        GameState->MaxObjectArrayCount = 100;
        GameState->ObjectArray = PushArray(&GameState->Arena, world_object, GameState->MaxObjectArrayCount);
        GameState->TransformedObjectArray = PushArray(&GameState->Arena, world_object, GameState->MaxObjectArrayCount);

        material RefractiveMaterial = {};
        {
            RefractiveMaterial.Type = MaterialType_ReflectiveRefractive;
            RefractiveMaterial.Color = V3(0, 1, 0);
            RefractiveMaterial.RefractionIndex = 0.86f;
        }

        material ReflectiveMaterial = {};
        {
            ReflectiveMaterial.Type = MaterialType_Reflective;
            ReflectiveMaterial.Color = V3(1, 1, 1);
            ReflectiveMaterial.ReflectionPercent = 0.8f;
        }

        material DiffuseMaterial = {};
        {
            DiffuseMaterial.Type = MaterialType_Diffuse;
            DiffuseMaterial.Color = V3(1, 1, 1);
            DiffuseMaterial.DiffusePercent = 0.8f;
            DiffuseMaterial.SpecularPercent = 0.2f;
        }
        
        AddSphereToScene(GameState, V3(0, 0, 4), 1.0f, ReflectiveMaterial);
        AddSphereToScene(GameState, V3(-2, 0, 7), 1.0f, DiffuseMaterial);

        AddPlaneToScene(GameState, V3(0, 1, 0), V3(0, -1, 0), DiffuseMaterial);
        AddLightToScene(GameState, V3(0, 4, 0), 100.0f, V3(1, 1, 1));

        AddLightToScene(GameState, V3(-5, 4, 0), 100.0f, V3(1, 0, 1));
        AddLightToScene(GameState, V3(0, 0, 0), 200.0f, V3(1, 1, 1));
        AddLightToScene(GameState, V3(5, 4, 0), 50.0f, V3(0, 0, 1));
        
        // NOTE: Init ray direction vectors
        GameState->RayDir = PushArray(&GameState->Arena, v3, Display->Width*Display->Height);
        v2 PixelDim = V2(2.0f / (f32)Display->Width, 2.0f / (f32)Display->Height);
        v3 RayOrigin = {};
        for (u32 Y = 0; Y < Display->Height; ++Y)
        {
            for (u32 X = 0; X < Display->Width; ++X)
            {
                v3* RayDir = GameState->RayDir + Display->Width*Y + X;
                RayDir->x = -1.0f + 0.5f*PixelDim.x + X*PixelDim.x;
                RayDir->y = -1.0f + 0.5f*PixelDim.y + Y*PixelDim.y;
                RayDir->z = 1.0f;

                *RayDir = *RayDir - RayOrigin;
                *RayDir = Normalize(*RayDir);
            }
        }

        GameState->PrevMouseP = V2(0.5f, 0.5f);
        GameMem->IsInitialized = true;
    }

    // NOTE: Transform objects to camera space
    m4 WorldToCamera = {};
    {
        camera* Camera = &GameState->Camera;

        v2 Delta = Input->MouseP - GameState->PrevMouseP;
        GameState->PrevMouseP = Input->MouseP;

        v2 AngleDelta = V2(Delta.x, -Delta.y) / 2.5f;

        Camera->CurrentRotation += AngleDelta;
    
        if (Delta.x == 0.0f)
        {
            if (Input->MouseP.x < 0.3f)
            {
                Camera->CurrentRotation.x += -0.1f;
            }
            else if (Input->MouseP.x >= 0.7f)
            {
                Camera->CurrentRotation.x += 0.1f;
            }
        }
        if (Delta.y == 0.0f)
        {
            if (Input->MouseP.y < 0.3f)
            {
                Camera->CurrentRotation.y += 0.1f;
            }
            else if (Input->MouseP.y >= 0.7f)
            {
                Camera->CurrentRotation.y += -0.1f;
            }
        }
    
        // NOTE: Keep high precision
        if (Camera->CurrentRotation.x > 2.0f*Pi32)
        {
            Camera->CurrentRotation.x -= 2.0f*Pi32;
        }
        else if (Camera->CurrentRotation.x < -2.0f*Pi32)
        {
            Camera->CurrentRotation.x += 2.0f*Pi32;
        }
        if (Camera->CurrentRotation.y > 2.0f*Pi32)
        {
            Camera->CurrentRotation.y -= 2.0f*Pi32;
        }
        else if (Camera->CurrentRotation.y < -2.0f*Pi32)
        {
            Camera->CurrentRotation.y += 2.0f*Pi32;
        }

        // NOTE: Our camera matrix is in a left hand coordinate system
        v3 Target = V3(1, 0, 0); // NOTE: This vector has both angles = 0
        v3 Up = V3(0, 1, 0);
        Target = Normalize(RotateAroundAxis(Target, Up, Camera->CurrentRotation.x));

        v3 Horizontal = Cross(Up, Target);
        Horizontal = Normalize(Horizontal);
        Target = Normalize(RotateAroundAxis(Target, Horizontal, Camera->CurrentRotation.y));

        Up = Normalize(Cross(Target, Horizontal));

        Assert(Abs(Inner(Target, Up)) <= 0.001f);
        Assert(Abs(Inner(Up, Horizontal)) <= 0.001f);
        Assert(Abs(Inner(Horizontal, Target)) <= 0.001f);
        
        // NOTE: Move the camera by the given input
        f32 MoveForward = 0.0f;
        f32 MoveSideways = 0.0f;
        if (Input->MoveLeftDown)
        {
            MoveSideways += -1.0f;
        }
        if (Input->MoveRightDown)
        {
            MoveSideways += 1.0f;
        }
        if (Input->MoveUpDown)
        {
            MoveForward += 1.0f;
        }
        if (Input->MoveDownDown)
        {
            MoveForward += -1.0f;
        }

        v3 MovementVector = 0.1f*(MoveForward*Target + MoveSideways*Horizontal);
        Camera->Pos += MovementVector;
    
        {
            // NOTE: We inverse the transpose to apply the opposite
            // transformation of the cameras orientation
            m4 Rotation = {};
            Rotation.v[0].xyz = Horizontal;
            Rotation.v[1].xyz = Up;
            Rotation.v[2].xyz = Target;
            Rotation.v[3].w = 1.0f;
            Rotation = Transpose(Rotation);

            m4 Translation = TranslationMat(Camera->Pos);
            WorldToCamera = Rotation*Translation;
        }
    }

    // NOTE: Transform lights and objects into camera space
    for (u32 ObjectIndex = 0; ObjectIndex < GameState->CurrObjectArrayCount; ++ObjectIndex)
    {
        world_object* Object = GameState->ObjectArray + ObjectIndex;
        world_object* TransformedObj = GameState->TransformedObjectArray + ObjectIndex;
        *TransformedObj = *Object;

        switch (Object->ModelType)
        {
            case ModelType_Plane:
            {
                plane* P = &TransformedObj->Plane;
                P->Pos = (WorldToCamera * V4(P->Pos, 1.0f)).xyz;
                P->Normal = (Transpose(Inverse(WorldToCamera)) * V4(P->Normal, 0.0f)).xyz;
                i32 x = 0;
            } break;

            case ModelType_Sphere:
            {
                sphere* S = &TransformedObj->Sphere;
                S->Center = (WorldToCamera * V4(S->Center, 1.0f)).xyz;
                i32 i = 0;
            } break;
        }
    }

    for (u32 LightIndex = 0; LightIndex < GameState->CurrLightArrayCount; ++LightIndex)
    {
        world_object* Object = GameState->LightArray + LightIndex;
        world_object* TransformedObj = GameState->TransformedLightArray + LightIndex;
        *TransformedObj = *Object;

        switch (Object->ModelType)
        {
            case ModelType_PointLight:
            {
                point_light* P = &TransformedObj->PointLight;
                P->Pos = (WorldToCamera * V4(P->Pos, 1.0f)).xyz;
            } break;
        }
    }
    
    ClearScreen(Display->FrameBuffer, Display->Width, Display->Height);
    ClearScreen(GameState->ColorBuffer, Display->Width, Display->Height);
    ClearScreen(GameState->DepthBuffer, Display->Width, Display->Height);
    ClearScreen(GameState->NormalBuffer, Display->Width, Display->Height);

    // NOTE: Raytrace the scene
    u32* ColorLine = (u32*)GameState->ColorBuffer;
    u32* DepthLine = (u32*)GameState->DepthBuffer;
    u32* NormalLine = (u32*)GameState->NormalBuffer;
    v3 RayOrigin = {};
    for (u32 Y = 0; Y < Display->Height; ++Y)
    {
        u32* ColorPixel = ColorLine;
        u32* DepthPixel = DepthLine;
        u32* NormalPixel = NormalLine;
        
        for (u32 X = 0; X < Display->Width; ++X)
        {
            ray Ray = {};
            Ray.Dir = GameState->RayDir[Display->Width*Y + X];
            trace_ray_result Result = TraceRay(GameState, Ray);

            if (Result.MinT < F32_MAX)
            {
                *ColorPixel = (((u32)(Result.Material.Color.r*255.0f) << 16) |
                               ((u32)(Result.Material.Color.g*255.0f) << 8) |
                               ((u32)(Result.Material.Color.b*255.0f) << 0));
                *DepthPixel = (u32)(255.0f*(Result.Z / MAX_Z)) << 8;
                *NormalPixel = (((u32)(127.0f*Result.Normal.x + 255.0f) << 16) |
                                ((u32)(127.0f*Result.Normal.y + 255.0f) << 8) |
                                ((u32)(127.0f*Result.Normal.z + 255.0f) << 0));
            }

            ++ColorPixel;
            ++DepthPixel;
            ++NormalPixel;
        }

        ColorLine += Display->Width;
        DepthLine += Display->Width;
        NormalLine += Display->Width;
    }

    mm CopySize = Display->Width*Display->Height*sizeof(u32);
    Copy(GameState->ColorBuffer, Display->FrameBuffer, CopySize);
    //Copy(GameState->DepthBuffer, Display->FrameBuffer, CopySize);
    //Copy(GameState->NormalBuffer, Display->FrameBuffer, CopySize);
}
