
#include <memory>

#include <Catch/catch.hpp>

#include <ax/memory_pool.h>

struct pooled : ax::memory_pooled<pooled>
{
    char data_[12];
};

SCENARIO("memory pool reuses memory", "[memory_pool]")
{
    GIVEN("an uninitialized memory pool") {
        WHEN("an instance is allocated") {
            pooled* ptr = std::make_unique<pooled>().get();
            CHECK(std::make_unique<pooled>().get() == ptr);
        }
        WHEN("two instances are allocated") {
            std::set<pooled*> used;
            {
                for (unsigned i = 0; i != 8; ++i) {
                    auto p = std::make_unique<pooled>();
                    used.insert(p.get());
                    used.insert(std::make_unique<pooled>().get());
                }
            }
            for (unsigned i = 0; i != 8; ++i) {
                auto it = used.find(std::make_unique<pooled>().get());
                CHECK(it != used.end());
            }
        }
    }
}
