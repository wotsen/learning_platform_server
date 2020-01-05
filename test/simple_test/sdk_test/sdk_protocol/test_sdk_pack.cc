
#include <fstream>
#include "in_sdk.pb.h"

using namespace std;
using namespace insider::sdk;

void test_sdk_pack(Sdk &msg)
{
	// Sdk msg;

	Header *header = msg.mutable_header();

	header->set_msg_magic(SdkMagic::SDK_MAGIC);
	header->set_version(SdkVersion::SDK_CUR_VERSION);
	header->set_pack_id(3);
	header->mutable_time()->set_in_time(2018);
	header->mutable_time()->set_out_time(2019);
	header->set_data_dir(DataFlow::DATA_IN);
	header->mutable_host()->set_port(9001);
	header->mutable_host()->set_ipv4("127.0.0.1");
	header->set_trans_proto(TransProto::TCP);
	header->set_data_proto(DataProto::DATA_AI);

	Body *body = msg.mutable_body();

	body->mutable_user()->set_user_type(UserInfo::UserType::UserInfo_UserType_U_LOGIN);
	body->mutable_user()->set_token("asdf");
	body->mutable_user()->set_alive_time(3);

	User *user = body->mutable_user()->mutable_user();

	user->set_user_name("admin");
	user->set_user_pass("admin");

	body->set_url("xxx.com");
	body->set_method(OperationType::GET);
	// body->add_response_result(ResponseResult::OK);
	// body->add_response_result(ResponseResult::ERROR);

	Footer *footer = msg.mutable_footer();

	footer->set_res(0x123);

	// std::cout << msg.IsInitialized() << std::endl;

	// try {
	// 	std::cout << msg.DebugString() << std::endl;
	// } catch(std::system_error &err) {
	// 	std::cout << err.what() << std::endl;
	// }

	// fstream output("saa.txt", ios::out | ios::trunc | ios::binary);
	// if (!msg.SerializeToOstream(&output))
	// {
	// 	cerr << "Failed to write address book." << endl;
	// 	return -1;
	// }
	// cout << msg.SerializeAsString().size() << endl;

	// Sdk msg2;

	// msg2.ParseFromArray(msg.SerializeToArray(), msg.ByteSize());
	// cout << msg2.header().msg_magic() << endl;

	// cout << msg.ByteSize() << endl;
	// char *data = new char[msg.ByteSize()];

	// msg.SerializeToArray(data, msg.ByteSize());

	// Sdk msg3;

	// msg3.ParseFromArray(data, msg.ByteSize());
	// cout << msg3.header().msg_magic() << endl;
	// cout << msg3.body().response_result_size() << endl;
	// cout << msg3.body().response_result(1) << endl;

	// google::protobuf::ShutdownProtobufLibrary();

	// delete [] data;

	// return std::move(msg);
}
