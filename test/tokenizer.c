#include <gtest/gtest.h>

#include <tokenizer.h>

TEST(TestCase, _consume) {
    token_t *dummy = (token_t *)calloc(1, sizeof(token_t));
    
    _consume("s1")
}