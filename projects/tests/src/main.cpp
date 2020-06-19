#include "linked_list_test.cpp"
#include "matrix_test.cpp"
#include "pool_test.cpp"
#include "slot_map_test.cpp"
#include "vector_test.cpp"

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
