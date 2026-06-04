#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

static int parse_osc11_response(const char *buf) {
    const char *p = strstr(buf, "rgb:");
    if (!p) return -1;

    unsigned int r, g, b;
    if (sscanf(p, "rgb:%x/%x/%x", &r, &g, &b) != 3) return -1;

    int hex_len = 0;
    for (const char *q = p + 4; *q && *q != '/'; q++) hex_len++;
    if (hex_len == 0 || hex_len > 4) return -1;

    double scale = (double)((1u << (hex_len * 4)) - 1);
    double rn = r / scale, gn = g / scale, bn = b / scale;
    double luma = 0.299 * rn + 0.587 * gn + 0.114 * bn;
    return luma < 0.5 ? 1 : 0;
}

static int detect_via_colorfgbg(void) {
    const char *cfb = getenv("COLORFGBG");
    if (!cfb) return -1;

    const char *last = strrchr(cfb, ';');
    if (!last) return -1;

    int bg = atoi(last + 1);
    // Palette indices 7 and 15 are light; 0-6 and 8-14 are dark.
    if (bg == 7 || bg == 15) return 0;
    return 1;
}

int term_is_dark(void) {
    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
        return detect_via_colorfgbg();
    }

    struct termios old_t, new_t;
    if (tcgetattr(STDIN_FILENO, &old_t) != 0) {
        return detect_via_colorfgbg();
    }
    new_t = old_t;
    new_t.c_lflag &= ~(ICANON | ECHO);
    new_t.c_cc[VMIN] = 0;
    new_t.c_cc[VTIME] = 5;  // 500 ms per read

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_t) != 0) {
        return detect_via_colorfgbg();
    }

    tcflush(STDIN_FILENO, TCIFLUSH);

    int result = -1;
    const char query[] = "\033]11;?\007";
    if (write(STDOUT_FILENO, query, sizeof(query) - 1) > 0) {
        char buf[64];
        size_t n = 0;
        ssize_t r;
        while (n < sizeof(buf) - 1 &&
               (r = read(STDIN_FILENO, buf + n, sizeof(buf) - 1 - n)) > 0) {
            n += r;
            if (buf[n - 1] == '\007') break;
            if (n >= 2 && buf[n - 2] == '\033' && buf[n - 1] == '\\') break;
        }
        buf[n] = '\0';
        result = parse_osc11_response(buf);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);

    if (result == -1) result = detect_via_colorfgbg();
    return result;
}
