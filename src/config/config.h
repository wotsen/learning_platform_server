/**
 * @file config.h
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-04-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __wotsenLearnPlatSrv_CONFIG_H__
#define __wotsenLearnPlatSrv_CONFIG_H__

#include <memory>
#include <nlohmann/json.hpp>

namespace wotsen
{

class IConfig
{
public:
	IConfig(const std::string &file_name);
    virtual ~IConfig(){};

    virtual nlohmann::json &get_conf(void);

public:
	std::string file_name_;
    nlohmann::json conf_;
};

class SysConfig : public IConfig
{
public:
	SysConfig(const std::string &file_name);
};

class SysCapability : public IConfig
{
public:
	SysCapability(const std::string &file_name);
};

class Configures
{
private:
	Configures();
public:
	~Configures();

public:
	static std::shared_ptr<Configures>& configures(void);
	static nlohmann::json& sys_config(void);
	static nlohmann::json& sys_capability(void);

public:
	// bool load_conf(void);

private:
	std::shared_ptr<SysConfig> sys_config_;			///< 系统配置，每个属性需要标识只读还是可读可写
	std::shared_ptr<SysCapability> sys_capability_;	///< 能力集，只读
};

} // namespace wotsen


#endif // !__wotsenLearnPlatSrv_CONFIG_H__
