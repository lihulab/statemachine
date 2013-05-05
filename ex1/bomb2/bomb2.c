#include "qep_port.h"
#include "bsp.h"

enum BombSignals {
    UP_SIG = Q_USER_SIG,
    DOWN_SIG,
    ARM_SIG,
    TICK_SIG
};

typedef struct TickEvtTag {
    QEvt super;
    uint8_t fine_time;
} TickEvt;

typedef struct Bomb4Tag {
    QFsm super;
    uint8_t timeout;
    uint8_t code;
    uint8_t defuse;
} Bomb4;

void Bomb4_ctor(Bomb4 *me, uint8_t defuse);

static QState Bomb4_initial(Bomb4 *me, QEvt const *e);
static QState Bomb4_setting(Bomb4 *me, QEvt const *e);
static QState Bomb4_timing(Bomb4 *me, QEvt const *e);

/* the init timeout */
#define INIT_TIMEOUT 10

void Bomb4_ctor(Bomb4 *me, uint8_t defuse){
    QFsm_ctor(&me->super, (QStateHandler)&Bomb4_initial);
    me->defuse = defuse;
}

QState Bomb4_initial(Bomb4 *me, QEvt const *e){
    (void)me;
    me->timeout = INIT_TIMEOUT;
    return Q_TRAN(&Bomb4_setting);
}

QState Bomb4_setting(Bomb4 *me, QEvt const *e){
    switch(e->sig){
        case UP_SIG:{
            if (me->timeout < 60){
                ++me->timeout;
                BSP_display(me->timeout);
            }
            return Q_HANDLED();
        }
        case DOWN_SIG:{
            if (me->timeout > 1){
                --me->timeout;
                BSP_display(me->timeout);
            }
            return Q_HANDLED();
        }
        case ARM_SIG:{
            return Q_TRAN(&Bomb4_timing);
        }
    }
    return Q_IGNORED();
}

QState Bomb4_timing(Bomb4 *me, QEvt const *e){
    switch(e->sig) {
        case Q_ENTRY_SIG: {
            me->code = 0;
            return Q_HANDLED();
        }
        case UP_SIG: {
            me->code <<= 1;
            me->code |= 1;
            return Q_HANDLED();
        }
        case DOWN_SIG: {
            me->code <<= 1;
            return Q_HANDLED();
        }
        case ARM_SIG: {
            if (me->code == me->defuse) {
                return Q_TRAN(&Bomb4_setting);
            }
            return Q_HANDLED();
        }
        case TICK_SIG: {
            if (((TickEvt const *)e)->fine_time == 0) {
                --me->timeout;
                BSP_display(me->timeout);
                if (me->timeout == 0)
                    BSP_boom();
            }
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>

static Bomb4 l_bomb;
int main() {

    Bomb4_ctor(&l_bomb, 0x0D);       /* the secret defuse code, 1101 binary */

    printf("Time Bomb (QEP QFsm class)\n"
           "Press 'u'   for UP   event\n"
           "Press 'd'   for DOWN event\n"
           "Press 'a'   for ARM  event\n"
           "Press <Esc> to quit.\n");

    QFsm_init((QFsm *)&l_bomb, (QEvt *)0); /* take the initial transition */

    for (;;) {                                                /* event loop */
        static TickEvt tick_evt = { {TICK_SIG, 0, 0}, 0 };

        Sleep(100);                                         /* 100 ms delay */

        if (++tick_evt.fine_time == 10) {
            tick_evt.fine_time = 0;
        }
        /*printf("T(%1d)%c", tick_evt.fine_time,
                             (tick_evt.fine_time == 0) ? '\n' : ' ');
        */
        QFsm_dispatch((QFsm *)&l_bomb, (QEvt *)&tick_evt);

        if (kbhit()) {
            static QEvt const up_evt   = { UP_SIG,   0, 0 };
            static QEvt const down_evt = { DOWN_SIG, 0, 0 };
            static QEvt const arm_evt  = { ARM_SIG,  0, 0 };
            QEvt const *e = (QEvt *)0;

            switch (getch()) {
                case 'u': {                                     /* UP event */
                    printf("\nUP  : ");
                    e = &up_evt;                   /* generate the UP event */
                    break;
                }
                case 'd': {                                   /* DOWN event */
                    printf("\nDOWN: ");
                    e = &down_evt;               /* generate the DOWN event */
                    break;
                }
                case 'a': {                                    /* ARM event */
                    printf("\nARM : ");
                    e = &arm_evt;                 /* generate the ARM event */
                    break;
                }
                case '\33': {                                  /* <Esc> key */
                    printf("\nESC : Bye! Bye!");
                    fflush(stdout);
                    _exit(0);
                    break;
                }
            }
            if (e != (QEvt *)0) {            /* keyboard event available? */
                QFsm_dispatch((QFsm *)&l_bomb, e);    /* dispatch the event */
            }
        }
    }

    return 0;
}
