#define DATA_32BITS

#include "ppdl.h"
#include "evm_sim.h"

#define BLOCK_SIZE 1024*512 // 512 килобайт памяти
#define IMEM_WS 1


EVM *new_evm;

typedef tSimpleMemBlock<uint32, uint32, uint8> TProgMem;
typedef tSimpleMemBlock<uint32, uint256, uint256> TDataMem;

TProgMem* prog_mem;
TDataMem* data_mem;
TDataMem* storage_mem;
TDataMem* log_mem;
TDataMem* input_mem;

// KECCAK algorithm
uint256 KeccakAlg(TDevice* dev, uint32 offs, uint32 size)
{
  EVM* my_evm = (EVM*)dev;
  char f = my_evm->data_bus[offs]; // evm->data_bus + offs - pointer into start
  char e = my_evm->data_bus[offs + size - 1];
  printf("keccak success, first symb is %c last is %c\n",f,e);
  return uint256(0);
}


void* Init(void* mParams)
{
  // инициализируем подсистему памяти
  cSwapClass::Init(0x10000000);
  unsigned int peripherals;  
  int is_log_mem;

  prog_mem = new TProgMem(BLOCK_SIZE,IMEM_WS);
  data_mem = new TDataMem(BLOCK_SIZE,IMEM_WS);
  storage_mem = new TDataMem(BLOCK_SIZE,IMEM_WS);
  log_mem = new TDataMem(BLOCK_SIZE,IMEM_WS);
  input_mem = new TDataMem(BLOCK_SIZE, IMEM_WS);

  new_evm = new EVM("evm1");

  new_evm->prog_bus = prog_mem;
/*  new_evm->data_bus = data_mem;
  new_evm->storage_bus = storage_mem;
  new_evm->log_bus = log_mem;
  new_evm->input_data = input_mem;
  */
return (void*)new_evm;
}

void Destroy()
{
	delete prog_mem;
	delete data_mem;
	delete storage_mem;
	delete log_mem;
}

LIB_EXPORT void CheckPostProgram()
{
  // save storage mem and data mem 
  Logger.CloseLog();

  // 
  if (new_evm != NULL)
  {
    int256 res = new_evm->Pop();
    int32 ret_val = (res != 0); 
    printf("return value is %s\n", res.to_string().c_str());
    exit(ret_val);
  }
}