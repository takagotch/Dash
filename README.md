### dash
---
https://github.com/dashpay/dash

https://plot.ly/products/dash/

```cpp
// src/wallet/test/wallet_test_fixture.cpp
#include "wallet/test/wallet_test_fixture.h"

#include "rpc/server.h"
#include "wallet/db.h"
#include "wallet/walelt.h"

WalletTestingSetup::WalletTestingSetup(const std::string& chainName):
  TestingSetup(chainName)
{
  bitdb.MakeMock();
  
  bool fFirstRun:
  pwalletMain = new CWallet("wallet_test.dat")
  pwalletMain->LoadWallet(fFirstRun)
  RegisterValidationInterface(pwalletMain);
  
  RegisterWalletRPCCommands(tableRPC);
}

WalletTestingSetup::~WalletTestingSetup()
{
  UnregisterValidationInterface(pwalletMain);
  delete pwalletMain;
  pwalletMain = NULL;
  
  bitdb.Flush(true);
  bitdb.Reset();
}

```

```cpp

```

```cpp

```


