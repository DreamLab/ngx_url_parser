#include "gtest/gtest.h"

extern "C" {
    #include "../ngx_url_parser.h"
}

TEST(ngx_url_parser, CorrectUrl) {
    const char * str = "http://mkaciuba.pl";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPort) {
    const char * str = "http://mkaciuba.pl:443";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "443");
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}
TEST(ngx_url_parser, UrlWithPort2) {
    const char * str = "http://mkaciuba.pl:4";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "4");
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPortAndPath) {
    const char * str = "http://mkaciuba.pl:443/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "443");
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPath) {
    const char * str = "http://mkaciuba.pl/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPath2) {
    const char * str = "http://mkaciuba.pl/X";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/X");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndQuery) {
    const char * str = "http://mkaciuba.pl/?a=2&b[]=1";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, "a=2&b[]=1");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndQuery2) {
    const char * str = "http://mkaciuba.pl/d/b/?a=2&b[]=1";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/d/b/");
    ASSERT_STREQ(url.query, "a=2&b[]=1");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndQuery3) {
    const char * str = "https://mkaciuba.pl/d/b?a=2&b[]=1";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/d/b");
    ASSERT_STREQ(url.query, "a=2&b[]=1");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndFragment) {
    const char * str = "https://mkaciuba.pl/#test";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.fragment, "test");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithQuery) {
    const char * str = "https://mkaciuba.pl?aa";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, "aa");
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithQuery2) {
    const char * str = "https://mkaciuba.pl?";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, "");
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithFragment) {
    const char * str = "https://mkaciuba.pl#aa";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.fragment, "aa");
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithRandomQuery) {
    const char * str = "https://mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithauth) {

    const char * str = "https://marcin:a@mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.auth, "marcin:a");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithauthNoPass) {

    const char * str = "https://marcin:@mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.auth, "marcin:");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithauthNoUser) {

    const char * str = "https://:pw@mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.auth, ":pw");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithAllParameters) {

    const char * str = "https://a:pw@mkaciuba.pl:1/?cbf#abc";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, "1");
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, "abc");
    ASSERT_STREQ(url.query, "cbf");
    ASSERT_STREQ(url.auth, "a:pw");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, OnlyPath) {

    const char * str = "/a/b?x=y#test";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, NULL);
    ASSERT_STREQ(url.schema, NULL);
    ASSERT_STREQ(url.path, "/a/b");
    ASSERT_STREQ(url.fragment, "test");
    ASSERT_STREQ(url.query, "x=y");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, OnlyPath2) {

    const char * str = "/a.txt";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, NULL);
    ASSERT_STREQ(url.schema, NULL);
    ASSERT_STREQ(url.path, "/a.txt");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, DuplicatedSlash) {

    const char * str = "http://10.177.51.76:1337/example//dir/hi";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "10.177.51.76");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.path, "/example//dir/hi");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, LongUrl) {

    const char * str = "http://10.177.51.76:1337/v2//v2?y_ac&yr&ffffflllllllllllloooooooooooooooooooongggggggg%d%g$d1S";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "10.177.51.76");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.query, "y_ac&yr&ffffflllllllllllloooooooooooooooooooongggggggg%d%g$d1S");
    ASSERT_STREQ(url.path, "/v2//v2");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlFromExample) {

    const char * str = "https://user:password@mkaciuba.pl:555/path/?query#fragment";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.path, "/path/");
    ASSERT_STREQ(url.fragment, "fragment");
    ASSERT_STREQ(url.query, "query");
    ASSERT_STREQ(url.auth, "user:password");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, FreeMemoryTwoTimes) {

    const char * str = "https://user:password@mkaciuba.pl:555/path/?query#fragment";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ngx_url_free(&url);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.auth, NULL);
    ASSERT_STREQ(url.host, NULL);
}

TEST(ngx_url_parser, TestUrl) {

    const char * str = "http://10.177.51.76:1337//example/dir/hi";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "10.177.51.76");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "1337");
    ASSERT_STREQ(url.path, "//example/dir/hi");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.auth, NULL);

    ngx_url_free(&url);
}
