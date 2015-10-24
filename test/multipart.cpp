/*
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2015 Howard, Terrance <heyterrance@gmail.com>
 * Author: Howard, Terrance <heyterrance@gmail.com>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <Catch/catch.hpp>

#include <ash/multipart.h>

struct no_default {
    no_default() = delete;
    no_default(int) { }
};

TEST_CASE("constructor", "[multipart]")
{
    SECTION("default") {
        ash::multipart<int, char> m;
        m.get<int>() = 3;
        m.get<char>() = 'a';
        CHECK(m.get<int>() == 3);
        CHECK(m.get<char>() == 'a');
    }

    SECTION("nullptr") {
        ash::multipart<int, no_default> m{nullptr};
        CHECK(m.get_ptr<int>() == nullptr);
        CHECK(m.get_ptr<no_default>() == nullptr);
    }
}

TEST_CASE("make", "[multipart]")
{
    ash::multipart<int, no_default> m{nullptr};
    REQUIRE(m.get_ptr<no_default>() == nullptr);
    m.make<no_default>(1);
    CHECK(m.get_ptr<no_default>());
}
