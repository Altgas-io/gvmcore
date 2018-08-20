#define STEPGAS0         0
#define STEPGAS1         2
#define STEPGAS2         3
#define STEPGAS3         5
#define STEPGAS4         8
#define STEPGAS5         10
#define STEPGAS6         20
#define STEPGAS7         0
#define EXPBYTEGAS       10
#define SHA3GAS          30
#define SHA3WORDGAS      6
#define SLOADGAS         50
#define SSTORESETGAS     20000
#define SSTORERESETGAS   5000
#define SSTORECLEARGAS   5000
#define JUMPDESTGAS      1
#define LOGGAS           375
#define LOGDATAGAS       8
#define LOGTOPICGAS      375
#define CREATEGAS        32000
#define CALLGAS          40
#define MEMORYGAS        3
#define COPYGAS          3
#define VALUETRANSFERGAS 9000
#define CALLSTIPEND      2300
#define CALLNEWACCOUNT   25000

#define USEGAS(c) if(gas_available < c) StopExec(); else gas_available = gas_available - c;