/**
 * @file sdk_user_manage.cpp
 * @author yuwangliang (astralrovers@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <loguru.hpp>
#if defined(DEBUG)
#include "sdk_user_db_sqlite3.h"
#endif
#include "sdk_user_manage.h"

namespace wotsen
{

// 注册
// 修改资料
// 资料查询
// 删除用户
// 用户列别表

void sdk_user_manage_test(void)
{
#if defined(DEBUG)
    SdkUserDbSqlite3 db("sdk_userdb", "sdk_user");

    db.init();

    SdkDbUser user;

    auto ret = db.query("admin", &user);

    LOG_F(WARNING, "db ret %d\n", ret);

    if (!ret)
    {
        LOG_F(INFO, "id(%d), name(%s), nik(%s), sex(%d), pass(%s), email(%s), create_time(%s)\n",
                    user.id, user.name.c_str(), user.nik.c_str(), user.sex, user.pass.c_str(),
                    user.email.c_str(), user.create_time.c_str());
    }

    std::shared_ptr<char> head_pic(new char[128]);

    sprintf(head_pic.get(), "hello world.");

    SdkDbUser user2;

    user2.name = "ywl";
    user2.nik = "astralrovers";
    user2.sex = 3;
    user2.pass = "hello";
    user2.head_pic.data = head_pic;
    user2.head_pic.len = strlen(head_pic.get());
    user2.phone = "123";
    user2.email = "wotsen@outlook.com";
    user2.permission = 3;

    ret = db.add_user(&user2);

    LOG_F(WARNING, "db ret %d\n", ret);

    ret = db.query("ywl", &user);

    LOG_F(WARNING, "db ret %d\n", ret);

    if (!ret)
    {
        LOG_F(INFO, "id(%d), name(%s), nik(%s), sex(%d), pass(%s), head_pic(%s), email(%s), create_time(%s)\n",
                    user.id, user.name.c_str(), user.nik.c_str(), user.sex, user.pass.c_str(), user.head_pic.data.get(),
                    user.email.c_str(), user.create_time.c_str());
    }

    ret = db.del_user(&user2);

    LOG_F(WARNING, "db ret %d\n", ret);

    ret = db.query("ywl", &user);

    LOG_F(WARNING, "db ret %d\n", ret);

    SdkDbUser *ex = nullptr;
    uint32_t num = 0;

    if (e_sdk_user_db_ok == db.export_user(&ex, num))
    {
        for (uint32_t i = 0; i < num; i++)
        {
        LOG_F(INFO, "id(%d), name(%s), nik(%s), sex(%d), pass(%s), head_pic(%s), email(%s), create_time(%s)\n",
                    ex[i].id, ex[i].name.c_str(), ex[i].nik.c_str(), ex[i].sex, ex[i].pass.c_str(), ex[i].head_pic.data.get(),
                    ex[i].email.c_str(), ex[i].create_time.c_str());
        }

        db.free_export_user(&ex);
    }

    if (ex)
    {
        LOG_F(ERROR, "memory not free.\n");
    }

#endif
}

} // !namespace wotsen
