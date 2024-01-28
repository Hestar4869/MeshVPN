//
// Created by Administrator on 2024/1/23.
//

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include"tun.h"

//#include "tun.h"

namespace logging = boost::log;
namespace expr = boost::log::expressions;



void init_logging(logging::trivial::severity_level log_level) {
    // 设置日志输出格式
    //logging::add_console_log(std::clog, boost::log::keywords::format = "%TimeStamp% [%ThreadID%] %Message%");

    // 如果需要文件输出，可以使用下面的配置
    // logging::add_file_log("sample.log");

    // 设置日志过滤条件
    logging::core::get()->set_filter(logging::trivial::severity >= log_level);
}

int main() {

    init_logging(logging::trivial::info);
    std::shared_ptr<Tun> tun = Tun::CreateTun("wintun","192.168.10.1",24);
    tun->Start();
    // 休眠1min，查看tun是否创建
//    _sleep(60*1000);
    BOOST_LOG_TRIVIAL(info) << "Hello, MeshVPN!";
    return 0;
}