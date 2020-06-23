#include <helios/math/transformations.hpp>

#include <gtest/gtest.h>

using namespace helios;

TEST(Transformations, Translate)
{
    // translate by (5, 2, 4)
    Matrix4f translation = translate(Vector3f(5.0f, 2.0f, 4.0f));
    // check the last column for the contents
    EXPECT_EQ(translation.col3, Vector4f(5.0f, 2.0f, 4.0f, 1.0f));

    // Check the translation against a point at the origin
    Vector4f origin(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(translation * origin, Vector4f(5.0f, 2.0f, 4.0f, 1.0f));

    // Check the translation against a point in Quadrant I
    Vector4f q1(1.0f, 2.0f, 0.0f, 1.0f);
    EXPECT_EQ(translation * q1, Vector4f(6.0f, 4.0f, 4.0f, 1.0f));

    // Check the translation against a point in Quadrant II
    Vector4f q2(-1.0f, 2.0f, 0.0f, 1.0f);
    EXPECT_EQ(translation * q2, Vector4f(4.0f, 4.0f, 4.0f, 1.0f));

    // Check the translation against a point in Quadrant III
    Vector4f q3(-1.0f, -2.0f, 0.0f, 1.0f);
    EXPECT_EQ(translation * q3, Vector4f(4.0f, 0.0f, 4.0f, 1.0f));

    // Check the translation against a point in Quadrant IV
    Vector4f q4(1.0f, -2.0f, 0.0f, 1.0f);
    EXPECT_EQ(translation * q4, Vector4f(6.0f, 0.0f, 4.0f, 1.0f));
}

TEST(Transformations, Rotate)
{
    // rotate around the origin 90 degrees on the z axis
    Matrix4f rotation = rotate(Vector3f(0.0f, 0.0f, 90.0f));

    // Rotate a unit vector on the positive x, expect to move to positive y
    Vector4f positiveX = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    Vector4f actualY = rotation * positiveX;
    EXPECT_NEAR(actualY.x, 0.0f, 0.0001f);
    EXPECT_NEAR(actualY.y, 1.0f, 0.0001f);
    EXPECT_NEAR(actualY.z, 0.0f, 0.0001f);
    EXPECT_NEAR(actualY.w, 1.0f, 0.0001f);

    // rotate around the origin 90 degrees on the z axis
    rotation = rotate(Vector3f(0.0f, 90.0f, 0.0f));

    // Rotate a unit vector on the positive x, expect to move to negative z
    Vector4f actualZ = rotation * positiveX;
    EXPECT_NEAR(actualZ.x, 0.0f, 0.0001f);
    EXPECT_NEAR(actualZ.y, 0.0f, 0.0001f);
    EXPECT_NEAR(actualZ.z, -1.0f, 0.0001f);
    EXPECT_NEAR(actualZ.w, 1.0f, 0.0001f);

    // rotate around the origin 90 degrees on the x axis
    rotation = rotate(Vector3f(90.0f, 0.0f, 0.0f));

    // Rotate a unit vector on the positive x, expect to move to negative z
    Vector4f actualX = rotation * positiveX;
    EXPECT_NEAR(actualX.x, 1.0f, 0.0001f);
    EXPECT_NEAR(actualX.y, 0.0f, 0.0001f);
    EXPECT_NEAR(actualX.z, 0.0f, 0.0001f);
    EXPECT_NEAR(actualX.w, 1.0f, 0.0001f);
}

TEST(Transformations, Scale)
{
    // scale 1 on x, 1 on y, 2 on z
    Matrix4f sca = scale(Vector3f(1.0f, 1.0f, 2.0f));

    Vector4f vec = Vector4f(1.0f, 2.0f, -4.0f, 1.0f);
    Vector4f res = sca * vec;
    EXPECT_NEAR(res.x, 1.0f, 0.0001f);
    EXPECT_NEAR(res.y, 2.0f, 0.0001f);
    EXPECT_NEAR(res.z, -8.0f, 0.0001f);
    EXPECT_NEAR(res.w, 1.0f, 0.0001f);
}

TEST(Transformations, Transform)
{
    Matrix4f transformation =
        transform(Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 90.0f),
                  Vector3f(2.0f, 1.0f, 1.0f));
    Vector4f vec(1.0f, 0.0f, 0.0f, 1.0f);

    // expect at (1, 2, 0)
    Vector4f res = transformation * vec;
    EXPECT_NEAR(res.x, 1.0f, 0.0001f);
    EXPECT_NEAR(res.y, 2.0f, 0.0001f);
    EXPECT_NEAR(res.z, 0.0f, 0.0001f);
    EXPECT_NEAR(res.w, 1.0f, 0.0001f);
}
