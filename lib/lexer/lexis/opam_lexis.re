// opam lexer
// re2c $INPUT -o $OUTPUT -i

/* NB: constants for terminals generated from opam_parser.y.
   makeheaders arranges so that any needed here are included in
   opam_lexis.h. Isn't that sweet?
 */

#include <assert.h>
#include <errno.h>
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>

#include "liblogc.h"
#include "opam_lexis.h"

#if EXPORT_INTERFACE
#define BUFSIZE 1024
#ifndef MAX_DEPS
#define MAX_DEPS 64
#endif

#define ERR_VARIDENT -1

#endif

const char *deps[MAX_DEPS];
int opam_curr_tag = 0;

/* enable start conditions */
/*!types:re2c */

#define DEBUG_LEVEL debug_opamc_lexis
int  DEBUG_LEVEL;
#define TRACE_FLAG trace_opamc_lexis
bool TRACE_FLAG;

#define MODE_NAME(x) (char*)#x
char *opam_lexer_modes[256] = {
    [yyc0]           = MODE_NAME(*),
    [yycinit]        = MODE_NAME(init),
    [yycflags]       = MODE_NAME(flags),
    [yycflag]        = MODE_NAME(flag),
    [yyccmdlist]     = MODE_NAME(cmdlist),
    [yyccmds]        = MODE_NAME(cmds),
    [yycterm]        = MODE_NAME(term),
    [yycfilter]      = MODE_NAME(filter),
    [yycdepends]     = MODE_NAME(depends),
    [yycfpf]         = MODE_NAME(fpf),
    [yycfvf]         = MODE_NAME(fvf),
    [yyctriplequote] = MODE_NAME(triplequote),
};
#define LEXMODE(m) opam_lexer_modes[m]

#if EXPORT_INTERFACE
#include <stdbool.h>
#include "utarray.h"
union opam_token_u {
    char c;
    int i;
    char *s;
    bool boolval;
    UT_array *cmds;
    struct opam_cmd_s *cmd;
    UT_array *args;
    struct opam_arg_s *arg;
    char *filter;
};

struct opam_lexer_s
{
    const char *src;
    const char *start;
    int        line;
    int        pos; /* from start of line */
    const char *tok;
    const char *cursor;
    const char *limit;
    const char *marker;
    int mode;                   /* i.e. start condition */
};

#endif


EXPORT void opam_lexer_init(const char*src,
                     struct opam_lexer_s *lexer,
                     const char *input)
{
    lexer->src    = src;
    lexer->start  = input;
    lexer->cursor = input;
}

EXPORT void opam_lexer_free(opam_lexer_s *lexer)
{
    /* log_debug("lexer_free: %s", lexer->fname); */
    /* utarray_free(lexer->indent_kws); */
}

/* static void mtag(const char *t) */
// static void mtag(int t)
// {
//     /* fprintf(stderr, "mtag ctor idx: %d, s: %.22s\n", opam_curr_tag, t); */
//     /* deps[opam_curr_tag++] = t; */
// }


/* static void print_tags() { */
/*     /\* fprintf(stderr, "printing %d tags:\n", opam_curr_tag/2); *\/ */
/*     for (int i=0; i < opam_curr_tag/2 ; i++) { */
/*         fprintf(stderr, "\tVALTOK: '%*s'\n", (int)(deps[i*2+1] - deps[i*2]), deps[i*2]); */
/*     } */
/*     /\* fprintf(stderr, "done\n"); *\/ */
/* } */

#define YYMTAGP(t) mtag(YYCURSOR)
#define YYMTAGN(t) mtag(NULL)

//#define LEXDEBUG_FVF 1

static bool start = true;

