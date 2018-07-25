#ifndef DEBUGGER_INTERFACE_H
#define DEBUGGER_INTERFACE_H
#include <datatypes.h>
#include <debug_structs.h>
#include <string.h>


#define PERIPHERAL_OFFSET  25

// breakpoint
enum ePointType
{
  BP_STANDARD,
  BP_SINGLE_EXEC,
  BP_ON_NEXT,
  BP_WATCHPOINT,
};

typedef struct
{
  uint32 mStartAddr; // start disassembly addr
  uint32 mSize;
}tDisAsmArea;


struct tDisAsmDbgArea :public tDisAsmArea
{
  uint32 mEndPoint;
};

// ����������� ����� ��� ���������� �����������.
class TDbgInterface
{
public:
  // ��������� elf ���� � ������ ����������
  // ���������:
  //
  virtual tDisAsmArea* LoadElf(char* aElfFile) = 0;

  // ���������� ������ ���������� ���������� ������ ������ � ������
  // ���������:
  //    busnum - ������ ���� ������
  //    addr - ����� ������
  virtual int GetCellSize(int busnum,int addr) = 0;

  // ���������� ��������� �� ���������, ����������� ������������ ����������,
  // ��� ��:
  //   ������ ���������, ���������� �������� ��������, ������ � ����� � ������ �������
  // (��� ������� ���������)
  //   ���������� � �������� ��������� ��� ������
  //  ������ ������ ����������
  virtual const tDProcCfg* GetCfg()= 0;

  // ���������� size ���� ������ ������� � ������ addr � ������� mem.
  // ���������:
  //   mem-  ��������� �� ������, � ������� ����� ������� ������ �� ����������
  //   busnum - ������ ���� ������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, ������� ���������� �������
  virtual int GetMem(void *mem, int busnum, int addr,int size)=0;

  // ���������� size ���� ������ ������� ������� � ������ addr � ������� mem.
  // ���������:
  //   mem-  ��������� �� ������, � ������� ����� ������� ������ �� ����������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, ������� ���������� �������
  virtual int GetProgMem(void *mem, int addr, int size)=0;
  // ���������� size ���� ������ ������ ������� � ������ addr � ������� mem.
  // ���������:
  //   mem-  ��������� �� ������, � ������� ����� ������� ������ �� ����������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, ������� ���������� �������
  virtual int GetDataMem(void *mem,int addr,int size)=0;

  // ���������� size ���� ������ ������� � ������ addr, ������ �������� �� ������� mem.
  // ���������:
  //   mem-  ��������� �� ������, � ������� ����� ������� ������ �� ����������
  //   busnum - ������ ���� ������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, � ������� �������� ������
  virtual int SetMem(void *mem,int busnum,int addr,int size)=0;

  // ���������, ���� �� ��� ��������� ��������� ������� ���������� ������.
  // ���������:
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, � ������� �������� ������
  //   busnum - ������ ���� ������
  virtual int IsMemInRange(int addr,int size,int busnum = 0) = 0;

  // ���������� size ���� ������ ������� � ������ addr, ������ �������� �� ������� mem.
  // ���������:
  //   mem-  ��������� �� ������, � ������� ����� ������� ������ �� ����������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, � ������� �������� ������
  virtual int SetProgMem(void *mem,int addr,int size)=0;

  // ���������� size ���� ������ ������� � ������ addr, ������ �������� �� ������� mem.
  // ���������:
  //   mem-  ��������� �� ������, � ������� ����� ������� ������ �� ����������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, � ������� �������� ������
  virtual int SetDataMem(void *mem,int addr,int size)=0;

  // ��������� ������� ������ ��������� aVal
  //   busnum - ������ ���� ������
  //   addr - ��������� ����� � ������ ����������
  //   size - ������ ������ � ������, � ������� �������� ������
  virtual int FillMem(uint32 aVal,int busnum,int addr,int size)=0;

  // ����������������� ��������� �������� size, ����������� �� ������ bin
  // ������� ���������� ��������� �� ������, ���������� ����� ���������� � ���������
  // ������� : "�����     ��� ����������\n ������������������ ����������\n
  virtual int DisAssembly(char *str,int busnum,int addr,int size) = 0;

