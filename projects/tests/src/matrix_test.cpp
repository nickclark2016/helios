#include <helios/math/matrix.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(Matrix4f, DefaultConstructor)
{
    Matrix4f mat;
    for (i32 i = 0; i < 16; i++)
    {
        EXPECT_FLOAT_EQ(mat.data[i], 0.0f);
    }
}

TEST(Matrix4f, DiagonalConstructor)
{
    Matrix4f mat(1.0f);
    EXPECT_FLOAT_EQ(mat.data[0], 1.0f);
    EXPECT_FLOAT_EQ(mat.data[5], 1.0f);
    EXPECT_FLOAT_EQ(mat.data[10], 1.0f);
    EXPECT_FLOAT_EQ(mat.data[15], 1.0f);
}

TEST(Matrix4f, VectorConstructor)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f mat(col0, col1, col2, col3);

    EXPECT_EQ(mat.col0, col0);
    EXPECT_EQ(mat.col1, col1);
    EXPECT_EQ(mat.col2, col2);
    EXPECT_EQ(mat.col3, col3);
}

TEST(Matrix4f, PointerConstructor)
{
    f32 payload[] = {0.0f, 1.0f, 2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,
                     8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f};
    Matrix4f mat(payload);

    for (i32 i = 0; i < 16; i++)
    {
        EXPECT_FLOAT_EQ(payload[i], mat.data[i]);
    }
}

TEST(Matrix4f, CopyConstructor)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f orig(col0, col1, col2, col3);
    Matrix4f mat = orig;

    EXPECT_EQ(mat.col0, col0);
    EXPECT_EQ(mat.col1, col1);
    EXPECT_EQ(mat.col2, col2);
    EXPECT_EQ(mat.col3, col3);
}

TEST(Matrix4f, MoveConstructor)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f orig(col0, col1, col2, col3);
    Matrix4f mat = helios::move(orig);

    EXPECT_EQ(mat.col0, col0);
    EXPECT_EQ(mat.col1, col1);
    EXPECT_EQ(mat.col2, col2);
    EXPECT_EQ(mat.col3, col3);
}

TEST(Matrix4f, CopyAssignment)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f orig(col0, col1, col2, col3);
    Matrix4f mat;
    mat = orig;

    EXPECT_EQ(mat.col0, col0);
    EXPECT_EQ(mat.col1, col1);
    EXPECT_EQ(mat.col2, col2);
    EXPECT_EQ(mat.col3, col3);
}

TEST(Matrix4f, MoveAssignment)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f orig(col0, col1, col2, col3);
    Matrix4f mat;
    mat = helios::move(orig);

    EXPECT_EQ(mat.col0, col0);
    EXPECT_EQ(mat.col1, col1);
    EXPECT_EQ(mat.col2, col2);
    EXPECT_EQ(mat.col3, col3);
}

TEST(Matrix4f, DiagonalAssignment)
{
    Matrix4f mat;
    mat = 4.0f;

    EXPECT_FLOAT_EQ(mat.data[0], 4.0f);
    EXPECT_FLOAT_EQ(mat.data[5], 4.0f);
    EXPECT_FLOAT_EQ(mat.data[10], 4.0f);
    EXPECT_FLOAT_EQ(mat.data[15], 4.0f);
}

TEST(Matrix4f, Equality)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f lhs(col0, col1, col2, col3);
    Matrix4f rhs(col0, col1, col2, col3);

    ASSERT_TRUE(lhs == rhs);
    ASSERT_FALSE(lhs != rhs);
    EXPECT_EQ(lhs, rhs);
}

TEST(Matrix4f, Inequality)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f lhs(col1, col0, col2, col3);
    Matrix4f rhs(col0, col1, col2, col3);

    ASSERT_TRUE(lhs != rhs);
    ASSERT_FALSE(lhs == rhs);
    EXPECT_NE(lhs, rhs);
}

TEST(Matrix4f, PlusEquals)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f lhs(col1, col0, col2, col3);
    Matrix4f rhs(col0, col1, col2, col3);

    lhs += rhs;
    EXPECT_EQ(lhs,
              Matrix4f(col1 + col0, col0 + col1, col2 + col2, col3 + col3));
}

TEST(Matrix4f, MinusEquals)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f lhs(col1, col0, col2, col3);
    Matrix4f rhs(col0, col1, col2, col3);

    lhs -= rhs;
    EXPECT_EQ(lhs,
              Matrix4f(col1 - col0, col0 - col1, col2 - col2, col3 - col3));
}

