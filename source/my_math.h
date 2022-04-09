#ifndef MY_MATH_INCLUDE
#define MY_MATH_INCLUDE

#include <math.h> //TODO: Remove and replace with own, faster platform specific implementations
#include <assert.h>

#define PI 3.1415926535897932385f

typedef union v2
{
#ifdef __cplusplus
    v2() = default;
    v2 (f32 x, f32 y) : x(x), y(y){};
#endif
    
    struct
    {
        f32 x, y;
    };
    
    struct
    {
        f32 u, v;
    };
    
    struct
    {
        f32 width, height;
    };
    
    f32 elem[2];
    
#ifdef __cplusplus
    inline f32 &operator[](const int &index)
    {
        return elem[index];
    }
#endif
} v2;

typedef union v3
{
#ifdef __cplusplus
    v3() = default;
    v3 (f32 x, f32 y, f32 z) : x(x), y(y), z(z){};
    v3 (v2 vec2, f32 z) : xy(vec2), z(z){};
#endif
    
    struct
    {
        f32 x, y, z;
    };
    
    struct
    {
        f32 u, v, w;
    };
    
    struct
    {
        f32 r, g, b;
    };
    
    struct
    {
        v2 xy;
        f32 ignored0_;
    };
    
    struct
    {
        f32 ignored1_;
        v2 yz;
    };
    
    struct
    {
        v2 uv;
        f32 ignored2_;
    };
    
    struct
    {
        f32 ignored3_;
        v2 vw;
    };
    
    f32 elem[3];
    
#ifdef __cplusplus
    inline f32 &operator[](const int &Index)
    {
        return elem[Index];
    }
#endif
} v3;

typedef union v4
{
#ifdef __cplusplus
    v4() = default;
    v4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w){};
    v4(v2 xy, v2 zw) : xy(xy), zw(zw){};
    v4(v2 xy, f32 z, f32 w) : xy(xy), z(z), w(w){};
    v4(v3 xyz, f32 w) : xyz(xyz), w(w){};
#endif
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                f32 x, y, z;
            };
        };
        
        f32 w;
    };
    
    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                f32 r, g, b;
            };
        };
        
        f32 a;
    };
    
    struct
    {
        v2 xy;
        f32 ignored0_;
        f32 ignored1_;
    };
    
    struct
    {
        f32 ignored2_;
        v2 yz;
        f32 ignored3_;
    };
    
    struct
    {
        f32 ignored4_;
        f32 ignored5_;
        v2 zw;
    };
    
    f32 elem[4];
    
#ifdef __cplusplus
    inline f32 &operator[](const int &index)
    {
        return elem[index];
    }
#endif
} v4;

struct Mat4x4
{
    //These are stored ROW MAJOR - elem[ROW][COLUMN]!!!
    f32 elem[4][4];
};

struct mat3x3
{
    //These are stored ROW MAJOR - elem[ROW][COLUMN]!!!
    f32 elem[3][3];
};

struct mat2x2
{
    //These are stored ROW MAJOR - elem[ROW][COLUMN]!!!
    f32 elem[2][2];
};

inline Mat4x4 IdentityMatrix();

//Other v2's I might use. Torn on whether or not I should template things but I think for 90 percent of what I'm using vectors for floats should be what I want
struct v2i
{
    v2i() = default;
    v2i(s32 x, s32 y);
    
    union
    {
        s32 Elem[2];
        struct
        {
            s32 x, y;
        };
        struct
        {
            s32 width, height;
        };
        struct
        {
            s32 u, v;
        };
    };
};

struct v3i
{
    v3i() = default;
    v3i(s32 x, s32 y, s32 z);
    
    union
    {
        s32 Elem[3];
        struct
        {
            s32 x, y, z;
        };
        
        struct
        {
            s32 r, g, b;
        };
    };
};

struct v4i
{
    v4i() = default;
    v4i(s32 x, s32 y, s32 z, s32 w);
    
    union
    {
        s32 Elem[4];
        struct
        {
            s32 x, y, z, w;
        };
        
        struct
        {
            s32 r, g, b, a;
        };
    };
};

struct v4u32
{
    v4u32() = default;
    v4u32(u32 x, u32 y, u32 z, u32 w);
    
    union
    {
        s32 Elem[4];
        struct
        {
            u32 x, y, z, w;
        };
        
        struct
        {
            u32 r, g, b, a;
        };
    };
};

