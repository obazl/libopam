#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <libgen.h>
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>             /* PATH_MAX */
#endif
#include <pwd.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

/* #include "ini.h" */
#include "log.h"
/* #if EXPORT_INTERFACE */
#include "utarray.h"
#include "utstring.h"
/* #endif */

#include "opam_config.h"

/* const char *errmsg = NULL; */

extern bool verbose;
extern int  verbosity;

extern int rc;

/* char *bazel_script_dir = NULL; */

extern bool enable_jsoo;

/* /\* UT_string *opam_switch_id       = NULL; *\/ */

/* /\* UT_string *opam_switch_prefix   = NULL; *\/ */
/* UT_string *opam_coswitch_prefix = NULL; */

/* /\* UT_string *opam_switch_bin      = NULL; *\/ */
/* UT_string *opam_coswitch_bin    = NULL; */

/* /\* UT_string *opam_switch_lib      = NULL; *\/ */
/* UT_string *opam_coswitch_lib    = NULL; */

/* /\* UT_string *opam_ocaml_version   = NULL; *\/ */

/* seets global opam_switch_* vars */
/* EXPORT void opam_configure(char *_opam_switch) */
/* { */
/* #if defined(TRACING) */
/*     if (mibl_trace) */
/*         log_trace("opam_configure: '%s'", _opam_switch); */
/*     log_trace("cwd: %s\n", getcwd(NULL, 0)); */
/* #endif */

/*     /\* */
/*       if _opam_switch emtpy, discover current switch: */
/*           - check for local switch ('_opam' subdir of root dir) */
/*          - check env var OPAMSWITCH */
/*          - else run 'opam var switch' */
/*       2. discover lib dir: 'opam var lib' */
/*      *\/ */

/*     /\* utstring_new(opam_switch_id); *\/ */
/*     /\* /\\* utstring_new(opam_switch_prefix); *\\/ *\/ */
/*     /\* utstring_new(opam_coswitch_prefix); *\/ */
/*     /\* utstring_new(opam_switch_bin); *\/ */
/*     /\* utstring_new(opam_coswitch_bin); *\/ */
/*     /\* utstring_new(opam_switch_lib); *\/ */
/*     /\* utstring_new(opam_coswitch_lib); *\/ */
/*     /\* utstring_new(opam_ocaml_version); *\/ */

/*     /\* FIXME: handle switch arg *\/ */
/*     /\* FIXME: argv *\/ */
/*     char *exe = NULL, *result = NULL; */
/*     if (strlen(_opam_switch) == 0) { */

/*         exe = "opam"; */
/*         char *argv[] = {"opam", "var", "switch", NULL}; */

/*         result = run_cmd(exe, argv); */
/*         if (result == NULL) { */
/*             fprintf(stderr, "FAIL: run_cmd 'opam var switch'\n"); */
/*         } else { */
/*             utstring_printf(opam_switch_id, "%s", result); */
/*             if (verbose && verbosity > 1) */
/*                 log_info(" Current OPAM switch: %s", result); */

/* #if defined(TRACING) */
/*             log_debug("cmd result: '%s'", utstring_body(opam_switch_id)); */
/* #endif */
/*         } */
/*     } // else?? */
/*     /\* cmd = "opam var prefix"; *\/ */
/*     char *argv1[] = {"opam", "var", "prefix", NULL}; */
/*     result = NULL; */
/*     result = run_cmd(exe, argv1); */
/*     if (result == NULL) { */
/*         log_fatal("FAIL: run_cmd 'opam var prefix'\n"); */
/*         exit(EXIT_FAILURE); */
/*     } else { */
/*         if (verbose && verbosity > 1) */
/*             log_info("Current OPAM switch prefix: %s", result); */
/*         /\* utstring_printf(opam_switch_prefix, "%s", result); *\/ */
/*         /\* default: coswitch == switch *\/ */
/*         utstring_printf(opam_coswitch_prefix, "%s", result); */
/* #if defined(TRACING) */
/*         log_debug("cmd result: '%s'", utstring_body(opam_switch_bin)); */
/* #endif */
/*     } */

/*     /\* cmd = "opam var bin"; *\/ */
/*     char *argv2[] = {"opam", "var", "bin", NULL}; */
/*     result = NULL; */
/*     result = run_cmd(exe, argv2); */
/*     if (result == NULL) { */
/*         log_fatal("FAIL: run_cmd 'opam var bin'\n"); */
/*         exit(EXIT_FAILURE); */
/*     } else { */
/*         utstring_printf(opam_switch_bin, "%s", result); */
/*         /\* default: coswitch == switch *\/ */
/*         utstring_printf(opam_coswitch_bin, "%s", result); */
/* #if defined(TRACING) */
/*         log_debug("cmd result: '%s'", utstring_body(opam_switch_bin)); */
/* #endif */
/*     } */

