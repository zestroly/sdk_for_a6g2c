#ifndef ZBLUETOOTHSERVER_H
#define ZBLUETOOTHSERVER_H
#include <jsoncpp/json/json.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <list>

#include <cstdarg>
namespace A7 {


class ZBluetoothServer
{
  public:
    typedef void TCallback(const char* buff, int len);
    enum TProtocol {
      SERVER_SPP = 0,
      SERVER_HID,
    };

    struct TDevice{
        TProtocol protocol;
        TCallback* handler;
        char Mac[18];
        bool isWorking;
        bool isPingOk;
        std::thread* ServerThread;
    };


    void runService(TProtocol protocol, const char* mac ,TCallback hander);
    void delServer(const char* mac);
    void listServer(std::list<TDevice> &Devs);


    ZBluetoothServer();
    ~ZBluetoothServer();
    void loadConfig();
    void saveConfig();
    void updateConfig();

    void scan(std::map<std::string,std::string> &Devs);

    static void rfcomm_server_thread(TDevice* device);
    static void L2ping_thread(TDevice* device);

    void hid_server(TDevice* device);

    void hid_service(TDevice* device);

    void rfcomm_connect(std::string mac);

    void listDev(std::list<TDevice*>& Devs);
    void inserDev(TDevice* &device);
    void popDev(TDevice* &device, const char* mac);

  private:
    std::mutex _DevsMutex;
    std::list<TDevice*> _Devs;

    int _bt_dev_id;
    int _btsock;
};

}
#endif // ZBLUETOOTHSERVER_H
