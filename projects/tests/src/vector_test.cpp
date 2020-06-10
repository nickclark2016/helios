#include <helios/math/vector.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(Vector2f, DefaultConstructor)
{
    Vector2f vec;
    EXPECT_EQ(vec.x, 0);
    EXPECT_EQ(vec.y, 0);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.x, vec.data[0]);
    EXPECT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, ScalarConstructor)
{
    Vector2f vec(2.0f);
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 2.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, ComponentConstructor)
{
    Vector2f vec(2.0f, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, CopyConstructor)
{
    Vector2f original(2.0f, 3.0f);
    Vector2f vec = original;
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, MoveConstructor)
{
    Vector2f original(2.0f, 3.0f);
    Vector2f vec = helios::move(original);
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, ScalarCopyAssignment)
{
    Vector2f vec(1.0f, 2.0f);
    vec = 3.0f;
    EXPECT_FLOAT_EQ(vec.x, 3.0f);
    EXPECT_FLOAT_EQ(vec.y, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, CopyAssignment)
{
    Vector2f original(2.0f, 3.0f);
    Vector2f vec(1.0f, 2.0f);
    vec = original;
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, MoveAssignment)
{
    Vector2f original(2.0f, 3.0f);
    Vector2f vec(1.0f, 2.0f);
    vec = helios::move(original);
    EXPECT_FLOAT_EQ(vec.x, 2.0f);
    EXPECT_FLOAT_EQ(vec.y, 3.0f);
    EXPECT_FLOAT_EQ(vec.x, vec.r);
    EXPECT_FLOAT_EQ(vec.y, vec.g);
    EXPECT_FLOAT_EQ(vec.x, vec.u);
    EXPECT_FLOAT_EQ(vec.y, vec.v);
    EXPECT_FLOAT_EQ(vec.x, vec.data[0]);
    EXPECT_FLOAT_EQ(vec.y, vec.data[1]);
}

TEST(Vector2f, LeftScalarEquality)
{
    Vector2f vec(0.0f);
    float scalar = 0.0f;
    EXPECT_EQ(scalar, vec);
}

TEST(Vector2f, RightScalarEquality)
{
    Vector2f vec(0.0f);
    float scalar = 0.0f;
    EXPECT_EQ(vec, scalar);
}

TEST(Vector2f, Equality)
{
    Vector2f vec(0.0f);
    Vector2f vec2(0.0f, 0.0f);
    EXPECT_EQ(vec, vec2);
}

TEST(Vector2f, LeftScalarInequality)
{
    Vector2f vec(0.0f);
    float scalar = 1.0f;
    EXPECT_NE(scalar, vec);
}

TEST(Vector2f, RightScalarInequality)
{
    Vector2f vec(0.0f);
    float scalar = 1.0f;
    EXPECT_NE(vec, scalar);
}

TEST(Vector2f, Inequality)
{
    Vector2f vec(0.0f);
    Vector2f scalar(1.0f);
    EXPECT_NE(vec, scalar);
}

TEST(Vector2f, ScalarPlusEquals)
{
    Vector2f vec(1.0f, 2.0f);
    vec += 3.0f;
    EXPECT_FLOAT_EQ(vec.data[0], 4.0f);
    EXPECT_FLOAT_EQ(vec.data[1], 5.0f);
}

TEST(Vector2f, VectorPlusEquals)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f vec2(3.0f, 4.0f);
    vec += vec2;
    EXPECT_FLOAT_EQ(vec.data[0], 4.0f);
    EXPECT_FLOAT_EQ(vec.data[1], 6.0f);
}

TEST(Vector2f, LeftScalarAddition)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = 2.0f + vec;
    EXPECT_FLOAT_EQ(sum.data[0], 3.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 4.0f);
}

TEST(Vector2f, RightScalarAddition)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec + 2.0f;
    EXPECT_FLOAT_EQ(sum.data[0], 3.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 4.0f);
}

TEST(Vector2f, Addition)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec + Vector2f(2.0f);
    EXPECT_FLOAT_EQ(sum.data[0], 3.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 4.0f);
}

