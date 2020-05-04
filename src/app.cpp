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

#include <iostream>
#include <loguru.hpp>
#include <task/task.h>
#include <cstdarg>
#include "tools/coredump.h"
#include "config/os_param.h"
#include "version.h"
#include "config.h"
#include "sys_capability.h"
#include "appmodule_manager.h"
#include "appmodule_table.h"
#ifdef DEBUG
#include "sdk_net/sdk_router.h"
#endif
#include "sdk_midwares_table.h"
#if defined(DEBUG)
#include "sdk_session/sdk_user_manage.h"
#endif
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
	std::cout << "start app..................version: " << get_service_version() << std::endl;

	init(argc, argv);
}

void PlatSrvApp::stop(void)
{
    // TODO
	std::cout << "stop app..................version: " << get_service_version() << std::endl;
}

static void log_init(int argc, char **argv)
{
    loguru::init(argc, argv);

#ifdef DEBUG
    loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
#else
    loguru::g_stderr_verbosity = loguru::Verbosity_ERROR;
#endif
    loguru::add_file(SYS_LOG_PATH, loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file(SYS_ERRLOG_PATH, loguru::Append, loguru::Verbosity_ERROR);

    loguru::set_fatal_handler([](const loguru::Message& message) {
        std::string signal_name(message.message);

        if (signal_name == "SIGINT"
            || signal_name == "SIGSEGV"
            || signal_name == "SIGTERM"
            || signal_name == "SIGABRT"
            || signal_name == "SIGBUS"
            || signal_name == "SIGILL")
        {
            // TODO:保证只有主线程会受到这些信号
            wotsen::PlatSrvApp::plat_srv_app()->stop();
            exit(-1);
        }
	});
}

// FIXME:推出时有段错误
void PlatSrvApp::init(int argc, char **argv)
{
	// 日志初始化
    log_init(argc, argv);

    // 加载配置
	Configures::configures();

    // set_task_debug_cb([](const char *fmt, ...) -> void {});

    Task::task_init(get_max_task_num_capability(),
        [](const TaskExceptInfo &info) -> void {
            // TODO:
            LOG_F(ERROR, "except task %ld, %s reason %s\n", info.tid, info.task_name.c_str(), info.reason.c_str());
    });

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

    sdk_user_manage_test();
#endif
}

} // namespace wotsen