inline v2 operator/(v2 b, f32 a);
inline v2 operator/(v2 b, v2 a);
inline bool operator==(v2 a, v2 B);
inline bool operator!=(v2 a, v2 B);
inline v2 operator*(f32 a, v2 B);
inline v2 operator*(v2 B, f32 a);
inline v2& operator*=(v2& B, f32 a);
inline v2 operator+(v2 a, v2 B);
inline v2 operator+(v2 a, f32 B);
inline v2& operator+=(v2& a, v2 B);
inline v2& operator+=(v2& a, f32 B);
inline v2& operator-=(v2& a, f32 B);
inline v2& operator-=(v2& a, v2 B);
inline v2 operator-(v2 a, v2 B);
inline v2 operator-(v2 a, f32 b);
inline v2 operator-(v2 a);
inline v3 operator*(f32 A, v3 B);
inline v3 operator*(v3 B, f32 A);
inline v3 & operator*=(v3 &B, f32 A);
inline v3 operator-(v3 A);
inline v3 operator+(v3 A, v3 B);
inline v3 operator+(v3 a, f32 B);
inline v3 & operator+=(v3 &A, v3 B);
inline v3 & operator+=(v3 &A, f32 B);
inline v3 operator-(v3 A, v3 B);
inline v3 & operator-=(v3 &A, v3 B);
inline v4 operator*(f32 a, v4 B);
inline v4 operator*(v4 B, f32 a);
inline v4 & operator*=(v4 &B, f32 a);
inline v4 operator+(v4 a, v4 B);
inline v4 & operator+=(v4 &a, v4 B);
inline v4 operator-(v4 a, v4 b);
inline v4 operator-(v4 a);
inline v4 operator*(Mat4x4 A, v4 P);
local_func Mat4x4 operator*(Mat4x4 A, Mat4x4 B);

inline f32 Max(f32 x, f32 y);
inline f32 Min(f32 x, f32 y);
inline f32 Mod(f32 x, f32 y);
inline void AbsoluteVal(s32&& Value);
inline void AbsoluteVal(f32&& Value);
f32 AbsoluteValFloat(f32 Value);
inline void AbsoluteVal(v2&& Value);
inline f32 ToRadians(f32 angleInDegrees);
inline f32 ToDegrees(f32 angleInRadians);
inline f32 SinR(f32 angleInRadians);
inline f32 InvSinR(f32 angleInRadians);
inline f32 CosR(f32 AngleInRadians);
inline f32 InvCosR(f32 angleInRadians);
inline f32 TanR(f32 AngleInRadians);
inline f32 InvTanR(f32 value);
inline f32 SinD(f32 angleInDegrees);
inline f32 InvSinD(f32 angleInDegrees);
inline f32 CosD(f32 angleInDegrees);
inline f32 InvCosD(f32 angleInDegrees);
inline f32 TanD(f32 angleInDegrees);
inline f32 InvTanD(f32 angleInDegrees);
inline void Clamp(f32&& value, f32 min, f32 max);
inline f64 Sqrt(f64 number);
inline f32 Sqrt(f32 number);
inline sizet RoundUp(sizet NumToRound, sizet Multiple);
inline sizet RoundDown(sizet NumToRound, sizet Multiple);
inline f32 Floor(f32 floatToFloor);
inline s32 FloorF32ToI32(f32 floatToFloor);
inline s32 CeilF32ToI32(f32 floatToCeil);
inline v2 PerpendicularOp(v2 A);
inline f32 DotProduct(v2 a, v2 b);
inline f32 DotProduct(v3 a, v3 b);
inline v2 Hadamard(v2 a, v2 b);
inline v3 Hadamard(v3 a, v3 b);
inline f32 CrossProduct(v2 a, v2 b);
inline f32 MagnitudeSqd(v2 a);
inline f32 MagnitudeSqd(v3 a);
inline f32 Magnitude(v2 a);
inline f32 Magnitude(v3 a);
inline f32 Lerp(f32 a, f32 b, f32 t);
inline v4 Lerp(v4 a, v4 b, f32 t);
inline v2 Lerp(v2 a, v2 b, f32 t);
inline void Normalize(v2&& a);
inline void Normalize(v3&& a);
local_func void ConvertNegativeToPositiveAngle_Radians(f32&& radianAngle);
local_func void ConvertPositiveToNegativeAngle_Radians(f32&& radianAngle);
local_func void ConvertToCorrectPositiveRadian(f32&& angle);
Mat4x4 XRotation(f32 Angle);
inline Mat4x4 YRotation(f32 Angle);
inline Mat4x4 ZRotation(f32 Angle);
inline Mat4x4 ScaleMatrix(v3 scale);
local_func Mat4x4 Translate(Mat4x4 A, v4 T);
local_func Mat4x4 ProduceWorldTransformMatrix(v3 translation, v3 rotation, v3 scale);
local_func Mat4x4 ProduceCameraTransformMatrix(v3 xAxis, v3 yAxis, v3 zAxis, v3 vecToTransform);
local_func Mat4x4 ProduceProjectionTransformMatrix_UsingFOV(f32 FOV_inDegrees, f32 aspectRatio, f32 nearPlane, f32 farPlane);
inline v3 GetColumn(Mat4x4 A, u32 c);
inline Mat4x4 IdentityMatrix();
inline Mat4x4 Transpose(Mat4x4 A);
local_func v2 CastV2IToV2F(v2i vecToCast);
v2i CastV2FToV2I(v2 vecToCast);
Mat4x4 InvertMatrix(Mat4x4 matrixToInvert);
inline f32 Radians(f32 angleInDegrees);

