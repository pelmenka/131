#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define RTOD 180.0/3.1415
#define DTOR 3.1415/180.0

#include <initializer_list>
#include <cmath>

#ifndef _STL_ALGOBASE_H

template<typename T>
inline T& max(T &a, T &b)
{
    return ((a>b)?a:b);
}

template<typename T>
inline T& min(T &a, T &b)
{
    return ((a<b)?a:b);
}

template<typename T>
inline void swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

#else

using std::max;
using std::min;

#endif // _STL_ALGOBASE_H

template<typename T>
inline T clamp(const T &value, const T &mx, const T &mn)
{
    if(value > mx)
        return mx;
    if(value < mn)
        return mn;
    return value;
}

template<typename T>
inline T mix(const T &_1, const T &_2, double a) noexcept
{
    return _1*a+_2*(1.0-a);
}

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

template<typename T>
struct vec2
{
    T x, y;

    vec2():x(0),y(0){}

    template<typename S>
    vec2(const S &a):x(a),y(a){}

    template<typename S, typename Q>
    vec2(const S &a, const Q &b):x(a),y(b){}

    template<typename S>
    vec2(const vec2<S> &a):x(a.x),y(a.y){}

    vec2(std::initializer_list<T> a):vec2()
    {
        if(!a.size())
            return;
        auto i = a.begin();
        x = *i++;
        if(i == a.end()) return;
        y = *i;
    }

    template<typename S>
    vec2<T>& operator+=(const vec2<S> a) noexcept
    {
        x+=a.x;
        y+=a.y;
        return *this;
    }

    template <typename S>
    vec2<T> &operator-=(const vec2<S> a) noexcept
    {
        x-=a.x;
        y-=a.y;
        return *this;
    }

    template <typename S>
    vec2<T> &operator*=(const vec2<S> a) noexcept
    {
        x*=a.x;
        y*=a.y;
        return *this;
    }

    template <typename S>
    vec2<T> &operator*=(const S a) noexcept
    {
        x*=a;
        y*=a;
        return *this;
    }

    template <typename S>
    vec2<T> &operator/=(const vec2<S> a) noexcept
    {
        x/=a.x;
        y/=a.y;
        return *this;
    }

    template <typename S>
    vec2<T> &operator/=(const S a) noexcept
    {
        x/=a;
        y/=a;
        return *this;
    }

    template <typename S>
    vec2<T> &operator=(const vec2<S> a) noexcept
    {
        x=a.x;
        y=a.y;
        return *this;
    }

    template <typename S>
    inline vec2<T> operator+(const vec2<S> a) const noexcept
    {
        return vec2<T>(x+a.x, y+a.y);
    }

    template <typename S>
    inline vec2<T> operator-(const vec2<S> a) const noexcept
    {
        return vec2<T>(x-a.x, y-a.y);
    }

    inline vec2<T> operator-() const noexcept
    {
        return vec2<T>(-x, -y);
    }

    template <typename S>
    inline vec2<T> operator*(const S a) const noexcept
    {
        return vec2<T>(x*a, y*a);
    }

    template <typename S>
    inline vec2<T> operator*(const vec2<S> a) const noexcept
    {
        return vec2<T>(x*a.x, y*a.y);
    }

    template <typename S>
    inline vec2<T> operator/(const S a) const noexcept
    {
        return vec2<T>(x/a, y/a);
    }

    template <typename S>
    inline vec2<T> operator/(const vec2<S> a) const noexcept
    {
        return vec2<T>(x/a.x, y/a.y);
    }

    template <typename S>
    inline bool operator!=(const vec2<S> a) const noexcept
    {
        return x != a.x || y != a.y;
    }

    template <typename S>
    inline bool operator==(const vec2<S> a) const noexcept
    {
        return x == a.x && y == a.y;
    }

    template <typename S>
    inline bool operator>(const vec2<S> a) const noexcept
    {
        return x*x+y*y > a.x*a.x+a.y*a.y;
    }

    template <typename S>
    inline bool operator>=(const vec2<S> a) const noexcept
    {
        return x*x+y*y >= a.x*a.x+a.y*a.y;
    }

    template <typename S>
    inline bool operator<=(const vec2<S> a) const noexcept
    {
        return !(operator>(a));
    }

    template <typename S>
    inline bool operator<(const vec2<S> a) const noexcept
    {
        return !(operator>=(a));
    }

    inline T length() const noexcept{return sqrt(x*x+y*y);}
    inline vec2<T> normalize() const noexcept{return *this/length();}

    inline T &max(){return max(x, y);}
    inline T &min(){return min(x, y);}

    inline T *data(){return &x;}
    inline T &operator[](uint a){return *((&x)+a);}

    template <typename S>
    inline T dot(const vec2<S> &a) const noexcept
    {
        return T(x*a.x+y*a.y);
    }

    template <typename S>
    inline T cross(const vec2<S> &a) const noexcept
    {
        return T(x*a.y-y*a.x);
    }

