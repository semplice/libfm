#include <fm.h>

//ignore for test disabled asserts
#ifdef G_DISABLE_ASSERT
    #undef G_DISABLE_ASSERT
#endif

#define TEST_PARSING(func, str_to_parse, ...) \
    G_STMT_START { \
        char* expected[] = {__VA_ARGS__}; \
        test_parsing(func, str_to_parse, expected, G_N_ELEMENTS(expected)); \
    } G_STMT_END

typedef FmPath* (*FmPathFunc)(const char*);

static void test_parsing(FmPathFunc func, const char* str, const char** expected, int n_expected)
{
    GSList* elements = NULL, *l;
    int i;
    FmPath *path, *element;
    g_print("\ntry to parse \'%s\':\n[", str);
    path = func(str);

    for(element = path; element; element = element->parent)
        elements = g_slist_prepend(elements, element);

    for(i = 0, l = elements; l; l=l->next, ++i)
    {
        g_assert_cmpint(i, <, n_expected);
        element = (FmPath*)l->data;
        g_print("\'%s\'", element->name);
        if(l->next)
            g_print(", ");
        g_assert_cmpstr(element->name, ==, expected[i]);
    }
    g_slist_free(elements);
    g_print("]\n");

    g_assert_cmpint(i, ==, n_expected);

    fm_path_unref(path);
}

