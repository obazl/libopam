/* fvf = filtered version formula
   fpf = filtered package formula */

%include {
#include <assert.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utarray.h"
#include "uthash.h"

#include "liblogc.h"

#if INTERFACE
#ifndef YYMALLOCARGTYPE
#define YYMALLOCARGTYPE size_t
#endif
#endif

#define DEBUG_LEVEL debug_opamc_syntaxis
int  DEBUG_LEVEL;
#define TRACE_FLAG trace_opamc_syntaxis
bool TRACE_FLAG;

}

%include {
struct opam_arg_s *p;
#define DUMP_ARGS(args)                          \
    LOG_DEBUG(0, "DUMPING args", "");           \
    LOG_DEBUG(0, "args ct: %d",                 \
                  utarray_len(args));              \
    for(p=(struct opam_arg_s*)utarray_front(args); \
        p!=NULL;                                  \
        p=(struct opam_arg_s*)utarray_next(args,p)) {    \
       LOG_DEBUG(0, "\tval: %s, type: %d", p->val, p->t); \
     }
    /* while ( (p=(char**)utarray_next(tok.args,p))) { \ */
    /*     LOG_DEBUG(0, "\t%s",*p);             \ */
    /* } */
}

/* %ifdef PROFILE_dev */
/* %endif */

/* opam_parse_state->pkg->bindings is a uthash of bindings. parse rules
update it directly */
/* a binding is (name, val); val may be another hash (e.g. for depends: key)  */
/* FIXME: we need a custom struct for deps: name, version, optional version_relop, optional constraint (e.g. 'with-test') */
%extra_argument { struct opam_parse_state_s *opam_parse_state }
/* %extra_argument { struct opam_package_s *opam_pkg } */

/* we piggyback on lemon to declare some type consts */
/* %token BINDING_BOOL. */
/* %token BINDING_INT. */
/* %token BINDING_STRING. */
/* %token BINDING_STRINGLIST. */

%token AMP.
%token AUTHORS.
%token AVAILABLE.
%token BANG.
%token BUG_REPORTS.
%token Bool.
%token BUILD.
%token BUILD_DOC.
%token BUILD_ENV.
%token CmdListFld.
%token COLON.
%token COMMA.
%token CONFLICTS.
%token CONFLICT_CLASS.
%token DEPENDS.
%token DEPEXTS.
%token DEPOPTS.
%token DESCRIPTION.
%token DEV_REPO.
%token DOC.
%token DQ.
%token EQ.
%token ERROR.
%token EXTRA_FILES.
%token EXTRA_SOURCE.
%token FALSE.
%token FEATURES.
/* %token FILTER. */
%token FLAG.
%token FLAGS.
%token FldName.
%token FVF_LOGOP.
%token HOMEPAGE.
%token Ident.
%token IDENTCHAR.
%token INSTALL.
%token Int.
%token LBRACE.
%token LBRACKET.
%token LICENSE.
%token LogOp.
%token LPAREN.
%token MAINTAINER.
%token MESSAGES.
%token OPAM_VERSION.
%token PACKAGE.
%token PATCHES.
%token PIN_DEPENDS.
%token PkgName.
%token POST_MESSAGES.
%token QMARK.
%token RBRACE.
%token RBRACKET.
%token REMOVE.
%token RelOp.
%token RPAREN.
%token RUNTEST.
%token SETENV.
%token SQ.
%token String.
%token String3.
%token SUBSTS.
%token SYNOPSIS.
%token TAGS.
%token TERM.
%token TERM_STRING.
%token TERM_VARIDENT.
%token TQ.
%token TRUE.
%token URL.
%token Varident.
%token VERSION.

%right LogOp.
%right BANG.
%right QMARK.

%right RelOp.


/* **************** */
%token_type { union opam_token_u } /* terminals */

/* %default_type { struct opam_entry_s * } /\* non-terminal default *\/ */

