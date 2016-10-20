#include "km.h"

/* km.c -- keyboard modifiers state
 *
 * Implements objects which track the state of keyboard modifier -- is it active or not.
 *
 * Modifiers could be: pressed (as with shift), toggled (as with caps lock), and
 * latched (act as shift if any key gets released while the latch key is down,
 * or as lock-for-next-key-only otherwise).
 *
 * */

void KM_init(KM *km) {
    ZeroMemory(km, sizeof(KM));
}

void KM_nonmod_event(KM *km, bool down, SC sc) {
    if (km->latch) {
        if (!down) {
            if (km->latch_released) {
                km->latch = 0;
                km->latch_releases = 1;
                goto l_release;
            } else if (km->latch == sc) {
                km->latch_released = true;
                goto l_release;
            } else {
                km->latch_releases += 1;
            }
        }
    }
    return;
l_release:
    printf("{l_release}");
    if (!(km->in_effect = km->shifts_count || !km->latch_releases)) {
        VK vk = km->latch_faked;
        if (vk) {
            km->latch_faked = 0;
            keybd_event(vk, sc, KEYEVENTF_KEYUP, 0);
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
                printf("{/:%d}", km->in_effect);
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
            printf("{set latch %03x}", sc);
            km->latch = sc;
            km->latch_released = false;
            km->latch_releases = 0;
            VK vk = km->latch_faked;
            if (vk) {
                keybd_event(vk, sc, 0, 0);
            }
        }
    } else {
        km->latch_released = true;
        printf("{released:%d:%d}", km->latch, km->latch_releases);
        if (km->latch_releases) {
            km->latch = 0;
            VK vk = km->latch_faked;
            if (vk) {
                km->latch_faked = 0;
                keybd_event(vk, sc, KEYEVENTF_KEYUP, 0);
            }
        }
    }
    km->in_effect = km->latch || km->shifts_count;
    printf("{latch.in_effect=%d:%d}", km->in_effect, km->latch_releases);
}

void KM_lock_event(KM *km, bool down, SC sc) {
    if (down && km->lock_sc != sc) {
        bool lock1 = !km->locked;
        km->locked = lock1;
        km->in_effect = lock1 || km->latch || km->shifts_count;
    }
}