    inline vec2<T> yx() const noexcept
    {
        return vec2<T>(y, x);
    }
};

template<typename T>
struct vec3
{
    T x, y, z;

    vec3():x(0),y(0),z(0){}



    template<typename A, typename B, typename C>
    vec3(const A _1, const B _2, const C _3):x(_1),y(_2),z(_3){}

    template<typename A>
    vec3(const vec2<A> &_1):x(_1.x),y(_1.y),z(0){}

    template<typename A, typename B>
    vec3(const vec2<A> &_1, const B _2 = 0):x(_1.x),y(_1.y),z(_2){}

    template<typename A, typename B>
    vec3(const A _1, const vec2<B> &_2):x(_1),y(_2.x),z(_2.y){}

    template<typename S>
    vec3(const vec3<S> &a):x(a.x),y(a.y),z(a.z){}

    template<typename S>
    vec3(const S &a):x(a),y(a),z(a){}

    vec3(std::initializer_list<T> a):vec3()
    {
        if(!a.size())
            return;
        auto i = a.begin();
        x = *i++;
        if(i == a.end()) return;
        y = *i++;
        if(i == a.end()) return;
        z = *i;
    }

    template <typename S>
    vec3<T> &operator+=(const vec3<S> a) noexcept
    {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    template <typename S>
    vec3<T> &operator-=(const vec3<S> a) noexcept
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    template <typename S>
    vec3<T> &operator*=(const vec3<S> a) noexcept
    {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        return *this;
    }

    template <typename S>
    vec3<T> &operator*=(const S a) noexcept
    {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    template <typename S>
    vec3<T> &operator/=(const vec3<S> a) noexcept
    {
        x /= a.x;
        y /= a.y;
        z /= a.z;
        return *this;
    }

    template <typename S>
    vec3<T> &operator/=(const S a) noexcept
    {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    }

    template <typename S>
    inline vec3<T> operator+(const vec3<S> a) const noexcept
    {
        return vec3<T>(x+a.x, y+a.y, z+a.z);
    }

    template <typename S>
    inline vec3<T> operator-(const vec3<S> a) const noexcept
    {
        return vec3<T>(x-a.x, y-a.y, z-a.z);
    }

    template <typename S>
    inline vec3<T> operator*(const S a) const noexcept
    {
        return vec3<T>(x*a, y*a, z*a);
    }

    template <typename S>
    inline vec3<T> operator*(const vec3<S> a) const noexcept
    {
        return vec3<T>(x*a.x, y*a.y, z*a.z);
    }

    template <typename S>
    inline vec3<T> operator/(const S a) const noexcept
    {
        return vec3<T>(x/a, y/a, z/a);
    }

    template <typename S>
    inline vec3<T> operator/(const vec3<S> a) const noexcept
    {
        return vec3<T>(x/a.x, y/a.y, z/a.z);
    }

    template <typename S>
    inline bool operator==(const vec3<S> a) const noexcept
    {
        return (x==a.x && y==a.y && z==a.z);
    }

    template <typename S>
    inline bool operator!=(const vec3<S> a) const noexcept
    {
        return !(operator==(a));
    }

    template <typename S>
    inline bool operator>(const vec3<S> a) const noexcept
    {
        return x*x+y*y+z*z > a.x*a.x+a.y*a.y+a.z*a.z;
    }

    template <typename S>
    inline bool operator>=(const vec3<S> a) const noexcept
    {
        return x*x+y*y+z*z >= a.x*a.x+a.y*a.y+a.z*a.z;
    }

    template <typename S>
    inline bool operator<=(const vec3<S> a) const noexcept
    {
        return !(operator>(a));
    }

    template <typename S>
    inline bool operator<(const vec3<S> a) const noexcept
    {
        return !(operator>=(a));
    }

    inline T &max(){return max(x, max(y, z));}
    inline T &min(){return min(x, min(y, z));}

    inline T *data(){return &x;}
    inline T &operator[](uint a){return *((&x)+a);}

    template <typename S>
    inline T dot(const vec3<S> &a) const noexcept
    {
        return T(x*a.x+y*a.y+z*a.z);
    }

    template <typename S>
    inline vec3<T> cross(const vec3<S> &a) const noexcept
    {
        return vec3<T>(y*a.z-a.y*z, z*a.x-a.z*x, x*a.y-y*a.x);
    }
};

template<typename T>
struct vec4
{
    T x, y, z, w;

    vec4():x(0),y(0),z(0),w(0){}

    template<typename S>
    vec4(const S &a):x(a),y(a),z(a),w(a){}

    template<typename A, typename B, typename C, typename D>
    vec4(const A &_1, const B &_2, const C &_3 = 0, const D &_4 = 0):x(_1),y(_2),z(_3),w(_4){}

    template<typename S>
    vec4(const vec2<S> &_1, const S &_2 = 0, const S &_3 = 0):x(_1.x),y(_1.y),z(_2),w(_3){}

    template<typename S>
    vec4(S _1, const vec2<S> &_2, const S &_3 = 0):x(_1),y(_2.x),z(_2.y),w(_3){}

    template<typename S>
    vec4(S _1, S _2, const vec2<S> &_3 = 0):x(_1),y(_2),z(_3.x),w(_3.y){}

    template<typename S>
    vec4(const vec2<S> &_1, const vec2<S> &_2):x(_1.x),y(_1.y),z(_2.x),w(_2.y){}

    template<typename S, typename Q>
    vec4(const vec3<S> &_1, Q _2 = 0):x(_1.x),y(_1.y),z(_1.z),w(_2){}

    template<typename S>
    vec4(S _1, const vec3<S> &_2):x(_1),y(_1.x),z(_1.y),w(_1.z){}

    vec4(std::initializer_list<T> a):vec4()
    {
        if(!a.size())
            return;
        auto i = a.begin();
        x = *i++;
        if(i == a.end()) return;
        y = *i++;
        if(i == a.end()) return;
        z = *i++;
        if(i == a.end()) return;
        w = *i;
    }

    template <typename S>
    inline vec4<T> &operator+=(const vec4<S> &a) noexcept
    {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
    }

    template <typename S>
    inline vec4<T> &operator-=(const vec4<S> &a) noexcept
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
    }

    template <typename S>
    inline vec4<T> &operator*=(const vec4<S> &a) noexcept
    {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        w *= a.w;
        return *this;
    }

    template <typename S>
    inline vec4<T> &operator/=(const vec4<S> &a) noexcept
    {
        x /= a.x;
        y /= a.y;
        z /= a.z;
        w /= a.w;
        return *this;
    }

    template <typename S>
    inline vec4<T> &operator/=(const S &a) noexcept
    {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
        return *this;
    }

    template <typename S>
    inline vec4<T> operator+(const vec4<S> &a) const noexcept
    {
        return vec4<T>(x+a.x, y+a.y, z+a.z, w+a.w);
    }

    template <typename S>
    inline vec4<T> operator-(const vec4<S> &a) const noexcept
    {
        return vec4<T>(x-a.x, y-a.y, z-a.z, w-a.w);
    }

    template <typename S>
    inline vec4<T> operator*(const S &a) const noexcept
    {
        return vec4<T>(x*a, y*a, z*a, w*a);
    }

    template <typename S>
    inline vec4<T> operator*(const vec4<S> &a) const noexcept
    {
        return vec4<T>(x*a.x, y*a.y, z*a.z, w*a.w);
    }

    template <typename S>
    inline vec4<T> operator/(const S &a) const noexcept
    {
        return vec4<T>(x/a, y/a, z/a, w/a);
    }

    template <typename S>
    inline vec4<T> operator/(const vec4<S> &a) const noexcept
    {
        return vec4<T>(x/a.x, y/a.y, z/a.z, w/a.w);
    }

    template <typename S>
    inline bool operator==(const vec4<S> &a) const noexcept
    {
        return (x==a.x && y==a.y && z==a.z && w==a.w);
    }

    template <typename S>
    inline bool operator!=(const vec4<S> &a) const noexcept
    {
        return !(operator==(a));
    }
};

template<typename T>
struct mat2
{
    T data[2][2];
    mat2(T val = 1)
    {
        for(uint i = 0; i < 2; i++)
            for(uint j = 0; j < 2; j++)
                if(i == j)
                    data[i][j] = val;
                else data[i][j] = 0;
    }