#endif

#ifdef MY_MATH_IMPL

inline f32
Radians(f32 angleInDegrees)
{
    f32 angleInRadians = angleInDegrees * (PI / 180.0f);
    return angleInRadians;
};

v2 CastV2IToV2F(v2i vecToCast)
{
    v2 result{};
    
    result.x = (f32)vecToCast.x;
    result.y = (f32)vecToCast.y;
    
    return result;
}

v2i CastV2FToV2I(v2 vecToCast)
{
    v2i result{};
    
    result.x = (s32)vecToCast.x;
    result.y = (s32)vecToCast.y;
    
    return result;
};

inline bool
operator==(v2 a, v2 B)
{
    bool result { false };
    
    if (a.x == B.x && a.y == B.y)
        result = true;
    
    return result;
};

inline bool
operator!=(v2 a, v2 B)
{
    bool result { false };
    
    if (a.x != B.x || a.y != B.y)
        result = true;
    
    return result;
};

inline v2
operator*(f32 a, v2 B)
{
    v2 result;
    
    result.x = a * B.x;
    result.y = a * B.y;
    
    return (result);
}

inline v2
operator*(v2 B, f32 a)
{
    v2 result = a * B;
    
    return (result);
}

inline v2&
operator*=(v2& B, f32 a)
{
    B = a * B;
    
    return (B);
}

inline v2
operator/(v2 b, f32 a)
{
    v2 result;
    result.x = b.x / a;
    result.y = b.y / a;
    
    return result;
}

inline v2
operator/(v2 b, v2 a)
{
    v2 result{};
    result.x = b.x / a.x;
    result.y = b.y / a.y;
    
    return result;
};

inline v2
operator+(v2 a, v2 B)
{
    v2 result;
    
    result.x = a.x + B.x;
    result.y = a.y + B.y;
    
    return (result);
}

inline v2
operator+(v2 a, f32 B)
{
    v2 result;
    
    result.x = a.x + B;
    result.y = a.y + B;
    
    return (result);
}

inline v2&
operator+=(v2& a, v2 B)
{
    a = a + B;
    
    return (a);
}

inline v2&
operator+=(v2& a, f32 B)
{
    a.x = a.x + B;
    a.y = a.y + B;
    
    return (a);
}

inline v2&
operator-=(v2& a, f32 B)
{
    a.x = a.x - B;
    a.y = a.y - B;
    
    return (a);
}

inline v2&
operator-=(v2& a, v2 B)
{
    a.x = a.x - B.x;
    a.y = a.y - B.y;
    
    return (a);
}

inline v2
operator-(v2 a, v2 B)
{
    v2 result;
    
    result.x = a.x - B.x;
    result.y = a.y - B.y;
    
    return (result);
}

inline v2
operator-(v2 a, f32 b)
{
    v2 result;
    
    result.x = a.x - b;
    result.y = a.y - b;
    
    return (result);
}

inline v2
operator-(v2 a)
{
    v2 result;
    
    result.x = -a.x;
    result.y = -a.y;
    
    return(result);
}

