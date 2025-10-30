#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXARGS 128
#define MAXLINE 1024

/* Prototypes */
static void eval(char *cmdline);
static int  parseline(char *buf, char **argv);
static int  builtin_command(char **argv);

int main(void) {
    char cmdline[MAXLINE];

    while (1) {
        printf("\033[1;32mcomputer-wiz\033[1;33m$ \033[0m");
        if (!fgets(cmdline, MAXLINE, stdin)) {   // EOF or error
            putchar('\n');
            break;
        }
        eval(cmdline);
    }
    return 0;
}

/* eval — evaluate a command line */
static void eval(char *cmdline) {
    char *argv[MAXARGS];
    char  buf[MAXLINE];
    int   bg;                /* run in background? */
    pid_t pid;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL) return;   /* empty line */

    if (!builtin_command(argv)) {
        if ((pid = fork()) == 0) {                     /* child */
            /* Use execvp so PATH is searched; inherits current environment */
            execvp(argv[0], argv);
            /* If we get here, exec failed */
            fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            _exit(127);
        }

        if (!bg) {                                     /* foreground: wait */
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
            }
        } else {                                       /* background: just report */
            printf("[%d] %s", pid, cmdline);
            if (cmdline[strlen(cmdline)-1] != '\n') putchar('\n');
        }
    }
}

/* Built-ins: quit/exit, cd, pwd, ignore singleton & */
static int builtin_command(char **argv) {
    if (!strcmp(argv[0], "quit") || !strcmp(argv[0], "exit")) {
        exit(0);
    }
    if (!strcmp(argv[0], "&")) {           /* ignore lone & */
        return 1;
    }
    if (!strcmp(argv[0], "cd")) {
        const char *target = argv[1] ? argv[1] : getenv("HOME");
        if (!target) target = ".";
        if (chdir(target) < 0) {
            fprintf(stderr, "cd: %s: %s\n", target, strerror(errno));
        }
        return 1;
    }
    if (!strcmp(argv[0], "pwd")) {
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd))) {
            puts(cwd);
        } else {
            perror("pwd");
        }
        return 1;
    }
    return 0;  /* not a built-in */
}

/* parseline — build argv; return 1 if background (&), else 0 */
static int parseline(char *buf, char **argv) {
    char *delim;
    int argc = 0;
    int bg = 0;

    /* replace trailing newline with space */
    size_t n = strlen(buf);
    if (n > 0 && buf[n-1] == '\n') buf[n-1] = ' ';

    /* skip leading spaces */
    while (*buf && *buf == ' ') buf++;

    /* build argv */
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && *buf == ' ') buf++;
        if (argc >= MAXARGS-1) break;
    }
    argv[argc] = NULL;

    if (argc == 0) return 0;   /* blank line */

    /* background if last arg is exactly "&" */
    if (strcmp(argv[argc-1], "&") == 0) {
        bg = 1;
        argv[--argc] = NULL;
    }
    return bg;
}

