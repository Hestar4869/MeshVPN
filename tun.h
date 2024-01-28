//
// Created by Administrator on 2024/1/23.
//

#ifndef MESHVPN_TUN_H
#define MESHVPN_TUN_H

#include <memory>
#include <functional>
#include <string>
#include <winsock2.h>
#include <Windows.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <mstcpip.h>
#include "external/wintun/wintun.h"



class Tun {
public:
    Tun() = default;
    virtual ~Tun() = default;
    virtual bool Start() = 0;

    // 静态工厂方法，根据条件返回具体实现的指针
    static std::shared_ptr<Tun> CreateTun(std::string name, std::string ip, uint8_t mask);
};

// Windows环境
#ifdef WIN32

class WinTun : public Tun {
public:
    WinTun(std::string name, std::string ip, uint8_t mask);

    ~WinTun() override;

    bool Start() override;

//    bool CreateAdapter();

//    bool StartSession();

private:

    WINTUN_ADAPTER_HANDLE adapter_;
    WINTUN_SESSION_HANDLE session_;
    std::string name_;
    std::string ip_;
    uint8_t mask_;
    HANDLE quitEvent;
    volatile BOOL haveQuit;

    //todo 如何优雅的释放该句柄资源
    static HMODULE wintun_;

    class Garbo {
    public:
        ~Garbo() {
            if (WinTun::wintun_)
                FreeLibrary(WinTun::wintun_);
        }
    };

    // 当程序结束时，会自动的调用garbo析构函数并释放wintun_
    static Garbo garbo;

    // wintun.dll中的函数
    static WINTUN_CREATE_ADAPTER_FUNC *WintunCreateAdapter;
    static WINTUN_CLOSE_ADAPTER_FUNC *WintunCloseAdapter;
    static WINTUN_OPEN_ADAPTER_FUNC *WintunOpenAdapter;
    static WINTUN_GET_ADAPTER_LUID_FUNC *WintunGetAdapterLUID;
    static WINTUN_GET_RUNNING_DRIVER_VERSION_FUNC *WintunGetRunningDriverVersion;
    static WINTUN_DELETE_DRIVER_FUNC *WintunDeleteDriver;
    static WINTUN_SET_LOGGER_FUNC *WintunSetLogger;
    static WINTUN_START_SESSION_FUNC *WintunStartSession;
    static WINTUN_END_SESSION_FUNC *WintunEndSession;
    static WINTUN_GET_READ_WAIT_EVENT_FUNC *WintunGetReadWaitEvent;
    static WINTUN_RECEIVE_PACKET_FUNC *WintunReceivePacket;
    static WINTUN_RELEASE_RECEIVE_PACKET_FUNC *WintunReleaseReceivePacket;
    static WINTUN_ALLOCATE_SEND_PACKET_FUNC *WintunAllocateSendPacket;
    static WINTUN_SEND_PACKET_FUNC *WintunSendPacket;

    static void InitializeWintun(void);

    void Cleanup();
};

#endif


#if defined(__linux__) || defined(__unix__)
// Linux环境
class UnixTun : public Tun{

};
#endif

#endif //MESHVPN_TUN_H
