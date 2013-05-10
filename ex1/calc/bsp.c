#include "bsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISP_WIDTH  9

static char l_display[DISP_WIDTH + 1];
static int l_len;   /* number of display digits */

/* clear */
void BSP_clear(void) {
    memset(l_display, ' ', DISP_WIDTH-1);
    l_display[DISP_WIDTH - 1] = '0';
    l_display[DISP_WIDTH] = '\0';
    l_len = 0;
}

/* insert */
void BSP_insert(int keyid) {
    if (l_len == 0) {
        l_display[DISP_WIDTH - 1] = (char)keyid;
        ++l_len;
    }
    else if (l_len < (DISP_WIDTH - 1)) {
        memmove(&l_display[0], &l_display[1], DISP_WIDTH-1);
        l_display[DISP_WIDTH - 1] = (char)keyid;
        ++l_len;
    }
}

/* negate */
void BSP_negate(void) {
    BSP_clear();
    l_display[DISP_WIDTH - 2] = '-';
}

/* display */
void BSP_display(void) {
    printf("\n[%s]", l_display);
}

/* exit */
void BSP_exit(void) {
    printf("\n byebye \n");
    fflush(stdout);
    _exit(0);
}

/* get value */
double BSP_get_value(void) {
    return strtod(l_display, (char **)0);
}

/* eval */
int BSP_eval(double operand1, int oper, double operand2) {
    int ok = 1;
    double result;
    switch (oper) {
        case KEY_PLUS: {
            result = operand1 + operand2;
            break;
        }
        case KEY_MINUS: {
            result = operand1 - operand2;
            break;
        }
        case KEY_DIVIDE: {
            if ((operand2 < -1e-30) || (1e-30 < operand2))  {
                result = operand1 / operand2;
            }
            else {
                strcpy(l_display, " Error 0");
                ok = 0;
            }
            break;
        }
    }
    if (ok) {
        if ((-0.000001 < result) && (result < 0.000001)) {
            result = 0.0;
        }
        if ((-99999999.0 < result) && (result < 99999999.0)) {
            sprintf(l_display, "%9.6g", result);
        }
        else {
            strcpy(l_display, " Error 1");          /* error: out of range */
            ok = 0;
        }
    }
    return ok;
}

/* message */
void BSP_message(char const *msg) {
    printf(msg);
}

/* assert */
void Q_onAssert(char const *const file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
    _exit(-1);
}
