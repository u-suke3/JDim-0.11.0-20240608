// SPDX-License-Identifier: GPL-2.0-or-later

#include "jdlib/cookiemanager.h"

#include "gtest/gtest.h"


namespace {

class CookieManager_TestBase : public ::testing::Test
{
public:
    JDLIB::CookieManager* the_cookie_manager{};

    void SetUp() override
    {
        the_cookie_manager = JDLIB::get_cookie_manager();
    }

    void TearDown() override
    {
        JDLIB::delete_cookie_manager();
        the_cookie_manager = nullptr;
    }
};


class CookieManager_GetCookieByHost : public CookieManager_TestBase {};

TEST_F(CookieManager_GetCookieByHost, no_data)
{
    const std::string expect = "";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.com" ) );
}

TEST_F(CookieManager_GetCookieByHost, single_value)
{
    the_cookie_manager->feed( "example.com", "foo=bar" );
    const std::string expect = "foo=bar";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.com" ) );
}

TEST_F(CookieManager_GetCookieByHost, multiple_values)
{
    the_cookie_manager->feed( "example.com", "foo=bar" );
    the_cookie_manager->feed( "example.com", "baz=qux" );
    // 現在の実装ではクッキー名を辞書順でソートする
    const std::string expect = "baz=qux; foo=bar";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.com" ) );
}

TEST_F(CookieManager_GetCookieByHost, empty_values)
{
    the_cookie_manager->feed( "example.com", "foo=" );
    the_cookie_manager->feed( "example.com", "bar=" );
    const std::string expect = "bar=; foo=";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.com" ) );
}

TEST_F(CookieManager_GetCookieByHost, parsing_domain)
{
    the_cookie_manager->feed( "first.hello.world.test", "foo=bar; domain=.world.test" );
    the_cookie_manager->feed( "second.hello.world.test", "baz=qux" );
    const std::string expect = "baz=qux; foo=bar";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "second.hello.world.test" ) );
}

TEST_F(CookieManager_GetCookieByHost, parsing_path)
{
    // ルート(/)以外のpathは無視する
    the_cookie_manager->feed( "example.com", "foo=bar; path=/" );
    the_cookie_manager->feed( "example.com", "baz=qux; path=/user" );
    const std::string expect = "foo=bar";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.com" ) );
}

TEST_F(CookieManager_GetCookieByHost, parsing_expires)
{
    std::string expect;

    the_cookie_manager->feed( "example.test", "foo=bar" );
    expect = "foo=bar";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.test" ) );

    // expires属性が設定されていないcookieは値を更新する
    the_cookie_manager->feed( "example.test", "foo=qux;" );
    expect = "foo=qux";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.test" ) );

    // cookieの有効期限が現在時刻より古いときは値を消去する
    the_cookie_manager->feed( "example.test", "foo=bar; expires=Mon, 01 Apr 2024 09:00:00 GMT" );
    expect = "";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "example.test" ) );
}


class CookieManager_DeleteCookieByHost : public CookieManager_TestBase {};

TEST_F(CookieManager_DeleteCookieByHost, delete_toplevel)
{
    the_cookie_manager->feed( "hello.world.test", "foo=bar; domain=.world.test" );
    the_cookie_manager->feed( "hello.world.test", "baz=qux;" );
    // トップレベルのドメインまで削除される
    the_cookie_manager->delete_cookie_by_host( "hello.world.test" );

    const std::string expect = "";
    EXPECT_EQ( expect, the_cookie_manager->get_cookie_by_host( "hello.world.test" ) );
}

} // namespace