inline v3
operator*(f32 A, v3 B)
{
    v3 result;
    
    result.x = A*B.x;
    result.y = A*B.y;
    result.z = A*B.z;
    
    return(result);
}

inline v3
operator*(v3 B, f32 A)
{
    v3 result = A*B;
    
    return(result);
}

inline v3 &
operator*=(v3 &B, f32 A)
{
    B = A * B;
    
    return(B);
}

inline v3
operator-(v3 A)
{
    v3 result;
    
    result.x = -A.x;
    result.y = -A.y;
    result.z = -A.z;
    
    return(result);
}

inline v3
operator+(v3 A, v3 B)
{
    v3 result;
    
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;
    
    return(result);
}

inline v3
operator+(v3 a, f32 B)
{
    v3 result;
    
    result.x = a.x + B;
    result.y = a.y + B;
    result.z = a.z + B;
    
    return (result);
}

inline v3 &
operator+=(v3 &A, v3 B)
{
    A = A + B;
    
    return(A);
}

inline v3 &
operator+=(v3 &A, f32 B)
{
    A = A + B;
    
    return(A);
}

inline v3
operator-(v3 A, v3 B)
{
    v3 result;
    
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;
    
    return(result);
}

inline v3 &
operator-=(v3 &A, v3 B)
{
    A = A - B;
    
    return(A);
}

inline v4
operator*(f32 a, v4 B)
{
    v4 result;
    
    result.x = a*B.x;
    result.y = a*B.y;
    result.z = a*B.z;
    result.w = a*B.w;
    
    return(result);
}

inline v4
operator*(v4 B, f32 a)
{
    v4 result = a*B;
    
    return(result);
}

inline v4 &
operator*=(v4 &B, f32 a)
{
    B = a * B;
    
    return(B);
}

inline v4
operator+(v4 a, v4 B)
{
    v4 result;
    
    result.x = a.x + B.x;
    result.y = a.y + B.y;
    result.z = a.z + B.z;
    result.w = a.w + B.w;
    
    return(result);
}

inline v4 &
operator+=(v4 &a, v4 B)
{
    a = a + B;
    
    return(a);
}

inline v4
operator-(v4 a, v4 b)
{
    v4 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    
    return (result);
}

inline v4
operator-(v4 a)
{
    v4 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    
    return (result);
}

local_func Mat4x4
operator*(Mat4x4 A, Mat4x4 B)
{
    // NOTE(casey): This is written to be instructive, not optimal!
    
    Mat4x4 R = {};
    
    for(int r = 0; r <= 3; ++r) // NOTE(casey): Rows (of A)
    {
        for(int c = 0; c <= 3; ++c) // NOTE(casey): Column (of B)
        {
            for(int i = 0; i <= 3; ++i) // NOTE(casey): Columns of A, rows of B!
            {
                R.elem[r][c] += A.elem[r][i]*B.elem[i][c];
            }
        }
    }
    
    return(R);
}

local_func v4 TransformVec(Mat4x4 A, v4 P)
{
    v4 R;
    
    R.x = P.x*A.elem[0][0] + P.y*A.elem[0][1] + P.z*A.elem[0][2] + P.w*A.elem[0][3];
    R.y = P.x*A.elem[1][0] + P.y*A.elem[1][1] + P.z*A.elem[1][2] + P.w*A.elem[1][3];
    R.z = P.x*A.elem[2][0] + P.y*A.elem[2][1] + P.z*A.elem[2][2] + P.w*A.elem[2][3];
    R.w = P.x*A.elem[3][0] + P.y*A.elem[3][1] + P.z*A.elem[3][2] + P.w*A.elem[3][3];
    
    return(R);
};

inline v4 operator*(Mat4x4 A, v4 P)
{
    v4 R = TransformVec(A, P);
    return(R);
};

inline v3 GetColumn(Mat4x4 A, u32 c)
{
    v3 result = {A.elem[0][c], A.elem[1][c], A.elem[2][c]};
    return(result);
};

inline v3 GetRow(Mat4x4 A, u32 r)
{
    v3 result = {A.elem[r][0], A.elem[r][1], A.elem[r][2]};
    return(result);
};