TEST(Vector2f, ScalarMinusEquals)
{
    Vector2f vec(1.0f, 2.0f);
    vec -= 3.0f;
    EXPECT_FLOAT_EQ(vec.data[0], -2.0f);
    EXPECT_FLOAT_EQ(vec.data[1], -1.0f);
}

TEST(Vector2f, VectorMinusEquals)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f vec2(3.0f, 4.0f);
    vec -= vec2;
    EXPECT_FLOAT_EQ(vec.data[0], -2.0f);
    EXPECT_FLOAT_EQ(vec.data[1], -2.0f);
}

TEST(Vector2f, LeftScalarSubtraction)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = 2.0f - vec;
    EXPECT_FLOAT_EQ(sum.data[0], 1.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 0.0f);
}

TEST(Vector2f, RightScalarSubtraction)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec - 2.0f;
    EXPECT_FLOAT_EQ(sum.data[0], -1.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 0.0f);
}

TEST(Vector2f, Subtraction)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec - Vector2f(2.0f);
    EXPECT_FLOAT_EQ(sum.data[0], -1.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 0.0f);
}

TEST(Vector2f, ScalarMultiplicationEquals)
{
    Vector2f vec(1.0f, 2.0f);
    vec *= 3.0f;
    EXPECT_FLOAT_EQ(vec.data[0], 3.0f);
    EXPECT_FLOAT_EQ(vec.data[1], 6.0f);
}

TEST(Vector2f, VectorMultiplicationEquals)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f vec2(3.0f, 4.0f);
    vec *= vec2;
    EXPECT_FLOAT_EQ(vec.data[0], 3.0f);
    EXPECT_FLOAT_EQ(vec.data[1], 8.0f);
}

TEST(Vector2f, LeftScalarMultiplication)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = 2.0f * vec;
    EXPECT_FLOAT_EQ(sum.data[0], 2.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 4.0f);
}

TEST(Vector2f, RightScalarMultiplication)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec * 2.0f;
    EXPECT_FLOAT_EQ(sum.data[0], 2.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 4.0f);
}

TEST(Vector2f, Multiplication)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec * Vector2f(2.0f);
    EXPECT_FLOAT_EQ(sum.data[0], 2.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 4.0f);
}

TEST(Vector2f, ScalarDivisionEquals)
{
    Vector2f vec(1.0f, 2.0f);
    vec /= 3.0f;
    EXPECT_FLOAT_EQ(vec.data[0], 1.0f / 3.0f);
    EXPECT_FLOAT_EQ(vec.data[1], 2.0f / 3.0f);
}

TEST(Vector2f, VectorDivisionEquals)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f vec2(3.0f, 4.0f);
    vec /= vec2;
    EXPECT_FLOAT_EQ(vec.data[0], 1.0f / 3.0f);
    EXPECT_FLOAT_EQ(vec.data[1], 2.0f / 4.0f);
}

TEST(Vector2f, LeftScalarDivision)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = 2.0f / vec;
    EXPECT_FLOAT_EQ(sum.data[0], 2.0f / 1.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 2.0f / 2.0f);
}

TEST(Vector2f, RightScalarDivision)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec / 2.0f;
    EXPECT_FLOAT_EQ(sum.data[0], 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 2.0f / 2.0f);
}

TEST(Vector2f, Division)
{
    Vector2f vec(1.0f, 2.0f);
    Vector2f sum = vec / Vector2f(2.0f);
    EXPECT_FLOAT_EQ(sum.data[0], 1.0f / 2.0f);
    EXPECT_FLOAT_EQ(sum.data[1], 2.0f / 2.0f);
}

TEST(Vector2f, AbsoluteValue)
{
    Vector2f vec(-1.0f, 2.0f);
    Vector2f vec2(1.0f, -2.0f);
    Vector2f vec3(-1.0f, -2.0f);

    EXPECT_EQ(abs(vec), Vector2f(1.0f, 2.0f));
    EXPECT_EQ(abs(vec2), Vector2f(1.0f, 2.0f));
    EXPECT_EQ(abs(vec3), Vector2f(1.0f, 2.0f));
}

