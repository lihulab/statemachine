#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum BombSignals
{   /* signals of bomb FSM */
    UP_SIG,
    DOWN_SIG,
    ARM_SIG,
    TICK_SIG
};

enum BombStates 
{   /* states of bomb FSM */
    SETTING_STATE,
    TIMING_STATE
};

typedef struct EventTag
{
    uint8_t sig;
} Event;

typedef struct TickEvtTag
{
    Event super;
    uint8_t fine_time;
} TickEvt;

typedef struct Bomb1Tag
{
    /* bomb FSM */
    uint8_t state;
    uint8_t timeout;
    uint8_t code;
    uint8_t defuse;
} Bomb1;

void Bomb1_ctor(Bomb1 *me, uint8_t defuse); /* constrcutor */
void Bomb1_init(Bomb1 *me);
void Bomb1_dispatch(Bomb1 *me, Event const *me);

/* macro for taking a state trasition */
#define TRAN(__target) (me->state = (uint8_t)(__target))
/* init timeout value*/
#define INIT_TIMEOUT    10

void BSP_display(uint8_t data)
{
    printf("%d\n", data);
}

void BSP_boom(void)
{
    printf("boom\n");
    _exit(0);
}

void Bomb1_ctor(Bomb1 *me, uint8_t defuse)
{
    me->defuse = defuse;
}

void Bomb1_init(Bomb1 *me)
{
    me->timeout = INIT_TIMEOUT;
    TRAN(SETTING_STATE);
}

void Bomb1_dispatch(Bomb1 *me, Event const *me)
{
    switch (me->state)
    {
        case SETTING_STATE:
        {
            switch (e->sig)
            {
                case UP_SIG:
                {
                    if (me->timeout < 60)
                    {
                        ++me->timeout;
                        BSP_display(me->timeout);
                    }
                    break;
                }
                case DOWN_SIG:
                {
                    if (me->timeout > 1)
                    {
                        --me->timeout;
                        BSP_display(me->timeout);
                    }
                    break;
                }
                case ARM_SIG:
                {
                    me->code = 0;
                    TRAN(TIMING_STATE);
                    break;
                }
                            
            }
            break;
        }
        case TIMING_STATE:
        {
            switch (e->sig)
            {
                case UP_SIG:
                {
                    me->code <<= 1;
                    me->code |= 1;
                    break;
                }
                case DOWN_SIG:
                {
                    me->code <<= 1;
                    break;
                }
                case ARM_SIG:
                {
                    if (me->code == me->defuse)
                        TRAN(SETTING_STATE);
                    break;
                }
                case TICK_SIG:
                {
                    if (((TickEvt const *)e)->fine_time == 0)
                    {
                        --me->timeout;
                        BSP_display(me->timeout);
                        if (me->timeout == 0)
                            BSP_boom();
                    }
                    break;
                }
            }
            break;
        }
    }
}



/********************
 */

static Bomb1 bomb;

int main(void)
{
    Bomb1_ctor(&bomb, 0x0d);
    Bomb1_init(&bomb);

    while(1)
    {
        
    }

}
