/*
 * Copyright 2015 Howard, Terrance <heyterrance@gmail.com>
 * Author: Howard, Terrance <heyterrance@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