static void test_uri_parsing()
{
    FmPath* path;

    // test URIs
    TEST_PARSING(fm_path_new_for_uri, "file:///test/path/",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_uri, "file:/test/path",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_uri, "file://test/path",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_uri, "http://test/path/",
        "http://test/", "path");

    TEST_PARSING(fm_path_new_for_uri, "http://test",
        "http://test/");

    TEST_PARSING(fm_path_new_for_uri, "http:/test",
        "http://test/");

    TEST_PARSING(fm_path_new_for_uri, "http://////test",
        "http://test/");

    TEST_PARSING(fm_path_new_for_uri, "http://wiki.lxde.org/zh/%E9%A6%96%E9%A0%81",
        "http://wiki.lxde.org/", "zh", "首頁");

    TEST_PARSING(fm_path_new_for_uri, "mailto:test",
        "mailto:test");

    TEST_PARSING(fm_path_new_for_uri, "http://test/path/to/file",
        "http://test/", "path", "to", "file");

    // FIXME: is this ok?
    TEST_PARSING(fm_path_new_for_uri, "http://test/path/to/file?test_arg=xx",
        "http://test/", "path", "to", "file?test_arg=xx");

    // test user name, password, and port
    TEST_PARSING(fm_path_new_for_uri, "ftp://user@host",
        "ftp://user@host/");

    TEST_PARSING(fm_path_new_for_uri, "ftp://user:pass@host",
        "ftp://user:pass@host/");

    TEST_PARSING(fm_path_new_for_uri, "ftp://user@host:21",
        "ftp://user@host:21/");

    TEST_PARSING(fm_path_new_for_uri, "ftp://user:pass@host:21",
        "ftp://user:pass@host:21/");

    TEST_PARSING(fm_path_new_for_uri, "ftp://user:pass@host:21/path",
        "ftp://user:pass@host:21/", "path");

    TEST_PARSING(fm_path_new_for_uri, "ftp://user:pass@host:21/path/",
        "ftp://user:pass@host:21/", "path");

    TEST_PARSING(fm_path_new_for_uri, "ftp://user:pass@host:21/path//",
        "ftp://user:pass@host:21/", "path");

#if 0
    TEST_PARSING(fm_path_new_for_uri, "ftp://user:pass@host:21/../",
        "ftp://user:pass@host:21/", "path");
#endif

    // test special locations
    TEST_PARSING(fm_path_new_for_uri, "computer:",
        "computer:///");

    TEST_PARSING(fm_path_new_for_uri, "computer:/",
        "computer:///");

    TEST_PARSING(fm_path_new_for_uri, "computer:///",
        "computer:///");

    TEST_PARSING(fm_path_new_for_uri, "computer://///",
        "computer:///");

    TEST_PARSING(fm_path_new_for_uri, "computer://device",
        "computer:///", "device");

    TEST_PARSING(fm_path_new_for_uri, "computer:///device",
        "computer:///", "device");

    TEST_PARSING(fm_path_new_for_uri, "trash:",
        "trash:///");

    TEST_PARSING(fm_path_new_for_uri, "trash:/",
        "trash:///");

    TEST_PARSING(fm_path_new_for_uri, "trash://",
        "trash:///");

    TEST_PARSING(fm_path_new_for_uri, "trash:///",
        "trash:///");

    TEST_PARSING(fm_path_new_for_uri, "trash:////",
        "trash:///");

    TEST_PARSING(fm_path_new_for_uri, "trash:/file/",
        "trash:///", "file");

    TEST_PARSING(fm_path_new_for_uri, "trash://file",
        "trash:///", "file");

    TEST_PARSING(fm_path_new_for_uri, "trash:///file/",
        "trash:///", "file");

    TEST_PARSING(fm_path_new_for_uri, "trash:////file/",
        "trash:///", "file");

    TEST_PARSING(fm_path_new_for_uri, "trash:///file/../test",
        "trash:///", "test");

/*
    TEST_PARSING(fm_path_new_for_uri, "menu:");

    TEST_PARSING(fm_path_new_for_uri, "menu:/");

    TEST_PARSING(fm_path_new_for_uri, "menu://");

    TEST_PARSING(fm_path_new_for_uri, "menu:///");
*/
    TEST_PARSING(fm_path_new_for_uri, "menu://applications/",
        "menu://applications/");

    TEST_PARSING(fm_path_new_for_uri, "menu://applications/xxxx",
        "menu://applications/", "xxxx");

    TEST_PARSING(fm_path_new_for_uri, "menu://applications/xxxx/",
        "menu://applications/", "xxxx");

    TEST_PARSING(fm_path_new_for_uri, "menu://settings/xxxx/",
        "menu://settings/", "xxxx");

    TEST_PARSING(fm_path_new_for_uri, "menu://settings",
        "menu://settings/");

/*  This is not yet implemented
    TEST_PARSING(fm_path_new_for_uri, "applications://xxx",
        "menu://applications/", "xxx");
*/
    // test invalid URIs, should fallback to root.
    TEST_PARSING(fm_path_new_for_uri, "invalid_uri",
        "/");

    TEST_PARSING(fm_path_new_for_uri, "invalid_uri:",
        "/");

    TEST_PARSING(fm_path_new_for_uri, "invalid_uri:/",
        "/");

    TEST_PARSING(fm_path_new_for_uri, "invalid_uri:/invalid",
        "/");

    TEST_PARSING(fm_path_new_for_uri, "invalid_uri://///invalid",
        "/");

    TEST_PARSING(fm_path_new_for_uri, "",
        "/");

    TEST_PARSING(fm_path_new_for_uri, NULL,
        "/");

}

static void test_path_parsing()
{
    FmPath* path;

    TEST_PARSING(fm_path_new_for_path, "/test/path",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "/test/path/",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "/test/path//",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "/test//path//",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "/test///path//",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "//test/path",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "//",
        "/");

    TEST_PARSING(fm_path_new_for_path, "////",
        "/");

    TEST_PARSING(fm_path_new_for_path, "/",
        "/");

    // canonicalize
    TEST_PARSING(fm_path_new_for_path, "/test/./path",
        "/", "test", "path");

    TEST_PARSING(fm_path_new_for_path, "/test/../path",
        "/", "path");

    TEST_PARSING(fm_path_new_for_path, "/../path",
        "/", "path");

    TEST_PARSING(fm_path_new_for_path, "/./path",
        "/", "path");

    TEST_PARSING(fm_path_new_for_path, "invalid_path",
        "/");

    TEST_PARSING(fm_path_new_for_path, "",
        "/");

    TEST_PARSING(fm_path_new_for_path, NULL,
        "/");
}