TEST(Vector2f, Angle)
{
    Vector2f right(1.0f, 0.0f);
    Vector2f up(0.0f, 1.0f);
    EXPECT_EQ(angle(right, up), radians(90.0f));
    EXPECT_EQ(angle(right, up), right.angle(up));

    Vector2f downLeft(-1.0f, -1.0f);
    EXPECT_EQ(angle(right, downLeft), radians(135.0f));
    EXPECT_EQ(angle(right, downLeft), right.angle(downLeft));
}

TEST(Vector2f, DotProduct)
{
    Vector2f left(1.0f, 3.0f);
    Vector2f right(4.0f, 2.0f);
    EXPECT_FLOAT_EQ(left.dot(right), 10.0f);
    EXPECT_EQ(left.dot(right), dot(left, right));
}

TEST(Vector2f, EuclidianNorm)
{
    Vector2f vec(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(vec.euclidianNorm(), sqrtf(1.0f + 1.0f));
    EXPECT_EQ(vec.euclidianNorm(), euclidianNorm(vec));
}

TEST(Vector2f, Length)
{
    Vector2f vec(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(vec.length(), sqrtf(1.0f + 1.0f));
    EXPECT_EQ(vec.length(), length(vec));
}

TEST(Vector2f, Magnitude)
{
    Vector2f vec(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(vec.magnitude(), sqrtf(1.0f + 1.0f));
    EXPECT_EQ(vec.magnitude(), magnitude(vec));
}

TEST(Vector2f, Norm1)
{
    Vector2f vec(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(vec.norm1(), 2.0f);
    EXPECT_EQ(vec.norm1(), norm1(vec));
}

TEST(Vector2f, Norm2)
{
    Vector2f vec(1.0f, 1.0f);
    EXPECT_FLOAT_EQ(vec.norm2(), sqrtf(1.0f + 1.0f));
    EXPECT_EQ(vec.norm2(), norm2(vec));
}

TEST(Vector2f, Reflect)
{
    Vector2f vec(1.0f, -1.0f);
    Vector2f line(0.0f, 1.0f);
    Vector2f reflection(1.0f, 1.0f);
    EXPECT_EQ(vec.reflect(line), reflection);
    EXPECT_EQ(reflect(vec, line), reflection);
}

TEST(Vector3f, DefaultConstructor)
{
    Vector3f vec;
    EXPECT_EQ(vec.data[0], 0.0f);
    EXPECT_EQ(vec.data[1], 0.0f);
    EXPECT_EQ(vec.data[2], 0.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, ScalarConstructor)
{
    Vector3f vec(1.0f);
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 1.0f);
    EXPECT_EQ(vec.data[2], 1.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, ComponentConstructor)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 3.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, Vector2fWithDefaultConstructor)
{
    Vector3f vec(Vector2f(1.0f, 2.0f));
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 0.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, Vector2fAndScalarConstructor)
{
    Vector3f vec(Vector2f(1.0f, 2.0f), 3.0f);
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 3.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, CopyConstructor)
{
    Vector3f original(1.0f, 2.0f, 3.0f);
    Vector3f vec(original);
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 3.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, MoveConstructor)
{
    Vector3f original(1.0f, 2.0f, 3.0f);
    Vector3f vec(helios::move(original));
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 3.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, ScalarCopyAssignment)
{
    Vector3f vec;
    vec = 2.0f;
    EXPECT_EQ(vec.data[0], 2.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 2.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, Vector2fCopyAssignment)
{
    Vector2f orig(1.0f, 2.0f);
    Vector3f vec(3.0f);
    vec = orig;
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 0.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, CopyAssignment)
{
    Vector3f orig(1.0f, 2.0f, 3.0f);
    Vector3f vec(3.0f);
    vec = orig;
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 3.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, Vector2fMoveAssignment)
{
    Vector2f orig(1.0f, 2.0f);
    Vector3f vec(3.0f);
    vec = helios::move(orig);
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 0.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, MoveAssignment)
{
    Vector3f orig(1.0f, 2.0f, 3.0f);
    Vector3f vec(3.0f);
    vec = helios::move(orig);
    EXPECT_EQ(vec.data[0], 1.0f);
    EXPECT_EQ(vec.data[1], 2.0f);
    EXPECT_EQ(vec.data[2], 3.0f);
    EXPECT_EQ(vec.data[0], vec.x);
    EXPECT_EQ(vec.data[1], vec.y);
    EXPECT_EQ(vec.data[2], vec.z);
    EXPECT_EQ(vec.x, vec.r);
    EXPECT_EQ(vec.y, vec.g);
    EXPECT_EQ(vec.z, vec.b);
    EXPECT_EQ(vec.x, vec.u);
    EXPECT_EQ(vec.y, vec.v);
    EXPECT_EQ(vec.z, vec.w);
}

TEST(Vector3f, LeftScalarEquality)
{
    Vector3f vec(2.0f);
    EXPECT_EQ(2.0f, vec);
}

TEST(Vector3f, RightScalarEquality)
{
    Vector3f vec(2.0f);
    EXPECT_EQ(vec, 2.0f);
}

TEST(Vector3f, Equality)
{
    Vector3f vec(2.0f);
    EXPECT_EQ(vec, Vector3f(2.0f));
}

TEST(Vector3f, LeftScalarInequality)
{
    Vector3f vec(2.0f);
    EXPECT_NE(1.0f, vec);
}

TEST(Vector3f, RightScalarInquality)
{
    Vector3f vec(2.0f);
    EXPECT_NE(vec, 1.0f);
}

TEST(Vector3f, Inequality)
{
    Vector3f vec(2.0f);
    EXPECT_NE(vec, Vector3f(1.0f));
}

TEST(Vector3f, ScalarPlusEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec += 2.0f;
    EXPECT_EQ(vec, Vector3f(3.0f, 4.0f, 5.0f));
}

TEST(Vector3f, VectorPlusEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec += Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(3.0f, 5.0f, 7.0f));
}

TEST(Vector3f, LeftScalarAddition)
{
    Vector3f vec = 2.0f + Vector3f(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec, Vector3f(3.0f, 4.0f, 5.0f));
}

TEST(Vector3f, RightScalarAddition)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) + 2.0f;
    EXPECT_EQ(vec, Vector3f(3.0f, 4.0f, 5.0f));
}

TEST(Vector3f, Addition)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) + Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(3.0f, 5.0f, 7.0f));
}