%type package { struct opam_package_s * }
%destructor package {
    log_trace("freeing package");
}
%type binding { struct opam_binding_s * }
%type fpf { struct opam_deps_s * }
%type stringlist { UT_array * }

%syntax_error {
    LOG_ERROR(0, RED "SYNTAX ERROR" CRESET " yymajor: %d", yymajor);
    LOG_ERROR(0, RED "tos.stateno" CRESET ": %d", yypParser->yytos->stateno);
    LOG_ERROR(0, RED "tos.major" CRESET ": %d", yypParser->yytos->major);
    LOG_ERROR(0, RED "tos.minor" CRESET ": %d", yypParser->yytos->minor);
/* args passed:
   yyParser *yypParser, /\* The parser *\/
   int yymajor, /\* The major type of the error token *\/
   ParseTOKENTYPE yyminor /\* The minor type of the error token *\/
   ParseTOKENTYPE is union opam_token_u
 */

%ifdef YYDEBUG_EXIT_ON_ERROR
   exit(EXIT_FAILURE);
%else
     log_error("trying to recover...");
%endif
}

%parse_accept {
    LOG_TRACE(0, "Parsing complete", "");
}

%parse_failure {
    LOG_ERROR(0, "Giving up.  Parser is hopelessly lost... %d",
    yypParser->yyerrcnt);
}

/* *******************  PARSE RULES  ********************* */
%start_symbol package

package ::= bindings . {
    LOG_DEBUG(0, "Pkg src: %s", opam_parse_state->pkg->src);
    LOG_DEBUG(0, " bindings ct: %d",
              HASH_CNT(hh, opam_parse_state->pkg->bindings));
    struct opam_binding_s *b;
    for (b = opam_parse_state->pkg->bindings;
         b != NULL;
         b = b->hh.next) {
        LOG_DEBUG(0, "\t%s: %s", b->name, b->val);
    }
}

bindings ::= binding . {
    LOG_DEBUG(0, "One binding", "");
}

/* <field-binding> ::= <ident> : <value> */
/* <value> ::= <bool> | <int> | <string> | <ident> | <varident> | <operator> | <list> | <option> | "(" <value> ")" */

bindings(Bindings_lhs) ::= bindings(Bindings_rhs) binding . {
    LOG_DEBUG(0, "BINDINGS ct: %d",
           HASH_CNT(hh, opam_parse_state->pkg->bindings));
    /* no need to do anything, opam_parse_state->pkg->bindings already
       contains all bindings */
    Bindings_lhs = Bindings_rhs;
}

