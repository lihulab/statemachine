#include "qep_port.h"
#include "bsp.h"
#include "calc.h"

#include <stdio.h>
#include <conio.h>
#include <windows.h>

/*..........................................................................*/
int main() {
    Calc_ctor();            /* explicitly instantiate the calculator object */

    printf("Calculator example, QEP version: %s\n"
           "Press '0' .. '9'     to enter a digit\n"
           "Press '.'            to enter the decimal point\n"
           "Press '+' or '#'     to add\n"
           "Press '-'            to subtract or negate a number\n"
           "Press '*'            to multiply\n"
           "Press '/'            to divide\n"
           "Press '=' or <Enter> to get the result\n"
           "Press 'c' or 'C'     to Cancel\n"
           "Press 'e' or 'E'     to Cancel Entry\n"
           "Press <Esc>          to quit.\n\n",
           QEP_getVersion());

    QHsm_init(the_calc, (QEvt *)0);         /* trigger initial transition */

    for (;;) {                                                /* event loop */
        CalcEvt e;                                      /* Calculator event */

        BSP_display();                                  /* show the display */

        printf(": ");
        fflush(stdout);
        e.key_code = (uint8_t)_getche();            /* get a char with echo */

        switch (e.key_code) {
            case 'c':                         /* intentionally fall through */
            case 'C': {
                ((QEvt *)&e)->sig = C_SIG;
                break;
            }
            case 'e':                         /* intentionally fall through */
            case 'E': {
                ((QEvt *)&e)->sig = CE_SIG;
                break;
            }
            case '0': {
                ((QEvt *)&e)->sig = DIGIT_0_SIG;
                break;
            }
            case '1':                         /* intentionally fall through */
            case '2':                         /* intentionally fall through */
            case '3':                         /* intentionally fall through */
            case '4':                         /* intentionally fall through */
            case '5':                         /* intentionally fall through */
            case '6':                         /* intentionally fall through */
            case '7':                         /* intentionally fall through */
            case '8':                         /* intentionally fall through */
            case '9': {
                ((QEvt *)&e)->sig = DIGIT_1_9_SIG;
                break;
            }
            case '.': {
                ((QEvt *)&e)->sig = POINT_SIG;
                break;
            }
            case '+':                         /* intentionally fall through */
            case '-':                         /* intentionally fall through */
            case '*':                         /* intentionally fall through */
            case '/': {
                ((QEvt *)&e)->sig = OPER_SIG;
                break;
            }
            case '#': {                                  /* alternative '+' */
                ((QEvt *)&e)->sig = OPER_SIG;
                e.key_code = '+';
                break;
            }
            case '=':                         /* intentionally fall through */
            case '\r': {                                       /* Enter key */
                ((QEvt *)&e)->sig = EQUALS_SIG;
                break;
            }
            case '\33': {                                        /* ESC key */
                ((QEvt *)&e)->sig = OFF_SIG;
                break;
            }
            default: {
                ((QEvt *)&e)->sig = 0;                   /* invalid event */
                break;
            }
        }

        if (((QEvt *)&e)->sig != 0) {           /* valid event generated? */
            QHsm_dispatch(the_calc, (QEvt *)&e);        /* dispatch event */
        }
    }
    return 0;
}
