/**
 * @file config.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef DEBUG
#include <iostream>
#endif
#include <tfile/tfile.h>
#include "os_param.h"
#include "config.h"

namespace wotsen
{

IConfig::IConfig(const std::string &file_name) : file_name_(file_name)
{
	std::string data;

	try
	{
		data = tfile::read(file_name.c_str());
	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}

	try
	{
		conf_ = nlohmann::json::parse(data);
#ifdef DEBUG
		std::cout << conf_.dump(4) << std::endl;
#endif
	}
	catch(const nlohmann::detail::parse_error& e)
	{
		throw std::runtime_error(e.what());
	}
}

nlohmann::json& IConfig::get_conf(void)
{
	return conf_;
}

SysConfig::SysConfig(const std::string &file_name) : IConfig(file_name)
{

}

SysCapability::SysCapability(const std::string &file_name) : IConfig(file_name)
{

}

Configures::Configures()
{
	try
	{
		sys_config_ = std::make_shared<SysConfig>(SYS_CONFIG_FILENAME);
		sys_capability_ = std::make_shared<SysCapability>(OS_SYS_CAPAABILITY_FILENAME);
	}
	catch(const std::runtime_error& e)
	{
		throw e;
	}
}

Configures::~Configures()
{}

std::shared_ptr<Configures>& Configures::configures(void)
{
	static std::shared_ptr<Configures> conf(new Configures);

	return conf;
}

nlohmann::json& Configures::sys_config(void)
{
	return configures()->sys_config_->get_conf();
}

nlohmann::json& Configures::sys_capability(void)
{
	return configures()->sys_capability_->get_conf();
}

}
