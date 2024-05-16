
#include "doctest/doctest.h"
#include "locked-map.hpp"

namespace concurrency = donkeycar::concurrency;

TEST_CASE("locked-map test: basic ctor")
{
    concurrency::LockedMap<std::string, int> q;
    concurrency::LockedMap<std::string, int*> p;
    CHECK(q.size() == 0);
    CHECK(p.size() == 0);
    CHECK(q.empty() == true);
    CHECK(p.empty() == true);

    // expect std::out_of_range
    CHECK_THROWS_AS(q.at("foo"), std::out_of_range);
}
TEST_CASE("locked-map test: with struct values")
{
    struct Foo
    {
        std::string bar{"Foo.bar"};
        int baz = 42;
    };
    concurrency::LockedMap<std::string, Foo> q;
    concurrency::LockedMap<std::string, Foo*> p;
     
    SUBCASE("locked-map: no data") {
        CHECK(q.size() == 0);
        CHECK(p.size() == 0);
        CHECK(q.empty() == true);
        CHECK(p.empty() == true);
    
        // expect std::out_of_range
        CHECK_THROWS_AS(q.at("gimme"), std::out_of_range);
    }

    SUBCASE("locked-map: empty string key") {
        // expect std::out_of_range
        CHECK_THROWS_AS(q.at(""), std::out_of_range);
    }

    SUBCASE("locked-map: data") {
        q.insert("gimme", Foo());
        Foo f = q.at("gimme");
        CHECK(f.baz == 42);
        CHECK(f.bar == "Foo.bar");

        // verify insert at the same key is an update (upsert)
        Foo f2;
        f2.baz = 43;
        f2.bar = "Giants win";

        q.insert("gimme", f2);
        Foo f3 = q.at("gimme");
        CHECK(f3.baz == 43);
        CHECK(f3.bar == "Giants win");
    }
}
TEST_CASE("locked-map test: with smart_ptr values")
{
    struct Foo
    {
        std::string bar{"Foo.bar"};
        int baz = 42;
    };
    typedef std::shared_ptr<Foo> Datum;
    std::shared_ptr<Foo> d = std::make_shared<Foo>();
    concurrency::LockedMap<std::string, Datum> m;

    CHECK_THROWS_AS(m.at(""), std::out_of_range);
    
    m.insert("gimme", d);
    auto fptr = m.at("gimme");
    CHECK(fptr->baz == 42);
    CHECK(fptr->bar == "Foo.bar");
}