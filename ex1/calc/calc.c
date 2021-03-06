#include "qep_port.h"
#include "bsp.h"
#include "calc.h"

typedef struct CalcTag {
    QHsm super;
    double operand1;
    uint8_t operator;
} Calc;

static QState Calc_initial(Calc *me, QEvt const *e);
static QState Calc_on(Calc *me, QEvt const *e);
static QState Calc_error(Calc *me, QEvt const *e);
static QState Calc_ready(Calc *me, QEvt const *e);
static QState Calc_result(Calc *me, QEvt const *e);
static QState Calc_begin(Calc *me, QEvt const *e);
static QState Calc_negated1(Calc *me, QEvt const *e);
static QState Calc_operand1 (Calc *me, QEvt const *e);
static QState Calc_zero1    (Calc *me, QEvt const *e);
static QState Calc_int1     (Calc *me, QEvt const *e);
static QState Calc_frac1    (Calc *me, QEvt const *e);
static QState Calc_opEntered(Calc *me, QEvt const *e);
static QState Calc_negated2 (Calc *me, QEvt const *e);
static QState Calc_operand2 (Calc *me, QEvt const *e);
static QState Calc_zero2    (Calc *me, QEvt const *e);
static QState Calc_int2     (Calc *me, QEvt const *e);
static QState Calc_frac2    (Calc *me, QEvt const *e);
static QState Calc_final    (Calc *me, QEvt const *e);

/* local object */
static Calc l_calc;

/* global */
QHsm *const the_calc = (QHsm *)&l_calc;

/* the construction */
void Calc_ctor(void) {
    Calc *me = &l_calc;
    QHsm_ctor(&me->super, (QStateHandler)&Calc_initial);
}

/* HSM definition */
QState Calc_initial(Calc *me, QEvt const *e) {
    (void)e;
    BSP_clear();
    return Q_TRAN(&Calc_on);
}

/* calc on */
QState Calc_on(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Calc_ready);
        }
        case C_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_on);
        }
        case OFF_SIG: {
            return Q_TRAN(&Calc_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}

/* ready */
QState Calc_ready(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&Calc_begin);
        }
        case DIGIT_0_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_zero1);
        }
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_int1);
        }
        case POINT_SIG: {
            BSP_clear();
            BSP_insert((int)'0');
            BSP_insert((int)'.');
            return Q_TRAN(&Calc_frac1);
        }
        case OPER_SIG: {
            me->operand1 = BSP_get_value();
            me->operator = ((CalcEvt const *)e)->key_code;
            return Q_TRAN(&Calc_opEntered);
        }
    }
    return Q_SUPER(&Calc_on);
}

/* result */
QState Calc_result(Calc *me, QEvt const *e) {
    (void)me;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Calc_ready);
}

/* begin */
QState Calc_begin(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case OPER_SIG: {
            if (((CalcEvt const *)e)->key_code == KEY_MINUS) {
                return Q_TRAN(&Calc_negated1);
            }
            break;
        }
    }
    return Q_SUPER(&Calc_ready);
}

QState Calc_negated1(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            BSP_negate();
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case OPER_SIG: {
            if (((CalcEvt const *)e)->key_code == KEY_MINUS) {
                ;   /* just ignore */
                return Q_HANDLED();

            }
            break;
        }
        case CE_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_begin);
        }
        case DIGIT_0_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_zero1);
        }
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_int1);
        }
        case POINT_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_frac1);
        }
    }
    return Q_SUPER(&Calc_on);
}

QState Calc_negated2(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            BSP_negate();
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case OPER_SIG: {
            if (((CalcEvt const *)me)->key_code == KEY_MINUS) {
                ;
                return Q_HANDLED();
            }
            break;
        }
        case CE_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_opEntered);
        }
        case DIGIT_0_SIG: {
            return Q_TRAN(&Calc_zero2);
        }
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_int2);
        }
        case POINT_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_frac2);
        }
    }
    return Q_SUPER(&Calc_on);
}

QState Calc_operand1(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case CE_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_begin);
        }
        case OPER_SIG: {
            me->operand1 = BSP_get_value();
            me->operator = ((CalcEvt const *)e)->key_code;
            return Q_TRAN(&Calc_opEntered);
        }
        case EQUALS_SIG: {
            return Q_TRAN(&Calc_result);
        }
    }
    return Q_SUPER(&Calc_on);
}

QState Calc_zero1(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case DIGIT_0_SIG: {
            ;
            return Q_HANDLED();
        }
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_int1);
        }
        case POINT_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_frac1);
        }
    }
    return Q_SUPER(&Calc_operand1);
}

QState Calc_int1(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case DIGIT_0_SIG:
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_HANDLED();
        }
        case POINT_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_frac1);
        }
    }
    return Q_SUPER(&Calc_operand1);
}

QState Calc_frac1(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case DIGIT_0_SIG:
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_HANDLED();
        }
        case POINT_SIG: {
            ;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Calc_operand1);
}

QState Calc_opEntered(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case OPER_SIG: {
            if (((CalcEvt const *)e)->key_code == KEY_MINUS) {
                BSP_clear();
                return Q_TRAN(&Calc_negated2);
            }
            break;
        }
        case DIGIT_0_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_zero2);
        }
        case DIGIT_1_9_SIG: {
            BSP_clear();
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_int2);
        }
        case POINT_SIG: {
            BSP_clear();
            BSP_insert((int)'0');
            BSP_insert((int)'.');
            return Q_TRAN(&Calc_frac2);
        }
    }
    return Q_SUPER(&Calc_on);
}

QState Calc_operand2(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case CE_SIG: {
            BSP_clear();
            return Q_TRAN(&Calc_opEntered);
        }
        case OPER_SIG: {
            if (BSP_eval(me->operand1, me->operator, BSP_get_value())) {
                me->operand1 = BSP_get_value();
                me->operator = ((CalcEvt const *)e)->key_code;
                return Q_TRAN(&Calc_opEntered);
            }
            else {
                return Q_TRAN(&Calc_error);
            }
        }
        case EQUALS_SIG: {
            if (BSP_eval(me->operand1, me->operator, BSP_get_value())) {
                return Q_TRAN(&Calc_result);
            }
            else {
                return Q_TRAN(&Calc_error);
            }
        }
    }
    return Q_SUPER(&Calc_on);
}

QState Calc_zero2(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case DIGIT_0_SIG: {
            ;
            return Q_HANDLED();
        }
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_int2);
        }
        case POINT_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_frac2);
        }
    }
    return Q_SUPER(&Calc_operand2);
}

QState Calc_int2(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case DIGIT_0_SIG:
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_HANDLED();
        }
        case POINT_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_TRAN(&Calc_frac2);
        }
    }
    return Q_SUPER(&Calc_operand2);
}

QState Calc_frac2(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
        case DIGIT_0_SIG:
        case DIGIT_1_9_SIG: {
            BSP_insert(((CalcEvt const *)e)->key_code);
            return Q_HANDLED();
        }
        case POINT_SIG: {
            ;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Calc_operand2);
}

QState Calc_final(Calc *me, QEvt const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            BSP_exit();
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState Calc_error(Calc *me, QEvt const *e) {
    (void)me;          /* avoid the compiler warning about unused parameter */
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Calc_ready);
}
