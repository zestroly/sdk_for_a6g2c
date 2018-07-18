#include <iostream>
#include "ZBluetoothServer.h"
#include <unistd.h>


using namespace std;
using namespace A7;

char buff_pack[512];
int len;

void callback(const char* buff, int len)
{
  cout<<"callback["<<len<<"]("<<buff<<")"<<endl;
}

int main()
{
  map<string, string> devs;
  ZBluetoothServer btServer;
#if 0
  cout<<"start scan"<<endl;
  btServer.scan(devs);
  for(auto dev : devs)
  {
    cout<<dev.first<<"|"<<dev.second<<endl;
  }
#endif


//  btServer.runService(A7::ZBluetoothServer::SERVER_SPP, "DC:2C:26:09:66:8F", callback);
  btServer.runService(A7::ZBluetoothServer::SERVER_HID, "DC:2C:26:09:66:8F", NULL);

  getchar();
  while(1)
  {
    sleep(1);

#if 0
    std::list<ZBluetoothServer::TDevice*> devs;
    btServer.listDev(devs);
    for(auto& pdev : devs)
    {
      std::cout<<"[working]"<<pdev->Mac<<std::endl;
    }
#endif


  }
  return 0;
}
