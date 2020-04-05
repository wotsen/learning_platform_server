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

#define LOG_TAG "APP"

#include <iostream>
#include <easylogger/easylogger_setup.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
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
	alone_run();
	exception_setup();
}

PlatSrvApp::~PlatSrvApp()
{

}

void PlatSrvApp::alone_run(void)
{
	// 保证只有一个程序运行
    int fd = open(PID_FILE, O_RDWR | O_CREAT, 0666);
    char buf[16] = {'\0'};

    if (fd < 0)
    {
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }

	auto lock_file = [&](void) -> int
	{
		struct flock fl;

		fl.l_type = F_WRLCK; // 锁定文件，锁定失败则是已经运行了一个
		fl.l_start = 0;
		fl.l_whence = SEEK_SET;
		fl.l_len = 0;

		return fcntl(fd, F_SETLK, &fl);
	};

    if (lock_file() < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            std::cout << "proccessing is running!" << std::endl;
            exit(-1);
        }
		std::cout << "can't lock " << PID_FILE << ", reason :" << strerror(errno) << std::endl;
    }

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
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

void PlatSrvApp::run(void)
{
	init();

	std::cout << "start app..................version: " << get_service_version() << std::endl;
}

void PlatSrvApp::stop(void)
{

}

static void log_init(void)
{
    // 日志采用elog
    struct elog_custom_config elog_config = {
        .log_path = (char *)SYS_ELOG_PATH,
#ifdef NDEBUG
        .log_lv = ELOG_LVL_INFO
#else
        // debug版本打印所有日志
        .log_lv = ELOG_LVL_VERBOSE
#endif
    };

    if (easylogger_setup(&elog_config))
    {
        log_i("日志模块初始化完成...\n");
    }
    else
    {
        std::cout << "日志模块初始化失败!\n";
        exit(0);
    }
}

void PlatSrvApp::init(void)
{
	// 日志初始化
    log_init();

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