TEST(Vector3f, ScalarMinusEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec -= 2.0f;
    EXPECT_EQ(vec, Vector3f(-1.0f, 0.0f, 1.0f));
}

TEST(Vector3f, VectorMinusEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec -= Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(-1.0f, -1.0f, -1.0f));
}

TEST(Vector3f, LeftScalarSubtraction)
{
    Vector3f vec = 2.0f - Vector3f(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec, Vector3f(1.0f, 0.0f, -1.0f));
}

TEST(Vector3f, RightScalarSubtraction)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) - 2.0f;
    EXPECT_EQ(vec, Vector3f(-1.0f, 0.0f, 1.0f));
}

TEST(Vector3f, Subtraction)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) - Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(-1.0f, -1.0f, -1.0f));
}

TEST(Vector3f, ScalarMultiplicationEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec *= 2.0f;
    EXPECT_EQ(vec, Vector3f(2.0f, 4.0f, 6.0f));
}

TEST(Vector3f, VectorMultiplicationEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec *= Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(2.0f, 6.0f, 12.0f));
}

TEST(Vector3f, LeftScalarMultiplication)
{
    Vector3f vec = 2.0f * Vector3f(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec, Vector3f(2.0f, 4.0f, 6.0f));
}

TEST(Vector3f, RightScalarMultiplication)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) * 2.0f;
    EXPECT_EQ(vec, Vector3f(2.0f, 4.0f, 6.0f));
}

TEST(Vector3f, Multiplication)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) * Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(2.0f, 6.0f, 12.0f));
}

TEST(Vector3f, ScalarDivisionEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec /= 2.0f;
    EXPECT_EQ(vec, Vector3f(1.0f / 2.0f, 2.0f / 2.0f, 3.0f / 2.0f));
}

