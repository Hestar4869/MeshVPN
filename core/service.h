//
// Created by Administrator on 2024/1/29.
//

#ifndef MESHVPN_SERVICE_H
#define MESHVPN_SERVICE_H

#include <string>
#include <cstdint>
#include <map>
#include "../tun.h"

class Service{
public:
    bool BuildSubnet(std::string ip,uint8_t mask);
private:
    std::map<std::string,Tun> tuns;
};

#endif //MESHVPN_SERVICE_H
