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

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