local_func Mat4x4 ColumnPicture3x3(v3 xAxis, v3 yAxis, v3 zAxis)
{
    Mat4x4 result =
    {
        {
            {xAxis.x, yAxis.x, zAxis.x, 0.0f},
            {xAxis.y, yAxis.y, zAxis.y, 0.0f},
            {xAxis.z, yAxis.z, zAxis.z, 0.0f},
            {   0.0f,    0.0f,    0.0f, 1.0f}
        }
    };
    
    return result;
};

local_func Mat4x4
RowPicture3x3(v3 xAxis, v3 yAxis, v3 zAxis)
{
    Mat4x4 result =
    {
        {
            {xAxis.x, xAxis.y, xAxis.z, 0.0f},
            {yAxis.x, yAxis.y, yAxis.z, 0.0f},
            {zAxis.x, zAxis.y, zAxis.z, 0.0f},
            {   0.0f,    0.0f,    0.0f, 1.0f}
        }
    };
    
    return result;
};

//Other v2 implementations
v2i::v2i(s32 x, s32 y)
: x(x)
, y(y)
{}

inline v2i
operator*(s32 a, v2i B)
{
    v2i result;
    
    result.x = a * B.x;
    result.y = a * B.y;
    
    return (result);
}

inline v2i
operator*(v2i B, s32 a)
{
    v2i result = a * B;
    
    return (result);
}

inline v2i&
operator*=(v2i& B, s32 a)
{
    B = a * B;
    
    return (B);
}

inline v2i
operator+(v2i a, v2i b)
{
    v2i result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return (result);
}

inline v2i&
operator+=(v2i& a, v2i b)
{
    a = a + b;
    
    return (a);
}

inline v2i&
operator+=(v2i& a, s32 b)
{
    a.x = a.x + b;
    a.y = a.y + b;
    
    return (a);
}

inline v2i&
operator-=(v2i& a, s32 b)
{
    a.x = a.x - b;
    a.y = a.y - b;
    
    return (a);
}

inline v2i&
operator-=(v2i& a, v2i b)
{
    a.x = a.x - b.x;
    a.y = a.y - b.y;
    
    return (a);
}

inline v2i
operator-(v2i a, v2i b)
{
    v2i result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return (result);
}

v3i::v3i(s32 x, s32 y, s32 z)
: x(x)
, y(y)
, z(z)
{}

/* all v3i suff */

v4i::v4i(s32 x, s32 y, s32 z, s32 w)
: x(x)
, y(y)
, z(z)
, w(w)
{}

inline f32
Max(f32 x, f32 y)
{
    f32 result = fmaxf(x, y);
    return result;
};

inline f32
Min(f32 x, f32 y)
{
    f32 result = fminf(x, y);
    return result;
};

inline f32
Mod(f32 x, f32 y)
{
    f32 result = fmodf(x, y);
    return result;
};

inline void
AbsoluteVal(s32&& Value)
{
    Value = abs(Value);
}

inline void
AbsoluteVal(f32&& Value)
{
    Value = (f32)fabs(Value);
}

f32 AbsoluteValFloat(f32 Value)
{
    Value = (f32)fabs(Value);
    return Value;
};

inline void
AbsoluteVal(v2&& Value)
{
    Value = { (f32)fabs(Value.x), (f32)fabs(Value.y) };
}

inline f32
ToRadians(f32 angleInDegrees)
{
    f32 angleInRadians = angleInDegrees * (PI / 180.0f);
    return angleInRadians;
};

inline f32
ToDegrees(f32 angleInRadians)
{
    f32 angleInDegrees = (180.0f / PI) * angleInRadians;
    return angleInDegrees;
};

inline f32
SinR(f32 angleInRadians)
{
    f32 result = sinf(angleInRadians);
    return result;
};

inline f32
InvSinR(f32 angleInRadians)
{
    f32 result = asinf(angleInRadians);
    return result;
};

inline f32
CosR(f32 AngleInRadians)
{
    f32 result = cosf(AngleInRadians);
    return result;
};

inline f32
InvCosR(f32 angleInRadians)
{
    f32 result = acosf(angleInRadians);
    return result;
};

inline f32
TanR(f32 AngleInRadians)
{
    f32 result = tanf(AngleInRadians);
    return result;
};

inline f32
InvTanR(f32 value)
{
    f32 result = atanf(value);
    return result;
};

inline f32
SinD(f32 angleInDegrees)
{
    f32 angleInRadians = ToRadians(angleInDegrees);
    f32 result = sinf(angleInRadians);
    
    return result;
};

