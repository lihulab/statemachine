#include "qep_port.h"
#include "qassert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Q_DEFINE_THIS_FILE

typedef struct UltimateHookTag {
    QHsm super;                                         /* derive from QHsm */
} UltimateHook;

void UltimateHook_ctor(UltimateHook *me);

static QState UltimateHook_initial (UltimateHook *me, QEvt const *e);
static QState UltimateHook_generic (UltimateHook *me, QEvt const *e);
static QState UltimateHook_specific(UltimateHook *me, QEvt const *e);
static QState UltimateHook_final   (UltimateHook *me, QEvt const *e);

enum UltimateHookSignals {
    A_SIG = Q_USER_SIG,
    B_SIG,
    C_SIG,
    D_SIG
};

/*..........................................................................*/
void UltimateHook_ctor(UltimateHook *me) {
    QHsm_ctor(&me->super, (QStateHandler)&UltimateHook_initial);
}
/*..........................................................................*/
QState UltimateHook_initial(UltimateHook *me, QEvt const *e) {
    (void)e;                  /* avoid the "unreferenced parameter" warning */
    printf("top-INIT;");
    return Q_TRAN(&UltimateHook_generic);
}
/*..........................................................................*/
QState UltimateHook_final(UltimateHook *me, QEvt const *e) {
    (void)me;                 /* avoid the "unreferenced parameter" warning */
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("final-ENTRY;");
            printf("\nBye!Bye!\n");
            exit(0);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState UltimateHook_generic(UltimateHook *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("generic-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("generic-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            printf("generic-INIT;");
            return Q_TRAN(&UltimateHook_specific);
        }
        case A_SIG: {
            printf("generic-A;");
            return Q_HANDLED();
        }
        case B_SIG: {
            printf("generic-B;");
            return Q_HANDLED();
        }
        case C_SIG: {
            printf("generic:C(reset);");
            return Q_TRAN(&UltimateHook_generic);
        }
        case D_SIG: {
            return Q_TRAN(&UltimateHook_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState UltimateHook_specific(UltimateHook *me, QEvt const *e) {
    (void)me;                 /* avoid the "unreferenced parameter" warning */
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("specific-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            printf("specific-EXIT;");
            return Q_HANDLED();
        }
        case A_SIG: {
            printf("specific-A;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&UltimateHook_generic);
}
/* test harness ============================================================*/

#include <conio.h>

static UltimateHook l_test;

void main() {
    printf("Ultimate Hook pattern\nQEP version: %s\n"
           "Press 'a'..'c' to inject signals A..C\n"
           "Press 'd' or ESC to inject signal D and quit\n",
           QEP_getVersion());

    UltimateHook_ctor(&l_test);      /* instantiate the UltimateHook object */

                /* trigger the initial transition before dispatching events */
    QHsm_init((QHsm *)&l_test, (QEvt *)0);

    for (;;) {
        QEvt e;
        uint8_t c;

        printf("\n");
        c = (uint8_t)getch();        /* read one character from the console */
        printf("%c: ", c);

        switch (c) {
            case 'a':  e.sig = A_SIG;  break;
            case 'b':  e.sig = B_SIG;  break;
            case 'c':  e.sig = C_SIG;  break;
            case 'd':
            case 0x1B: e.sig = D_SIG;  break;                  /* terminate */
        }
                               /* dispatch the event into the state machine */
        QHsm_dispatch((QHsm *)&l_test, &e);
    }
}
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    _exit(-1);
}