TEST(Vector3f, VectorDivisionEquals)
{
    Vector3f vec(1.0f, 2.0f, 3.0f);
    vec /= Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(1.0f / 2.0f, 2.0f / 3.0f, 3.0f / 4.0f));
}

TEST(Vector3f, LeftScalarDivision)
{
    Vector3f vec = 2.0f / Vector3f(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec, Vector3f(2.0f / 1.0f, 2.0f / 2.0f, 2.0f / 3.0f));
}

TEST(Vector3f, RightScalarDivision)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) / 2.0f;
    EXPECT_EQ(vec, Vector3f(1.0f / 2.0f, 2.0f / 2.0f, 3.0f / 2.0f));
}

TEST(Vector3f, Division)
{
    Vector3f vec = Vector3f(1.0f, 2.0f, 3.0f) / Vector3f(2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec, Vector3f(1.0f / 2.0f, 2.0f / 3.0f, 3.0f / 4.0f));
}

TEST(Vector3f, AbsoluteValue)
{
    Vector3f vec(-1.0f, -2.0f, 3.0f);
    Vector3f res(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec.abs(), res);
    EXPECT_EQ(abs(vec), res);
}

TEST(Vector3f, Angle)
{
    Vector3f x(1.0f, 0.0f, 0.0f);
    Vector3f y(0.0f, 1.0f, 0.0f);
    Vector3f z(0.0f, 0.0f, 1.0f);
    Vector3f dlb(-1.0, -1.0f, -1.0f);
    EXPECT_FLOAT_EQ(x.angle(y), radians(90.0f));
    EXPECT_FLOAT_EQ(x.angle(z), radians(90.0f));
    EXPECT_FLOAT_EQ(x.angle(dlb), acosf(-1.0f / sqrtf(3)));
    EXPECT_EQ(angle(x, y), x.angle(y));
    EXPECT_EQ(angle(x, z), x.angle(z));
    EXPECT_EQ(angle(x, dlb), x.angle(dlb));
}

TEST(Vector3f, CrossProduct)
{
    Vector3f x(1.0f, 0.0f, 0.0f);
    Vector3f y(0.0f, 1.0f, 0.0f);
    Vector3f res(0.0f, 0.0f, 1.0f);
    EXPECT_EQ(x.cross(y), res);
    EXPECT_EQ(cross(x, y), res);
}

TEST(Vector3f, DotProduct)
{
    Vector3f x(1.0f, 2.0f, 3.0f);
    Vector3f y(2.0f, -1.0f, 3.0f);
    EXPECT_FLOAT_EQ(x.dot(y), 9.0f);
    EXPECT_EQ(dot(x, y), x.dot(y));
}

TEST(Vector3f, EuclidianNorm)
{
    Vector3f v(2.0f, 1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v.euclidianNorm(), 3.0f);
    EXPECT_EQ(euclidianNorm(v), v.euclidianNorm());
}

TEST(Vector3f, Length)
{
    Vector3f v(2.0f, 1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v.length(), 3.0f);
    EXPECT_EQ(length(v), v.length());
}

TEST(Vector3f, Magnitude)
{
    Vector3f v(2.0f, 1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v.magnitude(), 3.0f);
    EXPECT_EQ(magnitude(v), v.magnitude());
}

TEST(Vector3f, Norm1)
{
    Vector3f v(2.0f, 1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v.norm1(), 9.0f);
    EXPECT_EQ(norm1(v), v.norm1());
}

TEST(Vector3f, Norm2)
{
    Vector3f v(2.0f, 1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v.norm2(), 3.0f);
    EXPECT_EQ(norm2(v), v.norm2());
}

TEST(Vector3f, Reflect)
{
    Vector3f vec(1.0f, -1.0f, 0.0f);
    Vector3f line(0.0f, 1.0f, 0.0f);
    Vector3f reflection(1.0f, 1.0f, 0.0f);
    EXPECT_EQ(vec.reflect(line), reflection);
    EXPECT_EQ(reflect(vec, line), reflection);
}