inline f32
InvSinD(f32 angleInDegrees)
{
    f32 angleInRadians = ToRadians(angleInDegrees);
    f32 result = asinf(angleInRadians);
    
    return result;
};

inline f32
CosD(f32 angleInDegrees)
{
    f32 angleInRadians = ToRadians(angleInDegrees);
    f32 result = cosf(angleInRadians);
    
    return result;
};

inline f32
InvCosD(f32 angleInDegrees)
{
    f32 angleInRadians = ToRadians(angleInDegrees);
    f32 result = acosf(angleInRadians);
    
    return result;
};

inline f32
TanD(f32 angleInDegrees)
{
    f32 angleInRadians = ToRadians(angleInDegrees);
    f32 result = tanf(angleInRadians);
    
    return result;
};

inline f32
InvTanD(f32 angleInDegrees)
{
    f32 angleInRadians = ToRadians(angleInDegrees);
    f32 result = atanf(angleInRadians);
    
    return result;
};


inline void
Clamp(f32&& value, f32 min, f32 max)
{
    if(value < min)
    {
        value = min;
    }
    else if(value > max)
    {
        value = max;
    }
}

inline f64
Sqrt(f64 number)
{
    number = sqrt(number);
    return number;
};

inline f32
Sqrt(f32 number)
{
    number = sqrtf(number);
    return number;
};

inline sizet
RoundUp(sizet NumToRound, sizet Multiple)
{
    if (Multiple == 0)
        return NumToRound;
    
    sizet Remainder = NumToRound % Multiple;
    if (Remainder == 0)
        return NumToRound;
    
    return NumToRound + Multiple - Remainder;
};

inline sizet
RoundDown(sizet NumToRound, sizet Multiple)
{
    if (Multiple == 0)
        return NumToRound;
    
    sizet Remainder = NumToRound % Multiple;
    if (Remainder == 0)
        return NumToRound;
    
    return NumToRound - Remainder;
};

inline f32
Floor(f32 floatToFloor)
{
    f32 result = floorf(floatToFloor);
    return result;
};

inline s32
FloorF32ToI32(f32 floatToFloor)
{
    s32 result = (s32)floorf(floatToFloor);
    return result;
};

inline s32
CeilF32ToI32(f32 floatToCeil)
{
    s32 result = (s32)ceilf(floatToCeil);
    return result;
};

inline v2
PerpendicularOp(v2 A)
{
    v2 Result = {-A.y, A.x};
    return(Result);
}

inline f32
DotProduct(v2 a, v2 b)
{
    f32 result = a.x*b.x + a.y*b.y;
    
    return(result);
}

inline f32
DotProduct(v3 a, v3 b)
{
    f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
    
    return(result);
}

inline v2
Hadamard(v2 a, v2 b)
{
    v2 result = {a.x*b.x, a.y*b.y};
    
    return(result);
}

inline v3
Hadamard(v3 a, v3 b)
{
    v3 result = {a.x*b.x, a.y*b.y, a.z*b.z};
    
    return(result);
}

inline f32
CrossProduct(v2 a, v2 b)
{
    return a.x * b.y - a.y * b.x;
}

inline f32
MagnitudeSqd(v2 a)
{
    f32 result = DotProduct(a,a);
    
    return(result);
}

inline f32
MagnitudeSqd(v3 a)
{
    f32 result = DotProduct(a,a);
    
    return(result);
}

inline f32
Magnitude(v2 a)
{
    f32 result = Sqrt(MagnitudeSqd(a));
    return(result);
}

inline f32
Magnitude(v3 a)
{
    f32 result = Sqrt(MagnitudeSqd(a));
    return(result);
}

inline f32
Lerp(f32 a, f32 b, f32 t)
{
    f32 result = (1.0f - t)*a + t*b;
    
    return(result);
}

inline v4
Lerp(v4 a, v4 b, f32 t)
{
    v4 result = (1.0f - t)*a + t*b;
    
    return(result);
}

inline v2
Lerp(v2 a, v2 b, f32 t)
{
    v2 result = (1.0f - t)*a + t*b;
    
    return(result);
}

inline void
Normalize(v2&& a)
{
    a *= (1.0f / Magnitude(a));
};

inline void
Normalize(v3&& a)
{
    a *= (1.0f / Magnitude(a));
};

