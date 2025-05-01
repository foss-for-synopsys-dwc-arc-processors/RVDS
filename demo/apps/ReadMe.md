# Demo List

## Trivial Hello World
```
[+] hello - hello world demo
```

## S-mode MPU Demos
```
[+] smpu0 - MPU L1 (S-mode), static physical map demo
[+] smpu1 - MPU L1 (S-mode), static virtual map demo
[+] smpu2 - MPU L2 (HS-mode), static physical map demo
[+] smpu3 - MPU L2 (HS-mode), static virtual map demo
[-] smpu4 - MPU L1/L2 (VS/HS-mode), static physical map
[-] smpu5 - MPU L1 (VS-mode) emulation with L2 (HS-mode), on-demand virtual map demo
```

## TRAP Demos
```
[+] trap0 - privilege transitions, m-mode ecalls
[-] trap1 - privilege transitions, m/s/vs-mode ecalls
[+] trap2 - m-mode interrupt traps, direct mode
[-] trap3 - m-mode interrupt traps, vectored mode
[+/-] trap4 - m-mode interrupt traps, nested vectored mode, 
              needs clarification for maj p-bit behavior
```

# Test Monitor API


## Privilege Mode Control

```
tmon_call(TMON_FID_PRIV, target_priv)
```

## Traps Configuration

```
m_trap_mode(mode)
m_exc_delegate(eid, delegate)
m_maj_priority(iid, prio)
m_maj_enable(iid, enable)
m_maj_delegate(iid, delegate)
m_ext_enable(eiid, enable)
m_ext_delivery(enable)
m_ext_threshold(threshold)
m_all_enable(enable)
```
Optional
```
m_exc_setvec(eid, handler)
m_maj_setvec(iid, handler)
m_ext_setvec(eiid, handler)
```

## Traps Run-Time

```
tmon_call(TMON_FID_CB, cb_descriptor)
tmon_call(TMON_FID_EXPECT, trap_id)
tmon_call(TMON_FID_VERIFY, timeout)
tmon_call(TMON_FID_SSWI, enable)
tmon_call(TMON_FID_MSWI, enable)
```

## Trace Log

```
CASE(num)
TRACE(fmt, ...)
WARNING(fmt, ...)
ERROR(fmt, ...)
```


