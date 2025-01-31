#include <stdlib.h>

#include "liblogc.h"
#include "unity.h"
#include "parser_build_test.h"

char *errmsg = "";

char *data, *actual, *expected;

struct opam_package_s *opkg;

#define DEBUG_LEVEL debug_opamc_test
extern int DEBUG_LEVEL;
extern int debug_opamc_lexis;
#define TRACE_FLAG trace_opamc_test
extern bool TRACE_FLAG;

extern int  debug_opamc_lexis;
extern bool trace_opamc_lexis;
extern int  debug_opamc_syntaxis;
extern bool trace_opamc_syntaxis;

void setUp(void) {
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
}

void tearDown(void) {
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
}


    /* data = "build: [[\"foo\"]]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING("build", opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = "build: [[foo:var]]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING("build", opkg->bindings->name); */
    /* opam_package_free(opkg); */


void dune_1(void) {
    expected = "build";
    data = "build: [[\"dune\" \"build\" \"-p\" name \"-j\" jobs]]";
    opkg = opam_parse_string("dune_1", data);
    TEST_ASSERT_EQUAL_STRING("dune_1", opkg->src);
    TEST_ASSERT_EQUAL_STRING("build", opkg->bindings->name);
    int ct = HASH_CNT(hh, opkg->bindings);
    TEST_ASSERT_EQUAL_INT(1, ct);
    struct opam_binding_s *b = NULL;
    char *key = "build";
    HASH_FIND_STR(opkg->bindings, key, b);
    if (b) {
        UT_array *cmds = b->val;
        int cmd_ct = utarray_len(cmds);
        TEST_ASSERT_EQUAL_INT(1, cmd_ct);
        struct opam_cmd_s *cmd;
        struct opam_arg_s *arg;
        cmd = utarray_eltptr(cmds, 0);
        TEST_ASSERT_EQUAL_INT(6, utarray_len(cmd->args));
        arg = utarray_eltptr(cmd->args, 0);
        TEST_ASSERT_EQUAL_STRING("dune", arg->val);
        arg = utarray_eltptr(cmd->args, 1);
        TEST_ASSERT_EQUAL_STRING("build", arg->val);
        arg = utarray_eltptr(cmd->args, 2);
        TEST_ASSERT_EQUAL_STRING("-p", arg->val);
        TEST_ASSERT_EQUAL_INT(STRING_ARG, arg->t);
        arg = utarray_eltptr(cmd->args, 3);
        TEST_ASSERT_EQUAL_STRING("name", arg->val);
        TEST_ASSERT_EQUAL_INT(VAR_ARG, arg->t);
        arg = utarray_eltptr(cmd->args, 4);
        TEST_ASSERT_EQUAL_STRING("-j", arg->val);
        arg = utarray_eltptr(cmd->args, 5);
        TEST_ASSERT_EQUAL_STRING("jobs", arg->val);

    } else {
        UT_string *tmp; utstring_new(tmp);
        utstring_printf(tmp, "Binding %s not found.", key);
        TEST_FAIL_MESSAGE(utstring_body(tmp));
        utstring_free(tmp);
    }
    opam_package_free(opkg);
}

void cmds_2(void) {
    /* data = "build: [[foo] { bar }]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = "build: [[foo] { 32 }]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = " build: [[foo] {bar}]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = " build: [[foo] {bar}]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = " build: [[foo {bar}] {baz}]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = "build: [[\"foo\"  \"-bar\" { baz }]]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    /* data = " build: [[foo] { \"3.2\" & build }]"; */
    /* opkg = opam_parse_string("build_fld", data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, opkg->bindings->name); */
    /* opam_package_free(opkg); */

    //data = " build: { >=\"3.12\" & build }";
    /* data = "build: [ [\"dune\" subst] ]"; */
    /* data = "build: [ [\"foo\" bar] {pinned | \"unpinned\" } ]"; */
}

