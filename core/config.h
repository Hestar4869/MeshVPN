//
// Created by Administrator on 2024/1/29.
//

#ifndef MESHVPN_CONFIG_H
#define MESHVPN_CONFIG_H

#include <string>
#include <boost/property_tree/ptree.hpp>

class Config{
public:
    std::string auth_server;


    void Load(const std::string& filename);
    void Populate(const std::string& json);
private:
    void Populate(const boost::property_tree::ptree &tree);
};
#endif //MESHVPN_CONFIG_H