static void test_path_child()
{
    FmPath* parent = fm_path_get_home();
    FmPath* path;
    g_print("\n");
    path = fm_path_new_child(parent, "child");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "child");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "child/");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "child");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "child///");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "child");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "..");
    g_printf("path->name = %s\n", path->name);
    g_assert(path == parent->parent);
    fm_path_unref(path);

    path = fm_path_new_child(parent, "../");
    g_printf("path->name = %s\n", path->name);
    g_assert(path == parent->parent);
    fm_path_unref(path);

    path = fm_path_new_child(parent, "/");
    g_assert(path == parent);
    fm_path_unref(path);

    parent = fm_path_get_root();

    path = fm_path_new_child(parent, "..");
    g_assert(path == parent);
    fm_path_unref(path);

    path = fm_path_new_child(parent, "../");
    g_assert(path == parent);
    fm_path_unref(path);

    parent = NULL;
    path = fm_path_new_child(parent, "/");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "/");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "//");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "/");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "///");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "/");
    fm_path_unref(path);

/*  FIXME: how to handle this case?
    path = fm_path_new_child(parent, "/test");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "/test/");
    fm_path_unref(path);
*/

    path = fm_path_new_child(parent, "trash:");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "trash:///");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "trash:/");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "trash:///");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "trash:////");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "trash:///");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "..");
    g_printf("path->name = %s\n", path->name);
    g_assert_cmpstr(path->name, ==, "/");
    fm_path_unref(path);

    path = fm_path_new_child(parent, "../");
    g_assert_cmpstr(path->name, ==, "/");
    fm_path_unref(path);

/*
    path = fm_path_new_child(parent, "..");
    g_assert(path == NULL);

    path = fm_path_new_child(parent, ".");
    g_assert(path == NULL);
*/
}

static void test_predefined_paths()
{
    FmPath* path;
    char* tmp;

    path = fm_path_new_for_uri("trash:///");
    g_assert(path == fm_path_get_trash());
    fm_path_unref(path);

    path = fm_path_new_for_uri("trash:///xxx");
    g_assert(path->parent == fm_path_get_trash());
    fm_path_unref(path);

    path = fm_path_new_for_uri("menu://");
    g_assert(path == fm_path_get_apps_menu());
    fm_path_unref(path);

    path = fm_path_new_for_uri("menu://applications");
    g_assert(path == fm_path_get_apps_menu());
    fm_path_unref(path);

    path = fm_path_new_for_uri("menu://applications/test/");
    g_assert(path->parent == fm_path_get_apps_menu());
    fm_path_unref(path);

/*
    path = fm_path_new_for_path(g_get_home_dir());
    g_assert(path == fm_path_get_home());
    fm_path_unref(path);

    tmp = g_build_filename(g_get_home_dir(), "xxxx", "xx", NULL);
    path = fm_path_new_for_path(tmp);
    g_debug("path->name=%s", path->parent->parent->name);
    g_assert(path->parent->parent == fm_path_get_home());
    fm_path_unref(path);
    g_free(tmp);

    path = fm_path_new_for_path(g_get_user_special_dir(G_USER_DIRECTORY_DESKTOP));
    g_assert(path == fm_path_get_desktop());
    fm_path_unref(path);

    tmp = g_build_filename(g_get_user_special_dir(G_USER_DIRECTORY_DESKTOP), "xxxx", "xx", NULL);
    path = fm_path_new_for_path(tmp);
    g_assert(path->parent->parent == fm_path_get_desktop());
    fm_path_unref(path);
    g_free(tmp);
*/
}

int main (int   argc, char *argv[])
{
    g_type_init();
    fm_init(NULL);

    g_test_init (&argc, &argv, NULL); // initialize test program
    g_test_add_func("/FmPath/new_child_len", test_path_child);
    g_test_add_func("/FmPath/path_parsing", test_path_parsing);
    g_test_add_func("/FmPath/uri_parsing", test_uri_parsing);
    g_test_add_func("/FmPath/predefined_paths", test_predefined_paths);

    return g_test_run();
}
