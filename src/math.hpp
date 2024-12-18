#pragma once
#include "pch.hpp"

struct Vec2 {
    union {
        float x, w;
    };

    union {
        float y, h;
    };

    static const Vec2 ZERO;
    static const Vec2 ONES;
    static const Vec2 X_AXIS;
    static const Vec2 Y_AXIS;

    Vec2() : x{0}, y{0} {}

    explicit Vec2(float value) : x{value}, y{value} {}

    Vec2(float x, float y) : x{x}, y{y} {}

    Vec2& operator+=(const Vec2& o);
    Vec2& operator-=(const Vec2& o);
    Vec2& operator*=(const Vec2& o);
    Vec2& operator/=(const Vec2& o);
    Vec2& operator*=(float value);
    Vec2& operator/=(float value);
    bool operator==(const Vec2& o) const;
    bool operator!=(const Vec2& o) const;

    float Dot(const Vec2&) const;
    float Cross(const Vec2&) const;
    Vec2 Normalize() const;
    void ToNormalize();
    float LengthSqrd() const;
    float Length() const;
};

Vec2 operator+(const Vec2& v1, const Vec2& v2);
Vec2 operator-(const Vec2& v1, const Vec2& v2);
Vec2 operator*(const Vec2& v1, const Vec2& v2);
Vec2 operator/(const Vec2& v1, const Vec2& v2);
Vec2 operator*(const Vec2& v1, float value);
Vec2 operator/(const Vec2& v1, float value);
Vec2 operator*(float value, const Vec2& v1);
Vec2 operator/(float value, const Vec2& v1);
Vec2 operator-(const Vec2& v);

struct Color {
    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Blue;
    static const Color Green;
    static const Color Yellow;

    float r = 1, g = 1, b = 1, a = 1;

    Color& operator*=(const Color& o);
    Color& operator*=(float value);
    Color& operator/=(float value);
    bool operator==(const Color& o) const;
    bool operator!=(const Color& o) const;
};

Color operator*(const Color& v1, const Color& v2);
Color operator/(const Color& v1, const Color& v2);
Color operator*(const Color& v1, float value);
Color operator/(const Color& v1, float value);
Color operator*(float value, const Color& v1);
Color operator/(float value, const Color& v1);

struct Circle {
    Vec2 center;
    float radius = 0;

    operator bool() const {
        return radius > 0;
    }
};

struct Rect {
    Vec2 position = {};
    Vec2 size = {};

    Rect() = default;

    Rect(const Vec2& pos, const Vec2& size) : position{pos}, size{size} {}

    Rect(float x, float y, float w, float h) : position{x, y}, size{w, h} {}

    static Rect CreateFromTopLeft(const Vec2& pos, const Vec2& size) {
        return {pos, size};
    }

    static Rect CreateFromCenter(const Vec2& center, const Vec2& halfSize) {
        return {center - halfSize, halfSize * 2.0f};
    }

    operator bool() const {
        return size.w > 0 && size.h > 0;
    }
};

constexpr float PI = 3.141592653589;

float Deg2Rad(float deg);
float Rad2Deg(float rad);

Vec2 Rotate(const Vec2& p, float degree);

template <typename T>
T Clamp(T a, T b, T value) {
    return value < a ? a : (value > b ? b : value);
}

template <typename T>
T Sign(T value) {
    if (value > 0) {
        return 1;
    } else if (value == 0) {
        return 0;
    } else {
        return -1;
    }
}

Vec2 ProjectOn(const Vec2& v, const Vec2& dir);