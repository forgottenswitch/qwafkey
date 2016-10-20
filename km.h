#ifndef KM_H_INCLUDED
#define KM_H_INCLUDED

#include "stdafx.h"

/* km.h -- keyboard modifiers state */

#define KM_MAXSHIFTS 4

typedef struct {
    bool in_effect;
    bool locked;
    SC lock_sc;
    SC latch;
    VK latch_faked;
    bool latch_released;
    UCHAR latch_releases;
    SC shifts[KM_MAXSHIFTS];
    UCHAR shifts_count;
} KM;

void KM_init(KM *km);

#define parms KM *km, bool down, SC sc
void KM_nonmod_event(parms);
void KM_shift_event(parms);
void KM_latch_event(parms);
void KM_lock_event(parms);
#undef parms

#endif // KM_H_INCLUDED

