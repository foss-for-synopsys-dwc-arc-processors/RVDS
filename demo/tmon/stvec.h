/// @file       stvec.h
/// @brief      RISC-V Test Monitor - S-mode Trap API and Vector Tables 


/* Predefined M-mode Trap Wrappers */

extern void _s_trap_wrapper(void );        // direct mode exc/maj trap wrapper (stwr0.S)

/* Predefined Software Trap Vector Table */

extern void* s_trap_vector[];

/* Trap API */

extern int s_trap_mode(int mode);
// extern int s_exc_setvec(int eid, void *handler);
// extern int s_maj_setvec(int iid, void *handler);
// extern int s_ext_setvec(int eiid, void *handler);
// extern int s_exc_delegate(int eid, int delegate);
// extern int s_maj_delegate(int iid, int delegate);
// extern int s_maj_priority(int iid, int priority);
// extern int s_maj_enable(int iid, int enable);
// extern int s_ext_enable(int eiid, int enable);

// extern int s_ext_delivery(int enable);
// extern int s_ext_threshold(int threshold);

extern int s_all_enable(int enable);