    mat2(std::initializer_list<T> a):mat2()
    {
        if(!a.size())
            return;
        auto it = a.begin();
        for(int i = 0; i < 4 && it != a.end(); i++)
            data[i/2][i%2] = *it++;
    }

    template<typename S>
    mat2<T> operator*(mat2<S> a)
    {
        mat2<T> result(0);
        for(uint i = 0; i < 2; i++)
            for(uint j = 0; j < 2; j++)
                for(uint k = 0; k < 2; k++)
                    result.data[i][j] += data[k][i]*a.data[j][k];

        return result;
    }

    template<typename S>
    vec2<T> operator*(vec2<S> a)
    {
        vec2<T> result;
        for(uint i = 0; i < 2; i++)
            for(uint j = 0; j < 2; j++)
                result[i] += data[j][i]*a[j];
        return result;
    }

};

typedef vec2<char> vec2c;
typedef vec2<short> vec2s;
typedef vec2<int> vec2i;
typedef vec2<uchar> vec2b;
typedef vec2<ushort> vec2us;
typedef vec2<uint> vec2u;
typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

typedef vec3<char> vec3c;
typedef vec3<int> vec3i;
typedef vec3<float> vec3f;

typedef vec3<uchar> vec3b;
typedef vec3<uint> vec3u;
typedef vec3<double> vec3d;

typedef vec4<char> vec4c;
typedef vec4<int> vec4i;
typedef vec4<float> vec4f;

typedef vec4<uchar> vec4b;
typedef vec4<uint> vec4u;
typedef vec4<double> vec4d;

typedef mat2<float> mat2f;

#endif // COMMON_H_INCLUDED
