#ifndef __CALC_H
#define __CALC_H

enum CalcSignals {
    C_SIG = Q_USER_SIG,
    CE_SIG,
    DIGIT_0_SIG,
    DIGIT_1_9_SIG,
    POINT_SIG,
    OPER_SIG,
    EQUALS_SIG,
    OFF_SIG
};

typedef struct CalcEvtTag {
    QEvt super;                                    /* derives from QEvt */
    uint8_t key_code;                                    /* code of the key */
} CalcEvt;

void Calc_ctor(void);                                /* the calculator ctor */

extern QHsm * const the_calc;         /* "opaque" pointer to calculator HSM */

#endif
