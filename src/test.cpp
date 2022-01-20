#include "lrucache.hpp"
#include "gtest/gtest.h"

#include <algorithm>
#include <numeric>
#include <random>

const int NUM_OF_TEST2_RECORDS = 1'000'000;
const int TEST2_CACHE_CAPACITY = 500'000;

TEST(CacheTest, SimplePut) {
    cache::lru_cache<int, int> cache_lru(1);
    cache_lru.put(7, 777);
    EXPECT_TRUE(cache_lru.exists(7));
    EXPECT_TRUE(777 == cache_lru.get(7));
    EXPECT_EQ(1, cache_lru.size());
}

TEST(CacheTest, MissingValue) {
    cache::lru_cache<int, int> cache_lru(1);
    EXPECT_THROW(cache_lru.get(7), std::range_error);
}

TEST(CacheTest1, KeepsAllValuesWithinCapacity) {
    cache::lru_cache<int, int> cache_lru(TEST2_CACHE_CAPACITY);

    for (int i = 0; i < NUM_OF_TEST2_RECORDS; ++i) {
        cache_lru.put(i, i);
    }

    for (int i = 0; i < NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; ++i) {        
        EXPECT_FALSE(cache_lru.exists(i));
        decltype(cache_lru)::list_iterator_t iter{};
        EXPECT_FALSE(cache_lru.exists(i,iter));
    }

    for (int i = NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; i < NUM_OF_TEST2_RECORDS; ++i) {
        EXPECT_TRUE(cache_lru.exists(i));        
        EXPECT_TRUE(i == cache_lru.get(i));
    }

    EXPECT_TRUE(TEST2_CACHE_CAPACITY == cache_lru.size());
}

TEST(CacheTest1, EraseSingleValue) {
    auto records = std::array<int, NUM_OF_TEST2_RECORDS>();
    std::iota(records.begin(), records.end(), 0);

    cache::lru_cache<int,int> test{NUM_OF_TEST2_RECORDS};

    for (auto &i : records) {
      test.put(i, i);
    }

    std::random_device random_device;     // only used once to initialise (seed) engine
    std::mt19937 rn_generator(random_device());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0,NUM_OF_TEST2_RECORDS); // guaranteed unbiased

    auto random_integer_for_del = uni(rn_generator);
    test.erase(random_integer_for_del);

    int current_integer = 0;

    for (auto &n : records) {
      if (auto [ok, cache_it] = test.get_if_exists(n); ok){
        EXPECT_TRUE(records[n] == cache_it->second);
      }else{
        EXPECT_TRUE(random_integer_for_del == current_integer);
      }
      current_integer++;
    }
}

TEST(CacheTest1, EraseMultipleValues) {
    auto records = std::array<int, NUM_OF_TEST2_RECORDS>();
    std::iota(records.begin(), records.end(), 0);

    cache::lru_cache<int,int> test{NUM_OF_TEST2_RECORDS};

    for (auto &i : records) {
      test.put(i, i);
    }

    std::vector<int> integers_for_del;
    std::random_device random_device;     // only used once to initialise (seed) engine
    std::mt19937 rn_generator(random_device());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(1,15); // guaranteed unbiased

    auto random_count = uni(rn_generator);

    for(auto n = 0; n<random_count; n++)
        integers_for_del.push_back(uni(rn_generator));

    std::sort(std::begin(integers_for_del),std::end(integers_for_del));
    integers_for_del.erase(std::unique(std::begin(integers_for_del),std::end(integers_for_del)),std::end(integers_for_del));

    for (auto &n : integers_for_del) {
        try {
            test.erase(n);
        }  catch (const std::exception& ex) {
            EXPECT_FALSE(true)<< ex.what();
        }
    }

    for (auto &n : integers_for_del) {
        try {
            test.erase(n);
            EXPECT_FALSE(true) << "Element " + std::to_string(n) + " was not deleted!";
        }  catch (const std::exception& ex) {
            EXPECT_TRUE(true);
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