TEST(Matrix4f, MultiplyEquals)
{
    Matrix4f lhs({5.0f, 0.0f, 3.0f, 1.0f}, {2.0f, 6.0f, 8.0f, 8.0f},
                 {6.0f, 2.0f, 1.0f, 5.0f}, {1.0f, 0.0f, 4.0f, 6.0f});
    Matrix4f rhs({7.0f, 1.0f, 9.0f, 5.0f}, {5.0f, 8.0f, 4.0f, 3.0f},
                 {8.0f, 2.0f, 3.0f, 7.0f}, {0.0f, 6.0f, 8.0f, 9.0f});
    Matrix4f res({96.0f, 24.0f, 58.0f, 90.0f}, {68.0f, 56.0f, 95.0f, 107.0f},
                 {69.0f, 18.0f, 71.0f, 81.0f}, {69.0f, 52.0f, 92.0f, 142.0f});

    lhs *= rhs;
    EXPECT_EQ(lhs, res);
}

TEST(Matrix4f, Inverse)
{
    Matrix4f mat({0.0f, 3.0f, 0.0f, 0.0f}, {2.0f, 7.0f, 0.0f, 1.0f},
                 {8.0f, 1.0f, 1.0f, 0.0f}, {6.0f, 0.0f, 2.0f, 1.0f});
    Matrix4f expected({-1.0f / 4.0f, 1.0f / 12.0f, 1.0f / 6.0f, -1.0f / 12.0f},
                      {1.0f / 3.0f, 0.0f, 0.0f, 0.0f},
                      {5.0f / 3.0f, -2.0f / 3.0f, -1.0f / 3.0f, 2.0f / 3.0f},
                      {-11.0f / 6.0f, 5.0f / 6.0f, -1.0f / 3.0f, 1.0f / 6.0f});
    EXPECT_EQ(mat.inverse(), expected);
    EXPECT_EQ(inverse(mat), mat.inverse());
}

TEST(Matrix4f, Addition)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f lhs(col1, col0, col2, col3);
    Matrix4f rhs(col0, col1, col2, col3);

    Matrix4f res = lhs + rhs;
    EXPECT_EQ(res,
              Matrix4f(col1 + col0, col0 + col1, col2 + col2, col3 + col3));
}

TEST(Matrix4f, Subtraction)
{
    Vector4f col0 = {0.0f, 1.0f, 2.0f, 3.0f};
    Vector4f col1 = {4.0f, 5.0f, 6.0f, 7.0f};
    Vector4f col2 = {8.0f, 9.0f, 10.0f, 11.0f};
    Vector4f col3 = {12.0f, 13.0f, 14.0f, 15.0f};

    Matrix4f lhs(col1, col0, col2, col3);
    Matrix4f rhs(col0, col1, col2, col3);

    Matrix4f res = lhs - rhs;
    EXPECT_EQ(res,
              Matrix4f(col1 - col0, col0 - col1, col2 - col2, col3 - col3));
}

TEST(Matrix4f, Multiplication)
{
    Matrix4f lhs({5.0f, 0.0f, 3.0f, 1.0f}, {2.0f, 6.0f, 8.0f, 8.0f},
                 {6.0f, 2.0f, 1.0f, 5.0f}, {1.0f, 0.0f, 4.0f, 6.0f});
    Matrix4f rhs({7.0f, 1.0f, 9.0f, 5.0f}, {5.0f, 8.0f, 4.0f, 3.0f},
                 {8.0f, 2.0f, 3.0f, 7.0f}, {0.0f, 6.0f, 8.0f, 9.0f});
    Matrix4f expected(
        {96.0f, 24.0f, 58.0f, 90.0f}, {68.0f, 56.0f, 95.0f, 107.0f},
        {69.0f, 18.0f, 71.0f, 81.0f}, {69.0f, 52.0f, 92.0f, 142.0f});

    Matrix4f res = lhs * rhs;
    EXPECT_EQ(res, expected);
}

TEST(Matrix4f, VectorMultiplication)
{
    Matrix4f lhs({5.0f, 0.0f, 3.0f, 1.0f}, {2.0f, 6.0f, 8.0f, 8.0f},
                 {6.0f, 2.0f, 1.0f, 5.0f}, {1.0f, 0.0f, 4.0f, 6.0f});
    Vector4f rhs({7.0f, 1.0f, 9.0f, 5.0f});
    Vector4f expected(96.0f, 24.0f, 58.0f, 90.0f);

    Vector4f res = lhs * rhs;
    EXPECT_EQ(res, expected);
}

TEST(Matrix4f, ScalarMultiplication)
{
    Matrix4f mat({5.0f, 0.0f, 3.0f, 1.0f}, {2.0f, 6.0f, 8.0f, 8.0f},
                 {6.0f, 2.0f, 1.0f, 5.0f}, {1.0f, 0.0f, 4.0f, 6.0f});
    f32 scalar = 2.0f;
    Matrix4f expected({10.0f, 0.0f, 6.0f, 2.0f}, {4.0f, 12.0f, 16.0f, 16.0f},
                      {12.0f, 4.0f, 2.0f, 10.0f}, {2.0f, 0.0f, 8.0f, 12.0f});

    EXPECT_EQ(mat * scalar, expected);
    EXPECT_EQ(scalar * mat, expected);
}