EXPORT int get_next_opam_token(struct opam_lexer_s *lexer, union opam_token_u *otok)
{
    LOG_DEBUG(0, "get_next_opam_token <%s>: '%s'",
              LEXMODE(lexer->mode),
              lexer->cursor);
    if (start) {
        LOG_DEBUG(0, "STARTING LEXER", "");
        LOG_DEBUG(0, "lexer start mode: %d", lexer->mode);
        LOG_DEBUG(0, "\t<*>: %d", yyc0);
        LOG_DEBUG(0, "\t<init>: %d", yycinit);
        LOG_DEBUG(0, "\t<flags>: %d", yycflags);
        LOG_DEBUG(0, "\t<flag>: %d", yycflag);
        LOG_DEBUG(0, "\t<cmds>: %d", yyccmds);
        LOG_DEBUG(0, "\t<cmdlist>: %d", yyccmdlist);
        LOG_DEBUG(0, "\t<term>: %d", yycterm);
        LOG_DEBUG(0, "\t<filter>: %d", yycfilter);
        LOG_DEBUG(0, "\t<fvf>: %d", yycfvf);
	/* yyc0, */
	/* yycinit, */
	/* yyccmdlist, */
	/* yyccmds, */
	/* yycterm, */
	/* yycfilter, */
	/* yycdepends, */
	/* yycfpf, */
	/* yycfvf, */
	/* yyctriplequote */
    }
#if defined(LEXDEBUG_FVF)
    // only set lexer->mode on initial call
    /* static bool start = true; */
    if (start) {
        LOG_DEBUG(0, "yycinit: %d, yycdepends: %d, yycfpf %d, yycfvf: %d",
              yycinit, yycdepends, yycfpf, yycfvf);
        lexer->mode = yycfpf;
        LOG_DEBUG(0, "start mode: %d", lexer->mode);
        start = false;
    }
#elif defined (LEXDEBUG_FPF)
    /* static bool start = true; */
    if (start) {
        LOG_DEBUG(0, "yycinit: %d, yycdepends: %d, yycfpf %d, yycfvf: %d",
              yycinit, yycdepends, yycfpf, yycfvf);
        lexer->mode = yycfpf;
        LOG_DEBUG(0, "start mode: %d", lexer->mode);
        start = false;
    }
#elif defined (LEXDEBUG_FILTERS)
    if (start) {
        LOG_DEBUG(0, "yycinit: %d, yycdepends: %d, yycfpf %d, yycfvf: %d",
              yycinit, yycdepends, yycfpf, yycfvf);
        lexer->mode = yycfvf;
        LOG_DEBUG(0, "start mode: %d", lexer->mode);
        start = false;
    }
#else
    if (start) {
        lexer->mode = yycinit;
        LOG_DEBUG(0, "lexer mode: %d", lexer->mode);
        start = false;
    }
    /* LOG_DEBUG(0, "lexer mode: %d", lexer->mode); */
#endif

    /* stags */
    const char *s1, *s2, *s3, *s4;
    (void)s3;
    (void)s4;
    /* const char *pkg1, *pkg2; */
    /* const char *pred1, *pred2; */
    /* const char *txt1, *txt2; */

    /* mtags */
    /* int f1, f2; */                 /* flags */
    /* int dep1, dep2; */
    /* const char *vtok1, *vtok2; */

    /* int h1, h2; */
    /*!stags:re2c format = "const char *@@;"; */
    /*!mtags:re2c format = "int @@;"; */
loop:
    opam_curr_tag = 0;
    lexer->tok = lexer->cursor;
    /*!mtags:re2c format = "@@ = -1;"; */
    /*!re2c
      re2c:api:style = free-form;

      re2c:define:YYCTYPE = char;
      re2c:define:YYCURSOR = lexer->cursor;
      re2c:define:YYLIMIT = lexer->limit;
      re2c:define:YYMARKER = lexer->marker;
      re2c:define:YYGETCONDITION = "lexer->mode";
      re2c:define:YYSETCONDITION = "lexer->mode = @@;";

      //re2c:define:YYMTAGP = mtag;
      //re2c:define:YYMTAGN = mtag;
      re2c:yyfill:enable  = 0;

/* TODO: regexp for path strings, for DIRECTORY variable
   reserve UPCASE for token constants */

      re2c:flags:tags = 1;

        end    = "\x00";
        eol    = [\n];
        ws     = [ \t]*;
        listws = [ \t\n,]*;
        wsnl   = [ \t\n]*;

        Dq      = "\"";
        TQ      = "\"\"\"";

        /* EQ      = wsnl "=" wsnl; */

        PATH    = [a-zA-Z0-9+/._]+;
        Letter  = [a-zA-Z];
        Digit   = [0-9];

        Int =  "-"? Digit+;

        Bool = ("true" | "false");

        // <string> ::= ( (") { <char> }* (") ) | ( (""") { <char> }* (""") )
        String = (TQ [^"]* TQ) | (Dq [^"]* Dq);

        //<identchar>     ::= <letter> | <digit>  | "_" | "-" | "+"
        Identchar = Letter | Digit | "_" | "-" | "+" ;

        // <ident> ::= { <identchar> }* <letter> { <identchar> }*
        Ident = Identchar* Letter Identchar*;

        // [(<ident>|"_"){ "+" ( <ident> | "_" ) }* : ] <ident>
        Varident = (Ident | "_") ("+" (Ident | "_"))* ":" Ident;

        Flag = ("light-uninstall"
                | "verbose"
                | "plugin");

        Logop = ("&" | "|");
        Relop = ("=" | "!=" | "<" | "<=" | ">" | ">=");
       // <version> ::= (") { <identchar> | "." | "~" }+ (")
        Version = "\"" (Identchar | "." | "~")+ "\"";


        // for debugging we may want to start with any start condition.
        // this <> seems to be necessary to initialize all yyc* vars
        <> {
            /* printf("yycinit: %d, yycdepends: %d, yycfvf: %d\n", */
            /*        yycinit, yycdepends, yycfvf); */
            /* lexer->mode = yycdepends; */
        }

        <*> eol { lexer->line++;
                  lexer->pos = 0;
                  LOG_DEBUG(0, "newline %d", lexer->line);
                  goto loop; }

        /* <init> "&"  { return AMP; } */
        <init> "(" { lexer->pos++; return LPAREN; }
        <init> ")" { lexer->pos++; return RPAREN; }
        <init> "\[" { lexer->pos++; return LBRACKET; }
        <init> "\]" { LOG_DEBUG(0, "<init> RBRACKET", "");
lexer->pos++; return RBRACKET; }
        <init> "{" => fvf {
            lexer->pos++;
            LOG_DEBUG(0, "<%s>: '{' => <%s>",
                      LEXMODE(yycinit), LEXMODE(yycfvf));
            return LBRACE; } // { starts either a filter or an fvf
        <init> "}" { lexer->pos++; return RBRACE; }

        <init> ":" { lexer->pos++; return COLON; }
        <init> "," { lexer->pos++; return COMMA; }

        // <string> ::= ( (") { <char> }* (") ) | ( (""") { <char> }* (""") )

        /* opam-version: "2.0" */
        <init> "flags" @s1 ws @s2 ":" => flags {
            LOG_DEBUG(0, "line %d, pos %d",
                      lexer->line, lexer->pos);
            lexer->pos += 5 + (size_t)(s2-s1) + 1;
            LOG_DEBUG(0, "fld(flags) => flags mode: %d",
                      lexer->mode);
            return FLAGS;
        }
        /* <flags> ws "verbose" ws { */
        /*     LOG_DEBUG(0, "flags", ""); */
        /*     return FLAGS; */
        /* } */
        <flags> ws "[" ws => flag {
            lexer->pos++;
            LOG_DEBUG(0, "[flags...", "");
            return LBRACKET;
        }
        <flag> ws "]" ws => init {
            lexer->pos++;
            LOG_DEBUG(0, "..flags]", "");
            return RBRACKET;
        }

        <flag> @s1 Ident @s2 {
            lexer->pos += (size_t)(s2-s1);
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "Flag: %s", otok->s);
            return FLAG;
        }

        <init> @s1 ("opam-version"
             | "version"
             | "maintainer"
             | "authors"
             | "name"
             | "license"
             | "homepage"
             | "doc"
             | "bug-reports"
             | "dev-repo"
             | "tags"
             | "patches"
             | "substs"
             | "build-doc"
             | "remove"
             /* | "depends" */
             | "depopts"
             | "depexts"
             | "synopsis"
             | "description"
             ) @s2 @s3 ws @s4 ":" {
            lexer->pos += (size_t)(s2-s1);
            lexer->pos += (size_t)(s4-s3) + 1;
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "<%s>: Fldname(%s)",
                      LEXMODE(yycinit), otok->s);
            return FldName;
        }

        /* cmdlist fields */
        <init> @s1 ("build"
                    | "install"
                    | "run-test"
                    | "remove"
                    | "build-doc"   /* deprecated */
                    | "build-test") /* deprecated */
               @s2 @s3 ws @s4 ":" => cmdlist {
            LOG_DEBUG(0, "line %d, pos %d",
                      lexer->line, lexer->pos);
            lexer->pos += (size_t)(s2-s1);
            lexer->pos += (size_t)(s4-s3);
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "<%s>: CmdListFld(%s)",
                      LEXMODE(yyc0), otok->s);
            return CmdListFld;
        }

        /* local config file grammar */
        /* <local-config> @s1 ("wrap-build-commands" */
        /*             | "pre-build-commands" */
        /*             | "post-build-commands" */
        /*             | "wrap-install-commands" */
        /*             | "pre-install-commands" */
        /*             | "post-install-commands" */
        /*             | "wrap-remove-commands" */
        /*             | "pre-remove-commands" */
        /*             | "post-remove-commands" */
        /*             | "pre-session-commands" */
        /*             | "post-session-commands" */
        /*      ) @s2 @s3 ws @s4 ":" { */
        /*     otok->s = strndup(s1, (size_t)(s2-s1)); */
        /*     return FldName; */
        /* } */


        /*     return INSTALL; */
        /* } */
        /* <init> "run-test" ws ":" => cmdlist { */
        /*     LOG_DEBUG(0, "run-test fld => cmdlist mode: %d", lexer->mode); */
        /*     return RUNTEST; */
        /* } */

        /* fpf-valued flds: depends, conflicts;
           depopts partially */
        <init> "depends" @s1 ws @s2 ":" {
            LOG_DEBUG(0, "line %d, pos %d",
                      lexer->line, lexer->pos);
            lexer->pos += 8 + (size_t)(s2-s1);
            otok->s = strndup("depends", 7);
            LOG_DEBUG(0, "<%s>: DEPENDS(%s)",
                      LEXMODE(yyc0), otok->s);
            return DEPENDS;
        }


        /* <cmds> ":" { return COLON; } */
        <cmdlist> "[" => cmds {
            lexer->pos++;
            LOG_DEBUG(0, "<%s>: LBRACKET",
                      LEXMODE(yyccmdlist));
            //LEXMODE(yyccmdlist));
            return LBRACKET; }

        <cmds> "[" => term { // rename: cmd
            lexer->pos++;
            LOG_DEBUG(0, "<%s>: LBRACKET",
                      LEXMODE(yyccmds), otok->s);
            return LBRACKET; }
        <cmds> "]" => init {
            lexer->pos++;
            // always marks end of cmds fld so back to <init>
            // s/b <cmdlist> "]" => init ??
            LOG_DEBUG(0, "<cmds> RBRACKET", "");
            return RBRACKET;
        }
        /*  [ <term> { <filter> } ... ] { <filter> }  */
        <cmds> "{" => filter {
            lexer->pos++;
            LOG_DEBUG(0, "<cmds> lbrace mode %d", lexer->mode);
            return LBRACE;
        }

        // <term> ::= <string> | <varident>
        <term> "\"" @s1 ([^"] | "\\\"" )* @s2 "\"" {
                lexer->pos += (size_t)(s2-s1) +2;
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "String term: %s", otok->s);
                return String;
        }
        <term> @s1 Varident @s2 {
                lexer->pos += (size_t)(s2-s1);
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "Varident term: %s", otok->s);
                return Varident;
        }

        <term> @s1 Ident @s2 {
                lexer->pos += (size_t)(s2-s1);
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "Ident term: %s", otok->s);
                return Ident;
        }

        /* [ <term> { <filter> } ... ] */
        <term> "{" => filter {
                lexer->pos++;
                LOG_DEBUG(0, "<term> lbrace mode %d", lexer->mode);
                return LBRACE; }

        <filter> "}" => term {
                lexer->pos++;
                LOG_DEBUG(0, "<filter> rbrace mode %d", lexer->mode);
                return RBRACE; }
        <filter> "}" => cmds {
                lexer->pos++;
                LOG_DEBUG(0, "<filter> rbrace mode %d", lexer->mode);
                return RBRACE; }

     // <filter> ::= varident | string | int | bool
     //              | ... compounds
        <filter> @s1 Varident @s2 {
                lexer->pos += (size_t)(s2-s1);
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "<%s>:  Varident(%s)",
                          LEXMODE(yycfilter), otok->s);
                return Varident;
        }
        <filter> @s1 String @s2 {
                lexer->pos += (size_t)(s2-s1);
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "<%s>:  String(%s)",
                          LEXMODE(yycfilter), otok->s);
                return String;
        }
        /* <filter> @s1 Int @s2 { */
        /*         lexer->pos += (size_t)(s2-s1); */
        /*         char *s = strndup(s1, (size_t)(s2-s1)); */
        /*         otok->i = atoi(s); */
        /*         free(s); */
        /*         LOG_DEBUG(0, "<%s>:  Int(%s)", */
        /*                   LEXMODE(yycfilter), otok->s); */
        /*         return Int; } */

        /* <filter> @s1 Bool @s2 { */
        /*         lexer->pos += (size_t)(s2-s1); */
        /*         char *s = strndup(s1, (size_t)(s2-s1)); */
        /*         otok->s = s; */
        /*         free(s); */
        /*         LOG_DEBUG(0, "<%s>: Bool(%s)", */
        /*                   LEXMODE(yycfilter), otok->s); */
        /*         return Bool; } */

        <filter> @s1 Letter @s2 {
                lexer->pos += (size_t)(s2-s1);
                char *s = strndup(s1, (size_t)(s2-s1));
                LOG_ERROR(0, "Bad varident %s", s);
                otok->s = s;
                return ERR_VARIDENT; }


        <term> "[" { lexer->pos++; return LBRACKET; }
        <term> "]" => cmds {
                lexer->pos++;
                LOG_DEBUG(0, "<term> RBRACKET", "");
                return RBRACKET; }

        <init> "conflicts" => depends {
            lexer->pos += 9;
            return CONFLICTS;
        }
        <init> "depends" => depends {
            lexer->pos += 7;
            return DEPENDS;
        }

        <depends> ":" { lexer->pos++; return COLON; }
        <depends> "[" => fpf { lexer->pos++; return LBRACKET; }

        /*  PF Package-Formulas */
        /*  not used in opam pkg files, only in config files */

        /*  FPF Filtered-Package-Formulas */
        <fpf> Dq @s1 Ident @s2 Dq {
              lexer->pos += (size_t)(s2-s1) + 2;
              otok->s = strndup(s1, (size_t)(s2-s1));
              return PkgName;
        }
        <fpf> "(" { lexer->pos++; return LPAREN; }
        <fpf> ")" => init { lexer->pos++; return RPAREN; }
        <fpf> "]" => init { lexer->pos++; return RBRACKET; }
        <fpf> "{" => fvf {
            lexer->pos++;
            LOG_DEBUG(0, "mode %d: LBRACE", lexer->mode);
            return LBRACE;
        }

        <fvf> "}" => fpf {
            LOG_DEBUG(0, "<%s>: '}' => <%s>",
                      LEXMODE(yycfvf), LEXMODE(yycfpf));
            lexer->pos++; return RBRACE;
        }

        <fvf> "(" { lexer->pos++; return LPAREN; }
        <fvf> ")" { lexer->pos++; return RPAREN; }

        <fvf> @s1 Logop @s2 {
            lexer->pos += (size_t)(s2-s1);
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "<%s>:  LogOp(%s)",
                      LEXMODE(yycfvf),
                      otok->s);
            return LogOp;
        }

        /* <fvf> Dq @s1 ( Identchar | "+" | "." | "~" )+ @s2 Dq { */
        /*       lexer->pos += (size_t)(s2-s1) + 2; */
        /*       otok->s = strndup(s1, (size_t)(s2-s1)); */
        /*       LOG_DEBUG(0, "<fvf> %s", otok->s); */
        /*       return String; */
        /* } */

        <fvf> @s1 Int @s2 {
                lexer->pos += (size_t)(s2-s1);
                char *s = strndup(s1, (size_t)(s2-s1));
                otok->i = atoi(s);
                free(s);
                LOG_DEBUG(0, "<%s>:  Int(%d)",
                          LEXMODE(yycfilter), otok->i);
                return Int;
        }

        /* <fvf> @s1 Version @s2 { */
        /*         lexer->pos += (size_t)(s2-s1); */
        /*         otok->s = strndup(s1, (size_t)(s2-s1)); */
        /*         LOG_DEBUG(0, "<%s>: VERSION(%s)", */
        /*                   LEXMODE(yycfvf), otok->s); */
        /*         return String; */
        /* } */

        <fvf> @s1 String @s2 {
                lexer->pos += (size_t)(s2-s1);
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "<%s>: String(%s)",
                          LEXMODE(yycfvf), otok->s);
                return String;
        }

        /* <fvf,filter,filter> @s1 Int @s2 { */
        /*         otok->s = strndup(s1, (size_t)(s2-s1)); */
        /*         return FILTER; */
        /* } */
        /* <fvf> @s1 Bool @s2 { */
        /*         lexer->pos += (size_t)(s2-s1); */
        /*         otok->s = strndup(s1, (size_t)(s2-s1)); */
        /*         LOG_DEBUG(9, "<fvf> 4", ""); */
        /*         return Bool; */
        /* } */
        <fvf> @s1 Varident @s2 {
                lexer->pos += (size_t)(s2-s1);
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "<%s>: Varident(%s)",
                          LEXMODE(yycfvf), otok->s);
                return Varident;
        }


       // Int
        <*> @s1 Int @s2 {
                lexer->pos += (size_t)(s2-s1);
                char *s = strndup(s1, (size_t)(s2-s1));
                otok->i = atoi(s);
                free(s);
                LOG_DEBUG(0, "<%s>: Int(%d)",
                          LEXMODE(yycinit), otok->i);
                return Int;
        }
       // Bools
        <init> @s1 Bool @s2 {
           lexer->pos += (size_t)(s2-s1);
           int len = (size_t)(s2-s1);
           LOG_DEBUG(0, "len %d", len);
           char *str = strndup(s1, len);
           LOG_DEBUG(0, "strlen %d", strlen(str));
           /* s[(size_t)(s2-s1)] */
           otok->s = str;
           /* free(str); */
           LOG_DEBUG(0, "<%s>: Bool(%s)",
                     LEXMODE(yycinit), otok->s);
           return Bool;
            }


        // STRINGS
        // <string> ::= ( (") { <char> }* (") ) | ( (""") { <char> }* (""") )
        // triple-quoted - this doesn't quite get it right
        <init> "\"\"\"" => triplequote {
                lexer->pos+=3; return TQ; }
        <triplequote> "\"\"\"" => init {
                lexer->pos+=3; return TQ; }
        <triplequote> @s1 "\"" ([^"] | "\\\"" )* "\"" @s2 {
            lexer->pos += (size_t)(s2-s1);
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "triple q %s", otok->s);
            return String;
        }
        <triplequote> @s1 [^"]* @s2 {
            lexer->pos += (size_t)(s2-s1);
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "triple q2 %s", otok->s);
            return String3;
        }

        // single-quoted
        /* <init> "\"" @s1 ([^"] | "\\\"" )+ @s2 "\"" { */
        <init> @s1 String @s2 {
                otok->s = strndup(s1, (size_t)(s2-s1));
                LOG_DEBUG(0, "<%s>: String(\"%s\")",
                          LEXMODE(yycinit), otok->s);
                return String;
            }
        /* "," { goto loop; } */

        <*> @s1 ws @s2 {
                lexer->pos += (size_t)(s2-s1);
                goto loop; }

        <*> "#" .* eol {
            /* return COMMENT; */
            lexer->line++;
            goto loop;          /* skip comments */
        }

        <init> "(\*" .* "\*)" {
            // comment
            goto loop;
        }

        <*> @s1 Varident @s2 {
            otok->s = (char*) strndup(s1, (size_t)(s2 - s1));
            LOG_DEBUG(0, "<%s>: Varident(%s)",
                      LEXMODE(yyc0), otok->s);
            return Varident;
        }

        <init> @s1 "=" @s2 {
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "EQ relop: %s", otok->s);
            return RelOp;
        }

        /* <init> "\"" { return DQ; } */
        /* <init> "'" { return SQ; } */

        /* <*> @s1 ("=" | "!=" | "<" | "<=" | ">" | ">=") @s2 { */
        <*> @s1 Relop @s2 {
            lexer->pos += (size_t)(s2-s1);
            otok->s = strndup(s1, (size_t)(s2-s1));
            LOG_DEBUG(0, "<%s>: RelOp(%s)",
                      LEXMODE(yyc0), otok->s);
            return RelOp;
        }
        <*> "!" {
            LOG_DEBUG(0, "%d: '!' (<*>)", lexer->mode);
            lexer->pos++; return BANG; }
        <*> "?" {
            LOG_DEBUG(0, "%d: '?' (<*>)", lexer->mode);
            lexer->pos++; return QMARK; }
        <*> @s1 ("&" | "|") @s2 {
            lexer->pos++;
            LOG_DEBUG(0, "<%s>: LogOp(%s)",
                      LEXMODE(yyc0), otok->s);
            otok->s = strndup(s1, (size_t)(s2-s1));
            return LogOp;
        }

        <*>*         {
            LOG_ERROR(0, "lexing, pos %d", lexer->pos);
            LOG_DEBUG(0, "ERROR lexing: %s: %s",
                    lexer->src, lexer->tok);
            return(-2);
        }

        <*> end       {
            LOG_DEBUG(0, "lexing completed in mode %d", lexer->mode);
            return 0;
        }

        <*> ws | eol {
            // printf("looping\n");
            goto loop;
        }

    */
}
