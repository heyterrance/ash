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

#include <Catch/catch.hpp>

#include <vector>
#include <functional>

#include <ash/sstorage.h>

TEST_CASE("stable chunk create", "[static_storage]")
{
    ash::stable_chunk<unsigned, 8> store;
    std::vector<std::reference_wrapper<unsigned>> refs;
    refs.emplace_back(store.create(15));
    CHECK(refs[0] == 15);
    for (std::size_t i = 1; i != store.capacity() - 1; ++i) {
        refs.emplace_back(store.create(i));
    }
    CHECK(refs[0] == 15);
    CHECK_NOTHROW(store.try_create());
    CHECK_THROWS(store.try_create());
}

TEST_CASE("stable storage create", "[static_storage]")
{
    ash::stable_storage<char, 4> store;
    auto& a = store.create('A');
    auto& b = store.create('B');
    auto& c = store.create('C');
    auto& d = store.create('D');
    CHECK(store.chunk_count() == 1);
    CHECK(&a + 1 == &b);
    CHECK(&c + 1 == &d);

    auto& e = store.create('E');
    CHECK(store.chunk_count() == 2);
    CHECK_FALSE(&d + 1 == &e);
}
