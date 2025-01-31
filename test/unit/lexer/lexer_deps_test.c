#include <stdlib.h>

#include "liblogc.h"
#include "unity.h"
#include "lexer_test.h"

char *errmsg = "";

char *data, *actual, *expected;

#define DEBUG_LEVEL debug_opamc_test
extern int  DEBUG_LEVEL;
#define TRACE_FLAG trace_opamc_test
extern bool TRACE_FLAG;

extern int  debug_opamc_lexis;
extern bool trace_opamc_lexis;

#define CHECK_RESULT(expected, actual) \
    if (actual == NULL) {                            \
        TEST_FAIL_MESSAGE("Lexical error");          \
    } else {                                         \
        TEST_ASSERT_EQUAL_STRING(expected, actual);  \
        free(actual);                                \
    }

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

/* depends: [ */
/*   "ocaml" {>= "4.04.0"} */
/*   "dune" {>= "1.11.0"} */
/*   "base-bytes" */
/*   "base-unix" */
/*   "seq" */
/*   "stdlib-shims" */
/* ] */

void simple(void) {
    data = "depends: [ \"base-bytes\" \"base-unix\" \"seq\" ]";
    expected = "DEPENDS LBRACKET "
        "String(\"base-bytes\") "
        "String(\"base-unix\") "
        "String(\"seq\") "
        "RBRACKET";
    actual = opam_lex_string(data);
    if (actual == NULL) {
        TEST_FAIL_MESSAGE("Lexical error");
    } else {
        TEST_ASSERT_EQUAL_STRING(expected, actual);
        free(actual);
    }
}

/* depends: [ */
/*   "ocaml" {>= "4.04.0"} */
/*   "dune" {>= "1.11.0"} */
  /* "owl" {= version} */
  /* "yojson" {>= "1.6.0" & < "2.0.0"} */
  /* "ppxlib" {>= "0.9.0" & < "0.14.0"} */
  /* "ounit" {with-test & >= "2.0.0"} */
  /* "sexplib0" {with-test & < "v0.15"} */
  /* "re" {with-test & >= "1.9.0"} */
  /* "stdio" {with-test} */
  /* "odoc" {with-doc} */
// "ocamlfind" {build} // like a ' dev tool' for bazel
  /*         "ocamlbuild" {build} */
  /*         "topkg" {build & >= "1.0.3"} */
  /* "menhir" {build & >= "20181113"} */

void filtered(void) {
    data = "depends: [ \"ocaml\" {>= \"4.04.0\"} ]";
    expected = "DEPENDS LBRACKET "
        "String(\"ocaml\") "
        "LBRACE RelOp(>=) String(\"4.04.0\") "
        "RBRACE "
        "RBRACKET";
    actual = opam_lex_string(data);
    if (actual == NULL) {
        TEST_FAIL_MESSAGE("Lexical error");
    } else {
        TEST_ASSERT_EQUAL_STRING(expected, actual);
        free(actual);
    }
}

void dbg(void) {
    data = "build: [ [foo] { a } ]";
    expected = "";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    initialize(argc, argv, "lexis");
    LOG_DEBUG(0, "beginning lexer test", "");

    UNITY_BEGIN();

    /* **************** */
    /* RUN_TEST(simple); */
    RUN_TEST(filtered);
    /* RUN_TEST(dbg); */

    int rc = UNITY_END();
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
    return rc;
}
