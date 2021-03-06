//****************************************************************************
// Product: QP/C++
// Last Updated for Version: 5.2.0
// Date of the Last Update:  Dec 03, 2013
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// Quantum Leaps Web sites: http://www.quantum-leaps.com
//                          http://www.state-machine.com
// e-mail:                  info@quantum-leaps.com
//****************************************************************************
#ifndef qvanilla_h
#define qvanilla_h

/// \file
/// \ingroup qf
/// \brief platform-independent interface to the cooperative "vanilla" kernel.


#include "qequeue.h"       // "Vanilla" kernel uses the native QF event queue
#include "qmpool.h"        // "Vanilla" kernel uses the native QF memory pool
#include "qpset.h"         // "Vanilla" kernel uses the native QF priority set

//****************************************************************************
// QF configuration for the Vanilla kernel

/// \brief This macro defines the type of the event queue used for the
/// active objects.
///
/// \note This is just an example of the macro definition. Typically, you need
/// to define it in the specific QF port file (qf_port.h). In case of the
/// Vanilla kernel, which always depends on the native QF queue, this macro
/// is defined at the level of the platform-independent interface vanilla.h.
#define QF_EQUEUE_TYPE     QEQueue

//****************************************************************************
// interface used only inside QF, but not in applications

#ifdef qf_pkg_h
                                              // native event queue operations
    #define QACTIVE_EQUEUE_WAIT_(me_) \
        Q_ASSERT((me_)->m_eQueue.m_frontEvt != static_cast<QEvt const *>(0))

    #define QACTIVE_EQUEUE_SIGNAL_(me_) \
        (QF_readySet_.insert((me_)->m_prio))

    #define QACTIVE_EQUEUE_ONEMPTY_(me_) \
        (QF_readySet_.remove((me_)->m_prio))

                                            // native QF event pool operations
    #define QF_EPOOL_TYPE_  QMPool
    #define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
        (p_).init((poolSto_), (poolSize_), (evtSize_))
    #define QF_EPOOL_EVENT_SIZE_(p_) \
        static_cast<uint_t>((p_).getBlockSize())
    #define QF_EPOOL_GET_(p_, e_, m_) \
        ((e_) = static_cast<QEvt *>((p_).get((m_))))
    #define QF_EPOOL_PUT_(p_, e_) ((p_).put(e_))

    extern "C" {
        #if (QF_MAX_ACTIVE <= 8)
            extern QP::QPSet8  QF_readySet_;     ///< \brief ready set of AOs
        #else
            extern QP::QPSet64 QF_readySet_;     ///< \brief ready set of AOs
        #endif
        extern uint8_t volatile QF_currPrio_;///<\brief current task/ISR prio.
    }                                                            // extern "C"

#endif                                                       // ifdef qf_pkg_h

#endif                                                           // qvanilla_h
