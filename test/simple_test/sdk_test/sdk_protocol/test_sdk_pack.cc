
#include <fstream>
#include "in_sdk.pb.h"

using namespace std;
using namespace insider::sdk;

void test_sdk_pack(Sdk &msg)
{
    Header *header = msg.mutable_header();
    Body *body = msg.mutable_body();
    Footer *footer = msg.mutable_footer();

    /**************************************************************/
    header->set_msg_magic(SdkMagic::SDK_MAGIC);
	header->set_version(SdkVersion::SDK_CUR_VERSION);
	header->set_pack_id(1);
	header->mutable_time()->set_in_time(2018);
	header->mutable_time()->set_out_time(2019);
	header->set_data_dir(DataFlow::DATA_IN);
    header->mutable_host()->set_ip_version(IpVersion::IPV4);
	header->mutable_host()->set_port(9002);
	header->mutable_host()->set_ip("127.0.0.1");
    header->mutable_dest()->set_ip_version(IpVersion::IPV4);
	header->mutable_dest()->set_port(9001);
	header->mutable_dest()->set_ip("0.0.0.0");
	header->set_trans_proto(TransProto::TCP);

    /***********************************************************/
    body->mutable_user_session()->set_user_type(UserSessionMsg_UserMethod_U_LOGIN);
    body->mutable_user_session()->mutable_user()->set_user_name("admin");
    body->mutable_user_session()->mutable_user()->set_user_pass("admin");
    body->mutable_user_session()->mutable_user()->set_permission(U_PERMISSION_ADMIN);
    body->mutable_user_session()->mutable_user()->set_email("astralrovers@outlook.com");
    body->mutable_user_session()->mutable_user()->set_phone("12345678901");
	body->mutable_user_session()->set_token("asdf");
	body->mutable_user_session()->set_alive_time(80);

	body->set_url("/index");
	body->set_method(OperationType::GET);

    Content *content = body->mutable_content();

    // 模块列表
    AppModuleCoutInfoList *lists = content->mutable_app_module_list();

    // 添加一个模块
    AppModuleCoutInfo *module = lists->add_app_modules();
    AppModuleBaseInfo *info = module->mutable_base_info();

    module->set_id(0);
    // 使用时不允许设置，只能查询
    module->set_run_state(E_APP_MODULE_RUN_ST_OK);

    info->set_name("test module1");
    info->set_enable(true);

    info->set_state(E_APP_MODULE_IDLE);
    info->set_permission(E_APP_MODULE_CFG_PERMISSION_ENABLE);

    // 添加一个模块
    module = lists->add_app_modules();
    info = module->mutable_base_info();

    info->set_name("test module2");
    info->set_enable(true);

    info->set_state(E_APP_MODULE_INIT);
    info->set_permission(E_APP_MODULE_CFG_PERMISSION_ENABLE);

    /**********************************************************/
    footer->set_res(123);
    Result *result = footer->mutable_result();
    SdkResult *sdk_result = result->mutable_sdk_result();
    ContentResult *content_result = result->mutable_content_result();

    sdk_result->set_status_code(OK);
    sdk_result->set_code("........sdk ok");

    content_result->set_status_code(R_CODE_OK);
    content_result->set_code("........sdk ok");

	return ;
}
