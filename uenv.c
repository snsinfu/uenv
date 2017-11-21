// MIT License
//
// Copyright (c) 2017 frickiericker
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

static void show_usage(void);
static void clear_env(void);
static int init_path(void);
static int load_env(const char *envfile);
static int do_load_env(FILE *file, char *buf, size_t bufsize);
static int is_comment(const char *line);
static int trim_newline(char *str);
static int split_key_value(char *buf, char **key, char **value);

int main(int argc, char **argv)
{
    const int error_code = 111; // Same as djb's envdir.

    int want_clear = 0;
    const char *envfile = NULL;
    const char *default_envfile = ".env";

    for (int ch; (ch = getopt(argc, argv, "cf:h")) != -1; ) {
        switch (ch) {
          case 'c':
            want_clear = 1;
            break;

          case 'f':
            envfile = optarg;
            break;

          case 'h':
            show_usage();
            return 0;

          default:
            return error_code;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        return 0;
    }

    if (want_clear) {
        clear_env();
        init_path();
    }

    if (envfile == NULL && access(default_envfile, F_OK) != -1) {
        envfile = default_envfile;
    }
    if (envfile && load_env(envfile) == -1) {
        return error_code;
    }

    execvp(argv[0], argv);
    fprintf(stderr, "Failed to execute %s: %s\n", argv[0], strerror(errno));
    return error_code;
}

// show_usage prints help to stderr.
void show_usage(void)
{
    const char *msg =
        "Usage: uenv [-ch] [-f envfile] command ...\n"
        "\n"
        "Options\n"
        "  -c          clear environment variables first\n"
        "  -f envfile  load environment variables from envfile (default: .env)\n"
        "  -h          show this help\n"
        "\n";
    fputs(msg, stderr);
}

// clear_env unsets all environment variables.
void clear_env(void)
{
    extern char **environ;
    static char *empty[1];
    environ = empty;
}

// init_path sets PATH to the system default.
//
// Returns 0 on success, or -1 on error.
int init_path(void)
{
    int return_value = -1;
    char *path = NULL;

    size_t size = confstr(_CS_PATH, NULL, 0);
    path = malloc(size);
    if (path == NULL) {
        perror("malloc");
        goto cleanup;
    }

    if (confstr(_CS_PATH, path, size) == 0) {
        fprintf(stderr, "Failed to get the system PATH: %s\n", strerror(errno));
        goto cleanup;
    }

    if (setenv("PATH", path, 1) == -1) {
        fprintf(stderr, "Failed to set PATH: %s\n", strerror(errno));
        goto cleanup;
    }
    return_value = 0;

  cleanup:
    free(path);
    return return_value;
}

// load_env loads key=value pairs from a file and sets environment variables
// accordingly.
//
// Returns 0 on success, or -1 on error.
int load_env(const char *envfile)
{
    int return_value = -1;
    FILE *file = NULL;
    char *buf = NULL;

    file = fopen(envfile, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open %s: %s\n", envfile, strerror(errno));
        goto cleanup;
    }

    const size_t bufsize = 32768;
    buf = malloc(bufsize);
    if (buf == NULL) {
        perror("malloc");
        goto cleanup;
    }

    if (do_load_env(file, buf, bufsize) == -1) {
        goto cleanup;
    }
    return_value = 0;

  cleanup:
    free(buf);
    fclose(file);
    return return_value;
}

// do_load_env loads key=value pairs from an open file using given buffer and
// sets environment variables accordingly.
//
// Returns 0 on success, or -1 on error.
int do_load_env(FILE *file, char *buf, size_t bufsize)
{
    for (int line_number = 1; ; line_number++) {
        if (fgets(buf, (int) bufsize, file) == NULL) {
            if (ferror(file)) {
                fprintf(stderr, "Error reading line %d: %s\n",
                        line_number, strerror(errno));
                return -1;
            }
            return 0; // End of file.
        }

        if (trim_newline(buf) == -1) {
            fprintf(stderr, "Line %d is too long or not terminated\n",
                    line_number);
            return -1;
        }

        if (is_comment(buf)) {
            continue;
        }

        char *key;
        char *value;
        if (split_key_value(buf, &key, &value) == -1) {
            fprintf(stderr, "No key=value pair in line %d\n",
                    line_number);
            return -1;
        }

        if (setenv(key, value, 1) == -1) {
            fprintf(stderr, "Failed to set environment at line %d: %s\n",
                    line_number, strerror(errno));
            return -1;
        }
    }
}

// is_comment determines if a line is a comment.
//
// Returns nonzero if the line is a comment, or 0 otherwise.
int is_comment(const char *line)
{
    return *line == '\0' || *line == '#';
}

// trim_newline replaces the first newline character in str with a nul
// character.
//
// Returns 0 on success, or -1 if no newline is found.
int trim_newline(char *str)
{
    char *pos = strchr(str, '\n');
    if (pos == NULL) {
        return -1;
    }
    *pos = '\0';
    return 0;
}

// split_key_value splits a string at the first occurrence of '='.
//
// The delimiter '=' in buf is replaced by a nul character, the left-hand side
// is assigned to *key, and the right-hand side is assigned to *value.
//
// Returns 0 on success, or -1 if no delimiter is found.
int split_key_value(char *buf, char **key, char **value)
{
    char *delim = strchr(buf, '=');
    if (delim == NULL || delim == buf) {
        return -1;
    }
    *delim = '\0';
    *key = buf;
    *value = delim + 1;
    return 0;
}
