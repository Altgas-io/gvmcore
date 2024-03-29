#include "system.h"
#include <stdarg.h>
#include <stdexcept>

#ifdef LINOS
#include <sys/types.h>
#endif

#if 0
// ������ ����� ������� ������������ ������ ��� ��������� �������������
// �� ������ ������� ��������� PPDL-Debugger
tDisAsmArea* ProcLoadElf(char* aName)
{
  try
  {
    return MainDevice->LoadElf(aName);
  }
  catch(.../*exception &exc*/) // output exception
  {
    return 0;
  };
}

tDProcCfg* GetProcCfg()
{
  return (tDProcCfg*)MainDevice->GetCfg();
}

// get memory
int ProcGetMem(void *mem,int busnum,int addr,int size)
{
  return MainDevice->GetMem(mem,busnum,addr,size);
}

int ProcGetCellSize(int busnum,int addr)
{
  return MainDevice->GetCellSize(busnum,addr);
}

// clear all memory
void ProcClearMem()
{
  MainDevice->ClearMem();
}

// set memory
int ProcSetMem(void *mem,int busnum,int addr,int size)
{
  return MainDevice->SetMem(mem,busnum,addr,size);
}

// get size of the memory cells
int ProcGetMemCellSize(int busnum,int addr)
{
  return MainDevice->GetCellSize(busnum,addr);
}

// disassembly
int ProcDisAsm(char *str,int busnum,int addr,int size)
{
  return MainDevice->DisAssembly(str,busnum,addr,size);
}

int ProcSetReg(int rnum,int anum,char* val)
{
  return MainDevice->SetReg(rnum,anum,val);
}

int ProcSetFifo(int fifo_num, char* data, int size)
{
  return MainDevice->SetFifo(fifo_num,data,size);
}

int ProcGetFifo(int fifo_num,char* data,int* size)
{
  return MainDevice->GetFifo(fifo_num,data,*size);
}

int Disassembly(char* str, int busnum, int aAddr, int aSize)
{
  return MainDevice->DisAssembly(str,busnum,aAddr,aSize);
}

int ProcSetPc(uint32 pc)
{
  MainDevice->SetPc(pc);
  return 1;
}

unsigned long ProcGetPc()
{
  return MainDevice->GetPc(0);
}

int ProcGetReg(int rnum,int anum,char* buf)
{
  return MainDevice->GetReg(rnum,anum,buf);
}

int ProcSetBreakPoint(unsigned long addr)
{
  return MainDevice->SetBreakPoint(addr);
}

int ProcClearBreakPoint(unsigned long addr)
{
  return MainDevice->ClearBreakPoint(addr);
}

int ProcStart()
{
  MainDevice->Start();
  return 0;
}

int ProcStop()
{
  MainDevice->Stop();
  return 0;
}

int ProcIsRun()
{
  return MainDevice->IsRun();
}

unsigned long ProcStepInto()
{
//  MainDevice->StepInto();
  SingleClock();
  unsigned long pcval = MainDevice->GetPc(0);
  return pcval;
}

void ProcReset()
{
  MainDevice->Reset();
}

int GetRegIndexByName(char* aName)
{
  tDProcCfg* proc_cfg = (tDProcCfg*)MainDevice->GetCfg();
  
  for(int i=0;i<proc_cfg->nreg;i++)
  {
    if(!strcmp(proc_cfg->regs[i].name,aName))
      return i;
  } 
  return -1;
}
#endif