void cmds_3(void) {
    data = "build: [[\"prep\"] [\"build\"] [\"install\"]]";
    LOG_DEBUG(1, "data: \"%s\"", data);
    opkg = opam_parse_string("case002", data);
    TEST_ASSERT_EQUAL_STRING("case002", opkg->src);
    int ct = HASH_CNT(hh, opkg->bindings);
    TEST_ASSERT_EQUAL_INT(1, ct);
    struct opam_binding_s *b = NULL;
    char *key = "build";
    HASH_FIND_STR(opkg->bindings, key, b);
    if (b) {
        UT_array *cmds = b->val;
        int cmd_ct = utarray_len(cmds);
        TEST_ASSERT_EQUAL_INT(3, cmd_ct);
        struct opam_cmd_s *cmd;
        struct opam_arg_s *arg;
        cmd = utarray_eltptr(cmds, 0);
        arg = utarray_eltptr(cmd->args, 0);
        TEST_ASSERT_EQUAL_STRING("prep", arg->val);

        cmd = utarray_eltptr(cmds, 1);
        arg = utarray_eltptr(cmd->args, 0);
        TEST_ASSERT_EQUAL_STRING("build", arg->val);

        cmd = utarray_eltptr(cmds, 2);
        arg = utarray_eltptr(cmd->args, 0);
        TEST_ASSERT_EQUAL_STRING("install", arg->val);
    } else {
        UT_string *tmp; utstring_new(tmp);
        utstring_printf(tmp, "Binding %s not found.", key);
        TEST_FAIL_MESSAGE(utstring_body(tmp));
        utstring_free(tmp);
    }
    opam_package_free(opkg);
}

void cmd_filter(void) {
    data = "build: [ [\"dune\" \"subst\"] { a } ]";
    LOG_DEBUG(1, "data: \"%s\"", data);
    opkg = opam_parse_string("case003", data);
    TEST_ASSERT_EQUAL_STRING("case003", opkg->src);
    int ct = HASH_CNT(hh, opkg->bindings);
    TEST_ASSERT_EQUAL_INT(1, ct);
    struct opam_binding_s *b = NULL;
    char *key = "build";
    HASH_FIND_STR(opkg->bindings, key, b);
    if (b) {
        UT_array *cmds = b->val;
        int cmd_ct = utarray_len(cmds);
        TEST_ASSERT_EQUAL_INT(1, cmd_ct);
        struct opam_cmd_s *cmd;
        struct opam_arg_s *arg;
        cmd = utarray_eltptr(cmds, 0);
        TEST_ASSERT_EQUAL_INT(2, utarray_len(cmd->args));
        arg = utarray_eltptr(cmd->args, 0);
        TEST_ASSERT_EQUAL_STRING("dune", arg->val);
        arg = utarray_eltptr(cmd->args, 1);
        TEST_ASSERT_EQUAL_STRING("subst", arg->val);

        /* cmd = utarray_eltptr(cmds, 2); */
        /* arg = utarray_eltptr(cmd->args, 0); */
        /* TEST_ASSERT_EQUAL_STRING("install", arg->val); */
    } else {
        UT_string *tmp; utstring_new(tmp);
        utstring_printf(tmp, "Binding %s not found.", key);
        TEST_FAIL_MESSAGE(utstring_body(tmp));
        utstring_free(tmp);
    }
    opam_package_free(opkg);
}

void dbg(void) {
    expected = "build";
    /* data = " \"foo\" { \"bar\" }"; */
    /*ok data = " \"foo\" { build }"; */
    /*ok? data = " \"foo\" { true }"; */

    /* data = "build: [ [foo] { ( a | B ) & (!c) } ]"; */

    /* data = "build: [[foo bar]]"; */
    //data = "build: [[foo] { \"bar\" }]";

    data = "build: [ [foo] { a | b } ]";

    data = "build: [[\"foo\" \"bar\"]]";
    data = "version: \"1.2.0\"\nbuild: [[\"bar\" \"baz\"]]";
    LOG_DEBUG(1, "data: \"%s\"", data);
    opkg = opam_parse_string("test_1", data);
    int ct = HASH_CNT(hh, opkg->bindings);
    TEST_ASSERT_EQUAL_INT(2, ct);
    struct opam_binding_s *b;
    char *key = "version";
    HASH_FIND_STR(opkg->bindings, key, b);
    if (b) {
        LOG_DEBUG(0, "found %s", key);
        TEST_ASSERT_EQUAL_STRING("1.2.0", b->val);

    } else {
        LOG_DEBUG(0, "not found: %s", key);
    }
    TEST_ASSERT_EQUAL_STRING("test_1", opkg->src);
    opam_package_free(opkg);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    initialize(argc, argv, "parser");
    LOG_DEBUG(0, "beginning parser test", "");

    UNITY_BEGIN();

    RUN_TEST(dune_1);
    /* RUN_TEST(cmds_3); */
    /* RUN_TEST(cmd_filter); */
    /* RUN_TEST(dbg); */

    int rc = UNITY_END();
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
    return rc;
}
