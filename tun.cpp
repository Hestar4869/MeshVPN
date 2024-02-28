//
// Created by Administrator on 2024/1/23.
//


#include <WS2tcpip.h>
#include <boost/log/trivial.hpp>
#include "tun.h"

std::shared_ptr<Tun> Tun::CreateTun(std::string name, std::string ip, uint8_t mask) {
    std::shared_ptr<Tun> ret ;
#ifdef WIN32
    ret = std::make_shared<WinTun>(name,ip,mask);
#endif

#if defined(__linux__) || defined(__unix__)
    ret = std::make_shared<UnixTun>(name,ip,mask);
#endif
    return ret;


}

#ifdef WIN32
HMODULE WinTun::wintun_;
WinTun::Garbo WinTun::garbo;
WINTUN_CREATE_ADAPTER_FUNC *WinTun::WintunCreateAdapter;
WINTUN_CLOSE_ADAPTER_FUNC *WinTun::WintunCloseAdapter;
WINTUN_OPEN_ADAPTER_FUNC *WinTun::WintunOpenAdapter;
WINTUN_GET_ADAPTER_LUID_FUNC *WinTun::WintunGetAdapterLUID;
WINTUN_GET_RUNNING_DRIVER_VERSION_FUNC *WinTun::WintunGetRunningDriverVersion;
WINTUN_DELETE_DRIVER_FUNC *WinTun::WintunDeleteDriver;
WINTUN_SET_LOGGER_FUNC *WinTun::WintunSetLogger;
WINTUN_START_SESSION_FUNC *WinTun::WintunStartSession;
WINTUN_END_SESSION_FUNC *WinTun::WintunEndSession;
WINTUN_GET_READ_WAIT_EVENT_FUNC *WinTun::WintunGetReadWaitEvent;
WINTUN_RECEIVE_PACKET_FUNC *WinTun::WintunReceivePacket;
WINTUN_RELEASE_RECEIVE_PACKET_FUNC *WinTun::WintunReleaseReceivePacket;
WINTUN_ALLOCATE_SEND_PACKET_FUNC *WinTun::WintunAllocateSendPacket;
WINTUN_SEND_PACKET_FUNC *WinTun::WintunSendPacket;

WinTun::WinTun(std::string name, std::string ip, uint8_t mask)
        : Tun(), adapter_(nullptr), session_(nullptr),
          name_(name), ip_(ip), mask_(mask),
          quitEvent(nullptr), haveQuit(false) {
    InitializeWintun();
}

WinTun::~WinTun() {
    Cleanup();
}

void WinTun::InitializeWintun(void) {
    if (!wintun_) {
        wintun_ = LoadLibraryExW(L"wintun.dll", NULL,
                                 LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);

    }
    if (!wintun_) {
        BOOST_LOG_TRIVIAL(error) << "Failed to load wintun library, last error code is " << GetLastError();
        return;
    }
    BOOST_LOG_TRIVIAL(trace) << "wintun library loaded";
#define X(Name) ((*(FARPROC *)&Name = GetProcAddress(wintun_, #Name)) == NULL)
    if (X(WintunCreateAdapter) || X(WintunCloseAdapter) || X(WintunOpenAdapter) || X(WintunGetAdapterLUID) ||
        X(WintunGetRunningDriverVersion) || X(WintunDeleteDriver) || X(WintunSetLogger) || X(WintunStartSession) ||
        X(WintunEndSession) || X(WintunGetReadWaitEvent) || X(WintunReceivePacket) || X(WintunReleaseReceivePacket) ||
        X(WintunAllocateSendPacket) || X(WintunSendPacket))
#undef X
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to init wintun functions, last error code is " << GetLastError();
        DWORD LastError = GetLastError();
        FreeLibrary(wintun_);
        SetLastError(LastError);
    }
    BOOST_LOG_TRIVIAL(trace) << "wintun functions loaded";
    BOOST_LOG_TRIVIAL(info) << "WinTun init successfully";
}

void WinTun::Cleanup() {
    if (session_) {
        WintunEndSession(session_);

    }
    if (adapter_)
        WintunCloseAdapter(adapter_);
    BOOST_LOG_TRIVIAL(info) << "WinTun clean up";
}

bool WinTun::Start() {
    // 将char*转换为宽字节，CP_ACP表示当前系统所用字符集
    int len = MultiByteToWideChar(CP_ACP, 0, name_.c_str(), -1, NULL, 0);
    LPWSTR wname = new WCHAR[len];
    MultiByteToWideChar(CP_ACP, 0, name_.c_str(), -1, wname, len);

    // 创建适配器
    adapter_ = WintunCreateAdapter(wname, L"Wintun", nullptr);

    if (!adapter_) {
        auto LastError = GetLastError();
        BOOST_LOG_TRIVIAL(error) << "Failed to create adapter, last error code is " << LastError;
        // log
        return false;
    }
    BOOST_LOG_TRIVIAL(trace) << "Adapter created successfully";
    // 设置子网以及自己ip
    MIB_UNICASTIPADDRESS_ROW AddressRow;
    InitializeUnicastIpAddressEntry(&AddressRow);
    WintunGetAdapterLUID(adapter_, &AddressRow.InterfaceLuid);
    AddressRow.Address.Ipv4.sin_family = AF_INET;
    inet_pton(AF_INET, ip_.c_str(), &AddressRow.Address.Ipv4.sin_addr.S_un.S_addr);
    AddressRow.OnLinkPrefixLength = mask_; /* This is a /mask network */
    AddressRow.DadState = IpDadStatePreferred;
    auto LastError = CreateUnicastIpAddressEntry(&AddressRow);
    if (LastError != ERROR_SUCCESS && LastError != ERROR_OBJECT_ALREADY_EXISTS) {
        BOOST_LOG_TRIVIAL(error) << "Failed to set IP address, error code is " << LastError;
        return false;
    }
    BOOST_LOG_TRIVIAL(trace) << "IP address set successfully";

    session_ = WintunStartSession(adapter_, 0x400000);
    if (!session_) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create session, error code is " << LastError;
        return false;
    }

    BOOST_LOG_TRIVIAL(info) << "WinTun started successfully";
    return true;
}
#endif

#if defined(__linux__) || defined(__unix__)

#endif
