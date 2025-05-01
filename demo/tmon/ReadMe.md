= RISC-V Test Loader/Monitor

== File Structure

```
  - tmon.c,h       - Test monitor API

  - mmon.c,h       - M-mode ecall handlers
  - smon.c,h       - S-mode ecall handlers
  - vmon.c.h       - VS-mode ecall handlers

  - mtwr0.S        - M-mode trap wrapper, direct mode
  - stwr0.S        - S-mode trap wrapper, direct mode
  - vtwr0.S        - S-mode trap wrapper, direct mode

  - mtwr1.S        - M-mode trap wrapper, vectored mode
  - stwr1.S        - S-mode trap wrapper, vectored mode
  - vtwr1.S        - VS-mode trap wrapper, vectored mode

  - mtwr3.S        - M-mode trap wrapper, nested vectored mode
  - stwr3.S        - S-mode trap wrapper, nested vectored mode
  - vtwr3.S        - VS-mode trap wrapper, nested vectored mode

  - mtvec.c,h       - M-mode trap vector table, all modes
  - stvec.c,h       - S-mode trap vector table, all modes
  - vtvec.c,h       - VS-mode trap vector table, all modes
```

== Trap Vector Table {m|s|v}tvec.c,h

```
    index   mode=0      mode=1              mode=3
    -----------------------------------------------------------------
    0       exc0        exc0                exc0
    1       exc1        exc1                exc1
    ..      
    31      exc31       exc31               exc31
    32      maj0        exc   -> vect0      exc   -> vect0
    33      maj1        maj1  -> vect1      ext1  -> vect1
    ..
    63      maj31       maj31 -> vect31     ext31 -> vect31
    64      ext0        ext0                ext32 -> chained
    65      ext1        ext1                ext33 -> chained
    ..
    95      ext31       ext31               ext63 -> chained
    -----------------------------------------------------------------
exc traps   32          32                  32
maj irqs    31          31                  -
ext irqs    32          32                  63
    {x}tvec &trap_wrap  &IVT[32]            &IVT[32]
```

== Trap Vector API {m|s}tvec.c,h
```
    {m|s}_trap_mode(mode)

    {m|s}_exc_setvec(eid, handler)
    {m|s}_maj_setvec(iid, handler)
    {m|s}_ext_setvec(eiid, handler)

    {m|s}_exc_delegate(eid, delegate)
    {m|s}_int_delegate(iid, delegate)

    {m|s}_int_enable(iid, enable)
```

== Mode 0
```
    mtvec -> mtwr0.S::_m_trap_wrapper
    stvec -> stwr0.S::_s_trap_wrapper
    vtvec -> vtwr0.S::_v_trap_wrapper

    M-mode SW vector table, default handlers, API -> mtvec.c
    S-mode SW vector table, default handlers, API -> stvec.c
    V-mode SW vector table, default handlers, API -> vtvec.c

    M-mode ecall handler -> mmon.c
    S-mode ecall handler -> smon.c
    V-mode ecall handler -> vmon.c
```

== Mode 1
```
    mtvec -> mtwr1.S::_m_trap_vector

    M-mode exc wrapper  -> mtwr1.S
    M-mode maj wrappers -> mtwr1.S
    M-mode ext wrapper  -> mter1.S

    M-mode SW vector table, default handlers, API -> mtvec.c
```

== Mode 3
```
    mtvec -> mtvec.c::&m_trap_vector[32]

    M-mode exc wrapper     -> mtwr3.S
    M-mode ext wrappers    -> mtwr3.S
    M-mode chained wrapper -> mtwr3.S


```