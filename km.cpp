#include "km.h"

void KM_init(KM *km) {
    ZeroMemory(km, sizeof(KM));
}

void KM_nonmod_event(KM *km, bool down, SC sc) {
    if (km->latch) {
        if (!down) {
            if (!km->latch_released) {
                km->latch_releases += 1;
            } else {
                km->latch = 0;
                km->in_effect = km->shifts_count;
            }
        }
    }
}

void KM_shift_event(KM *km, bool down, SC sc) {
    bool found = false;
    size_t i, shifts_count = km->shifts_count;
    fori (i, 0, shifts_count) {
        if (km->shifts[i] == sc) {
            if (!down) {
                size_t j = i + 1;
                fori (j, 0, shifts_count) {
                    km->shifts[j-1] = km->shifts[j];
                }
                km->in_effect = km->locked || km->latch || (km->shifts_count -= 1);
                break;
            } else {
                found = true;
                km->in_effect = true;
            }
        }
    }
    if (down && !found && km->shifts_count < KM_MAXSHIFTS) {
        km->shifts[km->shifts_count] = sc;
        km->shifts_count += 1;
        km->in_effect = true;
    }
}

void KM_latch_event(KM *km, bool down, SC sc) {
    km->locked = false;
    if (down) {
        if (!km->latch || km->latch_released) {
            km->latch = true;
            km->latch_released = false;
            km->latch_releases = 0;
        }
    } else {
        km->latch_released = true;
        km->latch = !km->latch_releases;
    }
    km->in_effect = km->latch || km->shifts_count;
}

void KM_lock_event(KM *km, bool down, SC sc) {
    if (down && km->lock_sc != sc) {
        bool lock1 = !km->locked;
        km->locked = lock1;
        km->in_effect = lock1 || km->latch || km->shifts_count;
    }
}