local_func
void ConvertNegativeToPositiveAngle_Radians(f32&& radianAngle)
{
    f32 circumferenceInRadians = 2*PI;
    radianAngle = Mod(radianAngle, circumferenceInRadians);
    if (radianAngle < 0) radianAngle += circumferenceInRadians;
};

local_func
void ConvertPositiveToNegativeAngle_Radians(f32&& radianAngle)
{
    if(radianAngle == 0.0f)
    {
        radianAngle = -6.28f;
    }
    else
    {
        f32 unitCircleCircumferenceInRadians = 2*PI;
        radianAngle = Mod(radianAngle, unitCircleCircumferenceInRadians);
        if (radianAngle > 0) radianAngle -= unitCircleCircumferenceInRadians;
    }
};

local_func
void ConvertToCorrectPositiveRadian(f32&& angle)
{
    f32 unitCircleCircumferenceInRadians = 2*PI;
    angle = Mod(angle, unitCircleCircumferenceInRadians);
};

Mat4x4 XRotation(f32 Angle)
{
    f32 c = CosR(Angle);
    f32 s = SinR(Angle);
    
    Mat4x4 R =
    {
        {
            {1, 0, 0, 0},
            {0, c,-s, 0},
            {0, s, c, 0},
            {0, 0, 0, 1}
        },
    };
    
    return(R);
}

inline Mat4x4
YRotation(f32 Angle)
{
    f32 c = CosR(Angle);
    f32 s = SinR(Angle);
    
    Mat4x4 R =
    {
        {
            { c, 0, s, 0},
            { 0, 1, 0, 0},
            {-s, 0, c, 0},
            { 0, 0, 0, 1}
        },
    };
    
    return(R);
}

