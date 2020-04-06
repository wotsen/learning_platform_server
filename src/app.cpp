/**
 * @file app.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

// #define LOG_TAG "APP"

#include <iostream>
// #include <easylogger/easylogger_setup.h>
#include <loguru.hpp>
#include "tools/coredump.h"
#include "config/os_param.h"
#include "version.h"
#include "config.h"
#include "sys_capability.h"
#include "appmodule_manager.h"
#include "appmodule_table.h"
#include "task_manage/task_manage.h"
#ifdef DEBUG
#include "sdk_net/sdk_router.h"
#endif
#include "sdk_midwares_table.h"
#include "app.h"

namespace wotsen
{

PlatSrvApp::PlatSrvApp()
{
	exception_setup();
}

PlatSrvApp::~PlatSrvApp()
{

}

void PlatSrvApp::exception_setup(void)
{
#if defined(DEBUG)
    // 调试版本开启coredump
    setup_coredump(SYS_SOFT_CORDUMP_PATH, SYS_SOFT_COREDUMP_SIZE) ? (void)0 : exit(0);
#endif
}

std::shared_ptr<PlatSrvApp> &PlatSrvApp::plat_srv_app(void)
{
	static std::shared_ptr<PlatSrvApp> app(new PlatSrvApp);

	return app;
}

void PlatSrvApp::run(int argc, char **argv)
{
	init(argc, argv);

	std::cout << "start app..................version: " << get_service_version() << std::endl;
}

void PlatSrvApp::stop(void)
{
	std::cout << "stop app..................version: " << get_service_version() << std::endl;
}

static void log_init(int argc, char **argv)
{
    // 日志采用elog
//     struct elog_custom_config elog_config = {
//         .log_path = (char *)SYS_ELOG_PATH,
// #ifdef NDEBUG
//         .log_lv = ELOG_LVL_INFO
// #else
//         // debug版本打印所有日志
//         .log_lv = ELOG_LVL_VERBOSE
// #endif
//     };

//     if (easylogger_setup(&elog_config))
//     {
//         log_i("日志模块初始化完成...\n");
//     }
//     else
//     {
//         std::cout << "日志模块初始化失败!\n";
//         exit(0);
//     }

    loguru::init(argc, argv);
    loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
    loguru::add_file("../log/everything.log", loguru::Append, loguru::Verbosity_MAX);

    loguru::set_fatal_handler([](const loguru::Message& message){
        std::string signal_name(message.message);

        if (signal_name == "SIGINT"
            || signal_name == "SIGSEGV"
            || signal_name == "SIGTERM"
            || signal_name == "SIGABRT"
            || signal_name == "SIGBUS"
            || signal_name == "SIGILL")
        {
            wotsen::PlatSrvApp::plat_srv_app()->stop();
            exit(-1);
        }
	});
}

void PlatSrvApp::init(int argc, char **argv)
{
	// 日志初始化
    log_init(argc, argv);

    // 加载配置
	Configures::configures();

    // 初始化任务管理
    wotsen::task_manage_init(get_max_task_num_capability(),
                            (wotsen::abnormal_task_do)(
                                [](const struct except_task_info &except_task) -> void {}));

    // 应用模块初初始化
    PlatSrvAppModuleManager::appmodule_manager(
                                    get_sys_modules(),
                                    get_ext_modules(),
                                    get_sys_apps(),
                                    get_ext_apps());
    PlatSrvAppModuleManager::init();

    // 注册中间件
	register_sdk_midwares();

#ifdef DEBUG
    show_sdk_router_map();
    _print_app_module_state();
#endif

}

} // namespace wotsen

