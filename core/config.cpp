//
// Created by Administrator on 2024/1/29.
//

#include "config.h"
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;

void Config::Load(const std::string &filename) {
    ptree tree;
    read_json(filename,tree);
    Populate(tree);
}

void Config::Populate(const std::string &json) {

}

void Config::Populate(const ptree &tree) {

}