/*     /\* cmd = "opam var lib"; *\/ */
/*     char *argv3[] = {"opam", "var", "lib", NULL}; */
/*     result = NULL; */
/*     result = run_cmd(exe, argv3); */
/*     if (result == NULL) { */
/*         log_fatal("FAIL: run_cmd 'opam var lib'\n"); */
/*         exit(EXIT_FAILURE); */
/*     } else { */
/*         utstring_printf(opam_switch_lib, "%s", result); */
/*         /\* default: coswitch == switch *\/ */
/*         utstring_printf(opam_coswitch_lib, "%s", result); */
/* #if defined(TRACING) */
/*         log_debug("cmd result: '%s'", utstring_body(opam_switch_lib)); */
/* #endif */
/*     } */

/*     char *argv4[] = {"opam", "var", "ocaml:version", NULL}; */
/*     result = NULL; */
/*     result = run_cmd(exe, argv4); */
/*     if (result == NULL) { */
/*         log_fatal("FAIL: run_cmd 'opam var prefix'\n"); */
/*         exit(EXIT_FAILURE); */
/*     } else { */
/*         if (verbose && verbosity > 1) */
/*             log_info("Current OPAM ocaml version: %s", result); */
/*         utstring_printf(opam_ocaml_version, "%s", result); */
/* #if defined(TRACING) */
/*         log_debug("cmd result: '%s'", utstring_body(opam_ocaml_version)); */
/* #endif */
/*     } */

/*     return; */
/* } */

/* ******************************** */
EXPORT char *opam_root(void)
{
    TRACE_ENTRY
    char *result = NULL;
    char *exe = "opam";
    char *argv[] = {"opam", "var", "root", NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        fprintf(stderr, "FAIL: run_cmd 'opam var root'\n");
    }
    return result;
}

/* ******************************** */
EXPORT char *opam_switch_name(void)
{
    /* log_debug("OOOOOOOOOOOOOOOO"); */
    /* log_debug("BUILD_WORKSPACE_DIRECTORY: %s", */
    /*           getenv("BUILD_WORKSPACE_DIRECTORY")); */
    /* log_debug("OPAM_SWITCH_PREFIX: %s", */
    /*           getenv("OPAM_SWITCH_PREFIX")); */
    /* log_debug("CWD: %s", getcwd(NULL,0)); */
    /* log_debug("PATH: %s", */
    /*           getenv("PATH")); */
    TRACE_ENTRY
    char *result = NULL;

    /* char *exe = "env"; */
    /* char *argv0[] = {"env", NULL}; */
    /* result = run_cmd(exe, argv0, envp); */
    /* if (result == NULL) { */
    /*     log_error("FAIL: run_cmd 'opam var switch'"); */
    /* } else { */
    /*     log_error("which opam: %s", result); */
    /* } */
    /* log_debug("PPPPPPPPPPPPPPPP"); */

    char *exe = "opam";
    char *argv[] = {"opam", "var", "switch", NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        log_error("FAIL: run_cmd 'opam var switch'");
        fprintf(stderr, "FAIL: run_cmd 'opam var switch'\n");
    /* } else { */
    /*     log_debug("opam var switch: %s", result); */
    }
    return result;
}

EXPORT char *opam_switch_prefix(char *opam_switch)
{
    TRACE_ENTRY;
    /* log_debug("opam_switch: %s", opam_switch); */
    char *result = NULL;
    char *exe = "opam";
    char *argv[] = {"opam", "var", "prefix", "--switch", opam_switch, NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        fprintf(stderr, "FAIL: run_cmd 'opam var prefix --switch'\n");
    }
    return result;
}

EXPORT char *opam_switch_lib(char *opam_switch)
{
    TRACE_ENTRY;
    /* log_debug("opam_switch: %s", opam_switch); */
    char *result = NULL;
    char *exe = "opam";
    char *argv[] = {"opam", "var", "lib", "--switch", opam_switch, NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        fprintf(stderr, "FAIL: run_cmd 'opam var lib --switch'\n");
    }
    return result;
}

EXPORT char *opam_switch_stublibs(char *opam_switch)
{
    TRACE_ENTRY;
    /* log_debug("opam_switch: %s", opam_switch); */
    char *result = NULL;
    char *exe = "opam";
    char *argv[] = {"opam", "var", "stublibs", "--switch", opam_switch, NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        fprintf(stderr, "FAIL: run_cmd 'opam var stublibs --switch'\n");
    }
    return result;
}

EXPORT char *opam_switch_ocaml_version(char *opam_switch)
{
    TRACE_ENTRY
    char *result = NULL;
    char *exe = "opam";
    char *argv[] = {"opam", "var", "ocaml:version", "--switch", opam_switch, NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        fprintf(stderr, "FAIL: run_cmd 'opam var ocaml:version'\n");
    }
    return result;
}

EXPORT char *opam_switch_base_compiler_version(char *opam_switch)
{
    TRACE_ENTRY
    char *result = NULL;
    char *exe = "opam";
    char *argv[] = {"opam", "var", "ocaml-base-compiler:version", "--switch", opam_switch, NULL};
    result = run_cmd(exe, argv);
    if (result == NULL) {
        fprintf(stderr, "FAIL: run_cmd 'opam var ocaml-base-compiler:version'\n");
    }
    return result;
}