binding(Binding) ::= FldName(Fldname) String(String) . {
    LOG_DEBUG(0, "BINDING: %s: %s", Fldname.s, String.s);
    /* create a binding and add it to the pkg hashmap */
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = Fldname.s;
    Binding->t = BINDING_STRING;
    Binding->val = strdup(String.s);
    HASH_ADD_KEYPTR(hh, // opam_parse_state->pkg->bindings->hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
}

binding(Binding) ::= FldName(Fldname) COLON string3(String) . {
    LOG_DEBUG(0, "BINDING: %s: %s", Fldname.s, String.s);
    /* create a binding and add it to the pkg hashmap */
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = Fldname.s;
    Binding->t = BINDING_STRING;
    Binding->val = strdup(String.s);
    HASH_ADD_KEYPTR(hh, // opam_parse_state->pkg->bindings->hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
}

binding(Binding) ::= FldName(Fldname) COLON LBRACKET stringlist(Stringlist) RBRACKET . {
    LOG_DEBUG(0, "BINDING stringlist %s, ct: %d",
           Fldname.s, utarray_len(Stringlist));
    /* create a binding and add it to the pkg hashmap */
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = Fldname.s;
    Binding->t = BINDING_STRINGLIST;
    Binding->val = (void*)Stringlist;
    HASH_ADD_KEYPTR(hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
#if YYDEBUG
    LOG_DEBUG(0, "BINDING stringlist val ct: %d",
           utarray_len((UT_array*)Binding->val));
#endif
}

/****************************************************************/
/* build, install, run-test, remove all have the same grammar:
    [ [ <term> { <filter> } ... ] { <filter> } ... ]
 */
binding(Binding) ::=
    CmdListFld LBRACKET cmdlist(Cmds) RBRACKET . {
    /* BUILD LBRACKET cmdlist(Cmds) RBRACKET . { */
    LOG_DEBUG(0, "build [cmdlist]", "");
    // for each cmd in cmdlist, print the args
    int ct = utarray_len(Cmds.cmds);
    LOG_DEBUG(0, "cmd ct: %d", ct);
    struct opam_cmd_s *cmd = NULL;
    for(cmd=(struct opam_cmd_s*)utarray_front(Cmds.cmds);
        cmd!=NULL;
        cmd=(struct opam_cmd_s*)utarray_next(Cmds.cmds, cmd)) {
        ct = utarray_len(cmd->args);
        LOG_DEBUG(0, "cmd args ct: %d", ct);
        struct opam_arg_s *arg = NULL;
        for(arg=(struct opam_arg_s*)utarray_front(cmd->args);
            arg!=NULL;
            arg=(struct opam_arg_s*)utarray_next(cmd->args, arg)) {
            LOG_DEBUG(0, "\t%s", arg->val);
        }
    }
    /* opam_parse_state->pkg->bindings, */
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = strndup("build", 5);
    Binding->t = BINDING_BUILD;
    Binding->val = Cmds.cmds;
    HASH_ADD_KEYPTR(hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
}

binding(Binding) ::=
    INSTALL LBRACKET cmdlist RBRACKET . {
    LOG_DEBUG(0, "BINDING install", "");
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = strndup("install", 7);
    Binding->t = BINDING_BUILD;
    /* Binding->val =  */
    HASH_ADD_KEYPTR(hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
}

binding(Binding) ::=
    RUNTEST LBRACKET cmdlist RBRACKET . {
    LOG_DEBUG(0, "BINDING run-test", "");
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = strndup("run-test", 8);
    Binding->t = BINDING_BUILD;
    /* Binding->val =  */
    HASH_ADD_KEYPTR(hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
}

cmdlist(CMDS) ::= . {
    LOG_DEBUG(0, "cmdlist (empty)", "");
    UT_array *cmds;
    utarray_new(cmds, &opam_cmd_icd);
    CMDS.cmds = cmds;
}

cmdlist(CMDS) ::= cmdlist(Cmds) cmd(Cmd) . {
    LOG_DEBUG(0, "cmdlist cmd", "");
    DUMP_ARGS(Cmd.cmd->args);
    utarray_push_back(Cmds.cmds, Cmd.cmd);
    CMDS.cmds = Cmds.cmds;
}

cmd(CMD) ::= LBRACKET args(Args) RBRACKET . {
    LOG_DEBUG(0, "cmd:: [args]", "");
    struct opam_cmd_s *cmd = (struct opam_cmd_s*)malloc(
                       sizeof(struct opam_cmd_s));
    cmd->args = Args.args;
    CMD.cmd = cmd;
    DUMP_ARGS(CMD.cmd->args);
}

cmd(CMD) ::= LBRACKET args(Args) RBRACKET filter(F) . {
    LOG_DEBUG(0, "cmd:: [args] filter", "");
    struct opam_cmd_s *cmd = (struct opam_cmd_s*)malloc(
                       sizeof(struct opam_cmd_s));
    cmd->args = Args.args;
    cmd->filter = F.filter;
    CMD.cmd = cmd;
}

args(ARGS) ::= . {
    LOG_DEBUG(0, "args (empty)", "");
    UT_array *args;
    utarray_new(args, &opam_arg_icd);
    /* struct opam_arg_s *arg = (struct opam_arg_s*)malloc( */
    /*                    sizeof(struct opam_arg_s)); */
    /* arg->val = strdup("testarg"); */
    /* arg->t   = 0; */
    /* utarray_push_back(args, arg); */
    DUMP_ARGS(args);
    ARGS.args = args;
}

args(NEWARGS) ::= args(xargs) arg(a) . {
    LOG_DEBUG(0, "args arg val %s, type %d", a.arg->val, a.arg->t);
    DUMP_ARGS(xargs.args);
    utarray_push_back(xargs.args, a.arg);
    DUMP_ARGS(xargs.args);
    NEWARGS = xargs;
}

arg(ARG) ::= String(str) . {
    LOG_DEBUG(0, "string arg: %s", str.s);
    struct opam_arg_s *arg = (struct opam_arg_s*)malloc(
                       sizeof(struct opam_arg_s));
    arg->val = strdup(str.s);
    arg->t   = 0;
    ARG.arg = arg;
}

arg(ARG) ::= Ident(ident) . {
    LOG_DEBUG(0, "var arg: %s", ident.s);
    struct opam_arg_s *arg = (struct opam_arg_s*)malloc(
                       sizeof(struct opam_arg_s));
    arg->val = strdup(ident.s);
    arg->t   = 1;
    ARG.arg = arg;
}

arg(ARG) ::= Varident(vid) . {
    LOG_DEBUG(0, "var arg: %s", vid.s);
    struct opam_arg_s *arg = (struct opam_arg_s*)malloc(
                       sizeof(struct opam_arg_s));
    arg->val = strdup(vid.s);
    arg->t   = 1;
    ARG.arg = arg;
}

arg ::= filter . {
    LOG_DEBUG(0, "filter arg", "");
}

/****************************************************************/
// depends and conflicts use same content model
//    [ <filtered-package-formula> ... ]
binding(Binding) ::=
    DEPENDS COLON LBRACKET filtered_package_formulas RBRACKET . {
#if YYDEBUG
        LOG_DEBUG(0, "BINDING depends", "");
#endif
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = strndup("depends", 7);
    Binding->t = BINDING_DEPENDS;
    /* Binding->val = ... will be list of pkgs */
    /* so here, alloc a UT hash table to hold the list */
    HASH_ADD_KEYPTR(hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name,
                    strlen(Binding->name),
                    Binding);
}

binding(Binding) ::=
    CONFLICTS COLON LBRACKET filtered_package_formulas RBRACKET . {
#if YYDEBUG
        LOG_DEBUG(0, "BINDING conflicts", "");
#endif
    Binding = calloc(sizeof(struct opam_binding_s), 1);
    Binding->name = strndup("conflicts", 9);
    Binding->t = BINDING_DEPENDS;
    /* Binding->val =  */
    HASH_ADD_KEYPTR(hh,
                    opam_parse_state->pkg->bindings,
                    Binding->name, strlen(Binding->name), Binding);
}

stringlist(Stringlist) ::= String(String) . {
#if YYDEBUG
    LOG_DEBUG(0, "STRINGLIST single: %s", String.s);
#endif
    utarray_new(Stringlist, &ut_str_icd);
    utarray_push_back(Stringlist, &String.s);
}

stringlist(Stringlist_lhs) ::= stringlist(Stringlist) String(String) . {
#if YYDEBUG
    LOG_DEBUG(0, "STRINGLIST multiple, ct: %d; new: %s",
           utarray_len(Stringlist), String.s);
#endif
    utarray_push_back(Stringlist, &String.s);
    Stringlist_lhs = Stringlist;
}

/* depends: [ <filtered-package-formula> ... ]
   <filtered-package-formula> ::=
   <filtered-package-formula> <logop> <filtered-package-formula>
   | ( <filtered-package-formula> )
   | <pkgname> { { <filtered-version-formula> }* }

    <pkgname>         ::= (") <ident> (")
*/
/*     pkgname ::= DQ Ident DQ . { */
/* #if YYDEBUG */
/*         printf("pkgname\n"); */
/* #endif */
/*     } */

    filtered_package_formulas ::= . {
#if YYDEBUG
        LOG_DEBUG(0, "filtered_package_formulas: null", "");
#endif
    }

    filtered_package_formulas ::= filtered_package_formulas fpf . {
#if YYDEBUG
        LOG_DEBUG(0, "filtered_package_formulas leftrec", "");
#endif
    }

    fpf ::= fpf LogOp fpf . {
#if YYDEBUG
        LOG_DEBUG(0, "fpf: fpf LogOp fpf", "");
#endif
    }

    fpf ::= LPAREN fpf RPAREN . {
#if YYDEBUG
        LOG_DEBUG(0, "fpf: (fpf)", "");
#endif
    }

    fpf ::= PkgName(Pkg) fvf_expr . {
        LOG_DEBUG(1, "fpf: pkgname fvf_expr:**************** %s",
                  Pkg.s);
    }

    fpf ::= PkgName(Pkg) . {
        LOG_DEBUG(0, "fpf: **************** pkgname: %s", Pkg.s);
        /* Deps = calloc(sizeof(struct opam_binding_s), 1); */
        /*  */
    }

    // ################################################################
    fvf_expr ::= LBRACE fvf RBRACE . {
      LOG_DEBUG(0, "fvf_expr: braces", "");
    }
/*
   <filtered-version-formula> ::=
   <filtered-version-formula> <logop> <filtered-version-formula>
   | "!" <filtered-version-formula>
   | "?" <filtered-version-formula>
   | "(" <filtered-version-formula> ")"
   | <relop> <version>
   | <filter>
   | <relop> <filter>

   e.g.   "ocaml" {>= "4.04.0"}
*/

    fvf ::= . {
#if YYDEBUG
        LOG_DEBUG(0, "fvf: null", "");
#endif
    }

        fvf ::= fvf logop_fvf . {
#if YYDEBUG
        LOG_DEBUG(0, "fvf: fvf logop_fvf", "");
#endif
    }

        logop_fvf ::= LogOp fvf . {
#if YYDEBUG
        LOG_DEBUG(0, "fvf: fvf logop fvf", "");
#endif
    }

/*         fvf ::=  BANG fvf . { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "fvf: bang fvf"); */
/* #endif */
/*     } */

/*         fvf ::= QMARK fvf . { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "fvf: qmark fvf"); */
/* #endif */
/*     } */

// | "(" <filtered-version-formula> ")"
/*         fvf ::=  LPAREN fvf RPAREN . { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "fvf: (fvf)"); */
/* #endif */
/*     } */

        // | <relop> <version>
/*         fvf_base ::= RelOp VERSION . { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "fvf_base: relop version"); */
/* #endif */
/*     } */

       fvf ::= fvf_base . {
#if YYDEBUG
        LOG_DEBUG(0, "fvf: fvf_base", "");
#endif
       }

/*        fvf ::= filter_expr . [AMP] { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "fvf: filter_expr", ""); */
/* #endif */
/*        } */

            fvf_base ::= VERSION . { // treated as <string>
#if YYDEBUG
        LOG_DEBUG(0, "fvf_base: version", "");
#endif
    }
        // | <filter>
/*         fvf_base ::= filter . { */
/* #if YYDEBUG */
/*             LOG_DEBUG(0, "fvf_base: filter"); */
/* #endif */
/*     } */

        // | <relop> <filter>
/*         fvf_base ::= RelOp filter . { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "fvf_base: relop filter"); */
/* #endif */
/*     } */

    /*
   <filter> ::= <filter> <logop> <filter>
   | "!" <filter>
   | "?" <filter>
   | ( <filter> )
   | <filter> <relop> <filter>
   | <varident>
   | <string>
   | <int>
   | <bool>

   NB: <varident>, <string>, <int>, <bool> are lexed as FILTER tokens
    */

/* arg ::= String build_filter . { */
/*     LOG_DEBUG(0, "arg TERM_STRING arg", ""); */
/* } */

/* build_filter ::= LBRACE Int(intgr) RBRACE . { */
/*     LOG_DEBUG(0, "{int}: %d", intgr.i); */
/* } */

/* build_filter_expr ::= String(str) . { */
/*     LOG_DEBUG(0, "filter pred: %s", str.s); */
/* } */

/* build_filter_expr ::= Int(intgr) . { */
/*     LOG_DEBUG(0, "filter pred: %d", intgr.i); */
/* } */

/* build_filter_expr ::= Varident(vi) . { */
/*     LOG_DEBUG(0, "filter pred:: varident %s", vi.s); */
/* } */

/* build_filter_expr ::= build_filter_expr LogOp(logop) build_filter_expr . { */
/*     LOG_DEBUG(0, "compound filter: <pred> %s <pred>", logop.s); */
/* } */

/* build_filter_expr ::= build_filter_expr RelOp build_filter_expr . { */
/*     LOG_DEBUG(0, "build_filter RelOp", ""); */
/* } */

/* build_filter_expr ::= FILTER . { */
/*     LOG_DEBUG(0, "build_filter"); */
/* } */

// build_filter_expr = string | var | int
/* build_filter ::= LBRACE build_filter_expr RBRACE . { */
/*     LOG_DEBUG(0, "{filter_expr}", ""); */
/* } */

filter ::= LBRACE pred RBRACE . {
    LOG_DEBUG(0, "{pred}", "");
}
pred ::= pred LogOp(logop) pred . {
    LOG_DEBUG(0, "compound pred: <pred> %s <pred>", logop.s);
}

pred ::= pterm . {
    LOG_DEBUG(0, "pred", "");
}

/* predicate terms:: int | string | var */
/* =>  struct opam_pterm_s */
pterm ::= Int(vid) . {
/* pred ::= Int(vid) . { */
    LOG_DEBUG(0, "pred int: %s", vid.s);
}
pred ::= String(str) . {
    LOG_DEBUG(0, "pred str: %s", str.s);
}
pterm ::= Varident(vid) . {
    LOG_DEBUG(0, "pterm var: %s", vid.s);
}
pred ::= BANG pred . {
        LOG_DEBUG(0, "pred: BANG filter", "");
}
pred ::= QMARK pred . {
    LOG_DEBUG(0, "pred: QMARK pred", "");
}
pred ::= LPAREN pred RPAREN . {
    LOG_DEBUG(0, "pred: (pred)", "");
}
pred ::= pred RelOp(relop) pred . {
        LOG_DEBUG(0, "pred: pred %s pred", relop.s);
}

/* filter ::= LBRACE Varident(vid) RBRACE . { */
/*     LOG_DEBUG(0, "FILTER varid: %s", vid.s); */
/* } */

/* filter ::= LBRACE Int(intgr) RBRACE . { */
/*     LOG_DEBUG(0, "filter int: %d", intgr.i); */
/* } */

/* filter ::= LBRACE String(str) RBRACE . { */
/*     LOG_DEBUG(0, "filter str: %s", str.s); */
/* } */

/* filter ::= LBRACE FILTER RBRACE . { */
/*     LOG_DEBUG(0, "filter", ""); */
/* } */

/* filter_expr ::= filter . [LogOp] { */
/*     LOG_DEBUG(0, "filter_expr: filter", ""); */
/*     } */

/* filter_expr ::= filter LogOp filter . { */
/*     LOG_DEBUG(0, "filter_expr: filter_expr logop_filter", ""); */
/* } */

/* filter ::= RelOp VERSION . { */
/*     LOG_DEBUG(0, "filter: relop version", ""); */
/* } */

/*         logop_filter ::= LogOp filter . { */
/* #if YYDEBUG */
/*         LOG_DEBUG(0, "logop_filter: logop filter"); */
/* #endif */
/*     } */

/* filter ::= FILTER . { // varident|string|int|bool */
/*         LOG_DEBUG(0, "filter: FILTER", ""); */
/* } */

/* filter ::= Varident . { */
/*     LOG_DEBUG(0, "filter: Varident", ""); */
/* } */

string3 ::= TQ String3 TQ . {
}

string3 ::= TQ String3 String String3 TQ . {
}