inline Mat4x4
ZRotation(f32 Angle)
{
    f32 c = CosR(Angle);
    f32 s = SinR(Angle);
    
    Mat4x4 result =
    {
        {
            {c,-s, 0, 0},
            {s, c, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        },
    };
    
    return(result);
}

inline Mat4x4 ScaleMatrix(v3 scale)
{
    v3 s = scale;
    Mat4x4 result =
    {
        {
            {s.x, 0,   0,   0},
            {0,   s.y, 0,   0},
            {0,   0,   s.z, 0},
            {0,   0,   0,   1}
        },
    };
    
    return result;
};

local_func Mat4x4 Translate(Mat4x4 A, v4 T)
{
    Mat4x4 result = A;
    
    result.elem[0][3] += T.x;
    result.elem[1][3] += T.y;
    result.elem[2][3] += T.z;
    
    return result;
};

local_func Mat4x4 ProduceWorldTransformMatrix(v3 translation, v3 rotation, v3 scale)
{
    Mat4x4 result{};
    
    ConvertToCorrectPositiveRadian($(rotation.x));
    ConvertToCorrectPositiveRadian($(rotation.y));
    ConvertToCorrectPositiveRadian($(rotation.z));
    
    Mat4x4 xRotMatrix = XRotation(rotation.x);
    Mat4x4 yRotMatrix = YRotation(rotation.y);
    Mat4x4 zRotMatrix = ZRotation(rotation.z);
    Mat4x4 fullRotMatrix = xRotMatrix * yRotMatrix * zRotMatrix;
    
    Mat4x4 scaleMatrix = ScaleMatrix(scale);
    
    Mat4x4 rotScaleMatrix = fullRotMatrix * scaleMatrix;
    
    result = Translate(rotScaleMatrix, v4{translation, 1.0f});
    
    return result;
};

local_func Mat4x4 ProduceCameraTransformMatrix(v3 xAxis, v3 yAxis, v3 zAxis, v3 vecToTransform)
{
    Mat4x4 result = RowPicture3x3(xAxis, yAxis, zAxis);
    v4 vecToTransform_4d {vecToTransform, 1.0f};
    result = Translate(result, -(result*vecToTransform_4d));
    
    return result;
};

local_func Mat4x4 ProduceProjectionTransformMatrix_UsingFOV(f32 FOV_inDegrees, f32 aspectRatio, f32 nearPlane, f32 farPlane)
{
    f32 fov = ToRadians(FOV_inDegrees);
    f32 tanHalfFov = TanR(fov / 2.0f);
    f32 xScale = 1.0f / (tanHalfFov * aspectRatio);
    f32 yScale = 1.0f / tanHalfFov;
    
    f32 a = (-farPlane - nearPlane) / (nearPlane - farPlane);
    f32 b = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    
    Mat4x4 result =
    {
        {
            {xScale, 0,      0,  0},
            {  0,    yScale, 0,  0},
            {  0,    0,      a,  b},
            {  0,    0,      1,  0}
        },
    };
    
    return result;
};

inline Mat4x4 IdentityMatrix()
{
    Mat4x4 R =
    {
        {{1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}},
    };
    
    return(R);
}

inline Mat4x4
Transpose(Mat4x4 A)
{
    Mat4x4 R;
    
    for(int j = 0; j <= 3; ++j)
    {
        for(int i = 0; i <= 3; ++i)
        {
            R.elem[j][i] = A.elem[i][j];
        }
    }
    
    return(R);
}

bool _InvertMatrix(f32 m[16], f32 invOut[16])
{
    f32 inv[16];
    f32 det;
    int i;
    
    inv[0] = m[5]  * m[10] * m[15] -
        m[5]  * m[11] * m[14] -
        m[9]  * m[6]  * m[15] +
        m[9]  * m[7]  * m[14] +
        m[13] * m[6]  * m[11] -
        m[13] * m[7]  * m[10];
    
    inv[4] = -m[4]  * m[10] * m[15] +
        m[4]  * m[11] * m[14] +
        m[8]  * m[6]  * m[15] -
        m[8]  * m[7]  * m[14] -
        m[12] * m[6]  * m[11] +
        m[12] * m[7]  * m[10];
    
    inv[8] = m[4]  * m[9] * m[15] -
        m[4]  * m[11] * m[13] -
        m[8]  * m[5] * m[15] +
        m[8]  * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];
    
    inv[12] = -m[4]  * m[9] * m[14] +
        m[4]  * m[10] * m[13] +
        m[8]  * m[5] * m[14] -
        m[8]  * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];
    
    inv[1] = -m[1]  * m[10] * m[15] +
        m[1]  * m[11] * m[14] +
        m[9]  * m[2] * m[15] -
        m[9]  * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];
    
    inv[5] = m[0]  * m[10] * m[15] -
        m[0]  * m[11] * m[14] -
        m[8]  * m[2] * m[15] +
        m[8]  * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];
    
    inv[9] = -m[0]  * m[9] * m[15] +
        m[0]  * m[11] * m[13] +
        m[8]  * m[1] * m[15] -
        m[8]  * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];
    
    inv[13] = m[0]  * m[9] * m[14] -
        m[0]  * m[10] * m[13] -
        m[8]  * m[1] * m[14] +
        m[8]  * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];
    
    inv[2] = m[1]  * m[6] * m[15] -
        m[1]  * m[7] * m[14] -
        m[5]  * m[2] * m[15] +
        m[5]  * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];
    
    inv[6] = -m[0]  * m[6] * m[15] +
        m[0]  * m[7] * m[14] +
        m[4]  * m[2] * m[15] -
        m[4]  * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];
    
    inv[10] = m[0]  * m[5] * m[15] -
        m[0]  * m[7] * m[13] -
        m[4]  * m[1] * m[15] +
        m[4]  * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];
    
    inv[14] = -m[0]  * m[5] * m[14] +
        m[0]  * m[6] * m[13] +
        m[4]  * m[1] * m[14] -
        m[4]  * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];
    
    inv[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];
    
    inv[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];
    
    inv[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];
    
    inv[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];
    
    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    
    if (det == 0)
        return false;
    
    det = 1.0f / det;
    
    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
    
    return true;
}

Mat4x4 InvertMatrix(Mat4x4 matrixToInvert)
{
    Mat4x4 result_invertedMatrix{};
    
    f32 originalMatrix_arrayForm[16];
    
    for(int i{}, k{}; i < 4; ++i)
    {
        for(int j{}; j < 4; ++j)
        {
            originalMatrix_arrayForm[k++] = matrixToInvert.elem[i][j];
        };
    };
    
    f32 outMat[16];
    bool success = _InvertMatrix(originalMatrix_arrayForm, outMat);
    assert(success);//The original matrix cannot be inverted!
    
    for(int i{}, k{}; i < 4; ++i)
    {
        for(int j{}; j < 4; ++j)
        {
            result_invertedMatrix.elem[i][j] = outMat[k++];
        };
    };
    
    return result_invertedMatrix;
};

#endif //MY_MATH_IMPL