  // �������� �������� � ������� � �������� rnum, ��� � ������� ������������ ������� rnum
  // � ������� anum.
  // ���������:
  // rnum - ������ �������� ��� ������� ���������
  // anum - ������ �������� � ����������� ������� (� ������ �������� ������������)
  // val - �������� ��� ������
  virtual int SetReg(int rnum,int anum,char* val)=0;

  // ���������� �������� �������� � �������� rnum, ��� ������� ������������ ������� rnum
  // � ������� anum.
  // ���������:
  // rnum - ������ �������� ��� ������� ���������
  // anum - ������ �������� � ����������� ������� (� ������ �������� ������������)
  // val - ��������� �� ������� ������ ��� ���������� ��������
  virtual int GetReg(int rnum,int anum,char* val)=0;

  // ���������� �������� �������� ���������
  virtual int SetPc(uint32 val)=0;


  // ������������� �������� ���� � �������� fifo_num
  // ���������:
  // fifo_num - ������ ����
  // data - ��������� �� ������ ����� �������� ����
  // size - ����� ������ ����
  virtual int SetFifo(int fifo_num,char* data,int size)=0;

  // ������������� �������� ���� � �������� fifo_num
  // ���������:
  // fifo_num - ������ ����
  // data - ��������� �� ������, � ������� ����� ��������� ����
  // size - ������ ����
  virtual int GetFifo(int fifo_num,char* data,int& size)=0;

  // ������� �������� �������� ���������
  virtual uint32 GetPc(int val) = 0;

  // ���������� ����� �������� �� ������ addr
  virtual int SetBreakPoint(uint32 addr,ePointType aType = BP_STANDARD) = 0; // set breakpoint

  // ������ ����� �������� �� ������ addr
  virtual int ClearBreakPoint(uint32 addr) = 0 ; // remove breakpoint
  //
  // �������� ������
  virtual void ClearMem()  = 0;

  //  �������� ���������
  virtual void Reset() = 0;

  // ������ ����������
  virtual void Start() = 0;

  // ���������� ����������
  virtual void Stop() = 0;

  // ���������, ������� �� ������� ���������
  virtual int IsRun()=0;

  // �������� ���������� ����� ����������
  virtual void StepInto() = 0;

  // ������� ���������� ���������� ������ � ������� ��������� ������������
  virtual unsigned int GetNumCycles() = 0;

  // ������� ���������� ����� ����������
  virtual int   GetInstructNum(uint64 wrd) = 0;
  virtual int   GetKindOfInstruction(uint32 aAddr) = 0;
  virtual char* GetNameFromPtr(void* ptr) = 0;

  // new service functions
  inline uint8 GetReg8(int rnum,int anum)
  {
    uint8 reg;
    this->GetReg(rnum,anum,(char*)&reg);
    return reg;
  }

  inline uint16 GetReg16(int rnum,int anum)
  {
    uint16 reg;
    this->GetReg(rnum,anum,(char*)&reg);
    return reg;
  }

  inline uint32 GetReg32(int rnum,int anum)
  {
    uint32 reg;
    this->GetReg(rnum,anum,(char*)&reg);
    return reg;
  }

  inline uint64 GetReg64(int rnum,int anum)
  {
    uint64 reg;
    this->GetReg(rnum,anum,(char*)&reg);
    return reg;
  }

  inline int SetReg8(int rnum,int anum,uint8 val)
  {
    return this->SetReg(rnum,anum,(char*)&val);
  }

  inline int SetReg16(int rnum,int anum,uint16 val)
  {
    return this->SetReg(rnum,anum,(char*)&val);
  }

  inline int SetReg32(int rnum,int anum,uint32 val)
  {
    return this->SetReg(rnum,anum,(char*)&val);
  }

  inline int SetReg64(int rnum,int anum,uint64 val)
  {
    return this->SetReg(rnum,anum,(char*)&val);
  }

  inline int GetRegIdx(char* aRegName)
  {
    const tDProcCfg* cfg = this->GetCfg();
    for(int i = 0;i < cfg->nreg;i++)
    {
      if(!strcmp(cfg->regs[i].name,aRegName))
        return i;
    }
    return -1;
  }
};


#endif
