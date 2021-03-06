/*****************************************************************************
* Product:  QK/C, port to 80x86, QK port, Open Watcom compiler
* Last Updated for Version: 5.2.0
* Date of the Last Update:  Nov 30, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef qk_port_h
#define qk_port_h
                                          /* QK-specific ISR entry and exit */
#define QK_ISR_ENTRY() do { \
    ++QK_intNest_; \
    QF_QS_ISR_ENTRY(QK_intNest_, QK_currPrio_); \
    _enable(); \
} while (0)

#define QK_ISR_EXIT() do { \
    _disable(); \
    outp(0x20, 0x20); \
    QF_QS_ISR_EXIT(QK_intNest_, QK_currPrio_); \
    --QK_intNest_; \
    if (QK_intNest_ == (uint8_t)0) { \
        uint8_t p = QK_schedPrio_(); \
        if (p != (uint8_t)0) { \
            QK_schedExt_(p); \
        } \
    } \
} while (0)


/* demonstration of advanced QK features: TLS and extended context switch   */
typedef struct Lib1_contextTag {         /* an example of a library context */
    double  x;
} Lib1_context;
typedef struct Lib2_contextTag {         /* an example of a library context */
    double  y;
} Lib2_context;
typedef union FPU_contextTag {
    uint32_t align;
    uint8_t  x87[108];               /* the x87 FPU context takes 108-bytes */
} FPU_context;

typedef struct ThreadContextTag {
    Lib1_context lib1;                                  /* library1 context */
    Lib2_context lib2;                                  /* library2 context */
    FPU_context  fpu;                                    /* the FPU context */
} ThreadContext;

enum QKTaskFlags {
    QK_LIB1_THREAD = 0x01,
    QK_LIB2_THREAD = 0x02,
    QK_FPU_THREAD  = 0x04
};

                                                 /* QK thread-local storage */
#define QK_TLS(act_) \
    impure_ptr1 = &((ThreadContext *)(act_)->thread)->lib1; \
    impure_ptr2 = &((ThreadContext *)(act_)->thread)->lib2

extern Lib1_context * volatile impure_ptr1;
extern Lib2_context * volatile impure_ptr2;

                                  /* QK extended context (FPU) save/restore */
#define QK_EXT_SAVE(act_)    \
    if (((act_)->osObject & QK_FPU_THREAD) != 0) \
        FPU_save(&((ThreadContext *)(act_)->thread)->fpu)

#define QK_EXT_RESTORE(act_) \
    if (((act_)->osObject & QK_FPU_THREAD) != 0) \
        FPU_restore(&((ThreadContext *)(act_)->thread)->fpu)

__declspec(__cdecl) void FPU_save(FPU_context *fpu); /* defined in assembly */
__declspec(__cdecl) void FPU_restore(FPU_context *fpu);      /* in assembly */

#include "qk.h"                 /* QK platform-independent public interface */
#include "qf.h"                 /* QF platform-independent public interface */

#endif                                                         /* qk_port_h */

/*****************************************************************************
* NOTE01:
* The QK port on x86 is not a pure play, but rather, QK is based in this
* case on DOS.
*/
