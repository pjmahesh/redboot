/*****************************************************************************
* Product: PELICAN crossing example, with QK-nano
* Last Updated for Version: 4.4.00
* Date of the Last Update:  Mar 01, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "bsp.h"
#include "pelican.h"

#include <dos.h>                       /* for _dos_setvect()/_dos_getvect() */
#include <conio.h>                                      /* for inp()/outp() */
#include <stdlib.h>                                          /* for _exit() */

/* Local-scope objects -----------------------------------------------------*/
static uint32_t l_tickCtr;

static void interrupt (*l_dosTmrISR)();
static void interrupt (*l_dosKbdISR)();

#define TMR_VECTOR      0x08
#define KBD_VECTOR      0x09

/*..........................................................................*/
static void interrupt ISR_tmr() {                             /* see NOTE01 */
    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */

    QF_tickISR();                       /* process all time events (timers) */
    Video_printNumAt(20, 17, VIDEO_FGND_YELLOW, ++l_tickCtr);

    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
static void interrupt ISR_kbd() {
    uint8_t key;
    uint8_t kcr;

    QK_ISR_ENTRY();                       /* inform QK-nano about ISR entry */

    key = inp(0x60);              /* key scan code from 8042 kbd controller */
    kcr = inp(0x61);                       /* get keyboard control register */
    outp(0x61, (uint8_t)(kcr | 0x80));       /* toggle acknowledge bit high */
    outp(0x61, kcr);                          /* toggle acknowledge bit low */

    if (key == 129) {                                       /* ESC pressed? */
        QActive_postISR((QActive *)&AO_Ped, TERMINATE_SIG, 0);
    }

    QK_ISR_EXIT();                         /* inform QK-nano about ISR exit */
}
/*..........................................................................*/
void BSP_init(void) {
    uint8_t n;

    Video_clearScreen(VIDEO_BGND_BLACK);
    Video_clearRect( 0,  0, 80,  7, VIDEO_BGND_LIGHT_GRAY);
    Video_clearRect( 0, 11, 41, 12, VIDEO_BGND_LIGHT_GRAY);
    Video_clearRect( 0,  7, 41, 11, VIDEO_BGND_RED);
    Video_clearRect( 0, 12, 41, 23, VIDEO_BGND_BLUE);
    Video_clearRect( 0, 23, 80, 24, VIDEO_BGND_LIGHT_GRAY);

    n = VIDEO_FGND_BLUE;
    Video_printStrAt(10, 0, n, "  __");
    Video_printStrAt(10, 1, n, " /  |      _   _ -|-     _ _");
    Video_printStrAt(10, 2, n, " \\__| | |  _\\ | \\ | | | | \\ \\");
    Video_printStrAt(10, 3, n, "    | \\_/ |_| | | | \\_| | | |");
    Video_printStrAt(10, 4, n, "    |");
    n = VIDEO_FGND_RED;
    Video_printStrAt(43, 0, n, "    _       __ ");
    Video_printStrAt(43, 1, n, "|  /_\\     |  \\  TM");
    Video_printStrAt(43, 2, n, "|  \\_   _  |__/ _");
    Video_printStrAt(43, 3, n, "|       _\\ |   |_");
    Video_printStrAt(43, 4, n, "|___   |_| |    _|");
    Video_printStrAt(10, 5, VIDEO_FGND_BLUE,
                     "_____________________________________________________");
    Video_printStrAt(10, 6, VIDEO_FGND_RED,
                     "i n n o v a t i n g   e m b e d d e d   s y s t e m s");
    Video_printStrAt(10,  8, VIDEO_FGND_WHITE,
                    "PELICAN Crossing Example");
    Video_printStrAt(10,  9, VIDEO_FGND_WHITE, "QP-nano");
    Video_printStrAt(20,  9, VIDEO_FGND_YELLOW, QP_getVersion());

    Video_printStrAt(4, 11, VIDEO_FGND_BLUE,
                     "Active Object   State");
    Video_printStrAt(4, 13, VIDEO_FGND_WHITE, "PELICAN");
    Video_printStrAt(4, 14, VIDEO_FGND_WHITE, "Pedestrian");

    Video_printStrAt(4, 17, VIDEO_FGND_WHITE, "Tick #");

    n = VIDEO_FGND_WHITE | VIDEO_BGND_BLACK;
    Video_printStrAt(43,  7, n, "      �������Ŀ");
    Video_printStrAt(43,  8, n, "      �       �");
    Video_printStrAt(43,  9, n, "      �       �");
    Video_printStrAt(43, 10, n, "      ���������");
    Video_printStrAt(43, 11, n, "��������������������������������");
    Video_printStrAt(43, 12, n, "  ^   ���������");
    Video_printStrAt(43, 13, n, "  |   ���������");
    Video_printStrAt(43, 14, n, " PEDS ���������  <- CARS ->");
    Video_printStrAt(43, 15, n, "  |   ���������");
    Video_printStrAt(43, 16, n, "  v   ���������");
    Video_printStrAt(43, 17, n, "��������������������������������");
    Video_printStrAt(43, 18, n, "                �������������Ŀ");
    Video_printStrAt(43, 19, n, "                � �Ŀ �Ŀ �Ŀ �");
    Video_printStrAt(43, 20, n, "                � � � � � � � �");
    Video_printStrAt(43, 21, n, "                � ��� ��� ��� �");
    Video_printStrAt(43, 22, n, "                ���������������");

    Video_printStrAt(4, 23, VIDEO_FGND_BLUE,
         "* Copyright (c) Quantum Leaps, LLC * www.quantum-leaps.com *");
    Video_printStrAt(28, 24, VIDEO_FGND_LIGHT_RED,
         "<< Press Esc to quit >>");
}
/*..........................................................................*/
void QF_onStartup(void) {
                                      /* save the origingal DOS vectors ... */
    l_dosTmrISR = _dos_getvect(TMR_VECTOR);
    l_dosKbdISR = _dos_getvect(KBD_VECTOR);

    QF_INT_DISABLE();
    _dos_setvect(TMR_VECTOR, &ISR_tmr);
    _dos_setvect(KBD_VECTOR, &ISR_kbd);
    QF_INT_ENABLE();
}
/*..........................................................................*/
void QF_stop(void) {
                                    /* restore the original DOS vectors ... */
    QF_INT_DISABLE();
    _dos_setvect(TMR_VECTOR, l_dosTmrISR);
    _dos_setvect(KBD_VECTOR, l_dosKbdISR);
    QF_INT_ENABLE();

    _exit(0);                                                /* exit to DOS */
}
/*..........................................................................*/
void QK_onIdle(void) {
}
/*..........................................................................*/
void BSP_signalCars(enum BSP_CarsSignal sig) {
    switch (sig) {
        case CARS_OFF:
            Video_printStrAt(62, 20, VIDEO_FGND_BLACK, "�");
            Video_printStrAt(66, 20, VIDEO_FGND_BLACK, "�");
            Video_printStrAt(70, 20, VIDEO_FGND_BLACK, "�");
            break;
        case CARS_RED:
            Video_printStrAt(62, 20, VIDEO_FGND_RED,   "�");
            Video_printStrAt(66, 20, VIDEO_FGND_BLACK, "�");
            Video_printStrAt(70, 20, VIDEO_FGND_BLACK, "�");
            break;
        case CARS_YELLOW:
            Video_printStrAt(62, 20, VIDEO_FGND_BLACK, "�");
            Video_printStrAt(66, 20, VIDEO_FGND_YELLOW,"�");
            Video_printStrAt(70, 20, VIDEO_FGND_BLACK, "�");
            break;
        case CARS_GREEN:
            Video_printStrAt(62, 20, VIDEO_FGND_BLACK, "�");
            Video_printStrAt(66, 20, VIDEO_FGND_BLACK, "�");
            Video_printStrAt(70, 20, VIDEO_FGND_GREEN, "�");
            break;
    }
}
/*..........................................................................*/
void BSP_signalPeds(enum BSP_PedsSignal sig) {
    switch (sig) {
        case PEDS_DONT_WALK:
            Video_printStrAt(51,  8, VIDEO_FGND_RED,  "DON\'T");
            Video_printStrAt(51,  9, VIDEO_FGND_RED,  "WALK");
            break;
        case PEDS_BLANK:
            Video_printStrAt(51,  8, VIDEO_FGND_RED,  "     ");
            Video_printStrAt(51,  9, VIDEO_FGND_RED,  "    ");
            break;
        case PEDS_WALK:
            Video_printStrAt(51,  8, VIDEO_FGND_GREEN, "WALK ");
            Video_printStrAt(51,  9, VIDEO_FGND_GREEN, "    ");
            break;
    }
}
/*..........................................................................*/
void BSP_showState(uint8_t prio, char const *state) {
    Video_printStrAt(20, 12 + prio, VIDEO_FGND_YELLOW, "                ");
    Video_printStrAt(20, 12 + prio, VIDEO_FGND_YELLOW, state);
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    Video_clearRect(0, 24, 80, 25, VIDEO_BGND_RED);
    Video_printStrAt( 0, 24, VIDEO_FGND_WHITE, "ASSERTION FAILED in file:");
    Video_printStrAt(26, 24, VIDEO_FGND_YELLOW, file);
    Video_printStrAt(57, 24, VIDEO_FGND_WHITE, "line:");
    Video_printNumAt(62, 24, VIDEO_FGND_YELLOW, line);

    QF_stop();                                       /* stop and cleanup QF */
}
