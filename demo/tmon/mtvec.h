/// @file       mtvec.h
/// @brief      RISC-V Test Monitor - M-mode Trap API and Vector Tables 


/* Predefined M-mode Trap Wrappers */

extern void _m_trap_wrapper(void );        // direct mode exc/maj trap wrapper (mtwr0.S)

/* Predefined Software Trap Vector Table */

extern void* m_trap_vector[];

/* M-mode Trap API */

extern int m_trap_mode(int mode);
extern int m_exc_setvec(int eid, void *handler);
extern int m_maj_setvec(int iid, void *handler);
extern int m_ext_setvec(int eiid, void *handler);
extern int m_exc_delegate(int eid, int delegate);
extern int m_maj_delegate(int iid, int delegate);
extern int m_maj_priority(int iid, int priority);
extern int m_maj_enable(int iid, int enable);
extern int m_ext_enable(int eiid, int enable);

extern int m_ext_delivery(int enable);
extern int m_ext_threshold(int threshold);

extern int m_all_enable(int enable);

