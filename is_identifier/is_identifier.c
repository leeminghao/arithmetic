#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    START, INID, DONE,
} StateType;

typedef enum {
    ID, ENDFILE, ERROR,
} TokenType;

static FILE *s_source = NULL;
static FILE *s_listing = NULL;

#define BUFLEN 256
static char s_line_buf[BUFLEN];
static int s_line_pos = 0;
static int s_buf_size = 0;

static int s_EOF_flag = 0;

#define MAXTOKENLEN 100
char s_token_string[MAXTOKENLEN+1];

static int get_next_char(void)
{
    if (!(s_line_pos < s_buf_size)) {
        if (fgets(s_line_buf, BUFLEN - 1, s_source)) {
            s_buf_size = strlen(s_line_buf);
            s_line_pos = 0;
            return s_line_buf[s_line_pos++];
        } else {
            s_EOF_flag = 1;
            return EOF;
        }
    } else {
        return s_line_buf[s_line_pos++];
    }
}

static int unget_next_char(void)
{
    if (!s_EOF_flag) {
        s_line_pos--;
    }
}

static TokenType get_token(void)
{
    int token_string_index = 0;
    TokenType current_token;
    StateType state = START;
    int save;

    while (state != DONE) {
        int c = get_next_char();
        save = 1;
        switch (state) {
            case START:
                if (isalpha(c)) {
                    state = INID;
                } else if ((c == ' ') || (c == '\t') || (c == '\n')) {
                    save = 0;
                } else {
                    state = DONE;
                    switch (c) {
                        case EOF:
                            save = 0;
                            current_token = ENDFILE;
                            break;
                        default:
                            current_token = ERROR;
                            break;
                    }
                }
                break;
            case INID:
                if (!isalnum(c)) {
                    unget_next_char();
                    save = 0;
                    state = DONE;
                    current_token = ID;
                }
                break;
            case DONE:
            default:
                state = DONE;
                current_token = ERROR;
                fprintf(s_listing, "Scanner ERROR\n");
                break;
        }

        if (save && token_string_index <= MAXTOKENLEN) {
            s_token_string[token_string_index++] = (char)c;
        }
        if (state == DONE) {
            s_token_string[token_string_index++] = '\0';
            if (current_token == ID) {
                printf("token: %s\n", s_token_string);
            }
        }
    }

    return current_token;
}

int main(int argc, char *argv[])
{
    char *filename = "./test.txt";

    s_source = fopen(filename, "r");
    if (s_source == NULL) {
        fprintf(stderr, "File %s not found\n", filename);
        return 1;
    }
    s_listing = stdout;
    while (1) {
        TokenType token = get_token();
        if (token == ENDFILE)
            break;
    }


    return 0;
}
