// std::map
#include <map>

// string
#include <string>

// std::ifstream
#include <fstream>

// nlohmann::json
#include "json.hpp"

#include "httpd.h"        // Allgemeine Apache-Header-Datei
#include "http_config.h"  // Konfigurations-Header-Datei
#include "http_core.h"    // AP_CORE_MODULE_INDEX
#include "http_log.h"     // Error-Log-Header-Datei
#include "http_protocol.h"// HTTP-Protokoll-Header-Datei
#include "http_request.h" // Request-Header-Datei
#include "ap_config.h"    // Apache-Konfigurations-Header-Datei


#undef APLOG_MODULE_INDEX
#define APLOG_MODULE_INDEX AP_CORE_MODULE_INDEX

using json = nlohmann::json;

class Config{
	public:
		Config(){
			filtersPreRequest = FilterList();
			filtersPostRequest = FilterList();
			filtersPostContent = FilterList();
			whitelist = CharList();
		}
	
		// Config Hooks
	
		static char* SetConfigLocalHook(cmd_parms *cmd, void *cfg, const char *arg){
			configPathLocal = (char*)malloc(strlen(arg) + 1);
			strcpy(configPathLocal, arg);
			return NULL;
		}
	
		static char* SetConfigHook(cmd_parms *cmd, void *cfg, const char *arg){
			configPath = (char*)malloc(strlen(arg) + 1);
			strcpy(configPath, arg);
			return NULL;
		}
	
		static char* SetRedisPath(cmd_parms *cmd, void *cfg, const char *arg){
			redisUrl = (char*)malloc(strlen(arg) + 1);
			strcpy(redisUrl, arg);
			return NULL;
		}
	
		static char* SetBlockCommandHook(cmd_parms *cmd, void *cfg, const char *arg){
			blockCommandFormat = (char*)malloc(strlen(arg) + 1);
			strcpy(blockCommandFormat, arg);
			return NULL;
		}
	
		static char* SetRedisConnectionType(cmd_parms *cmd, void *cfg, const char *arg){
			if(strcmp(arg, "unix"))
				redisProtocol = AF_UNIX;
			redisProtocol = AF_INET;
			return NULL;
		}
	
		static char* SetRedisPort(cmd_parms *cmd, void *cfg, const char *arg){
			redisPort = atoi(arg);
			return NULL;
		}
	
		static char* SetRedisTimeout(cmd_parms *cmd, void *cfg, const char *arg){
			redisTimeout = atoi(arg);
			return NULL;
		}
	
		// Config File Parser
	
		static void Parse(char* configPath){
			std::ifstream f(configPath);
			json data = json::parse(f);
			
			try{
				data.at("maxHits").get_to(maxHits);
			}catch(const std::exception& e){}

			try{
				data.at("blockTime").get_to(blockTime);
			}catch(const std::exception& e){}
			
			try{
				data.at("tickDown").get_to(tickDown);
			}catch(const std::exception& e){}
			
			defaults = new Filter();
			
			try{
				defaults->SetUseRegex(data["defaults"]["useRegex"]);
			}catch(const std::exception& e){}
			
			try{
				defaults->SetScore(data["defaults"]["score"]);
			}catch(const std::exception& e){}
			
			try{
				defaults->SetApplyForCDN(data["defaults"]["applyForCDN"]);
			}catch(const std::exception& e){}
			
			try{
				defaults->SetApplyForBots(data["defaults"]["applyForBots"]);
			}catch(const std::exception& e){}
			
			try{
				defaults->SetApplyForAssets(data["defaults"]["applyForAssets"]);
			}catch(const std::exception& e){}
			
			
			for (auto& el : data["defaults"]["whitelist"].items())
			{
				std::string ip_tmp = el.value().get<std::string>();
				char* ip = (char*)malloc(ip_tmp.length() + 1);
				strcpy(ip, ip_tmp.c_str());
					
				whitelist.Add(ip);
				free(ip);
			}
			
			for (auto& el : data["filters"].items())
			{
				Filter newFilter = Filter();
				
				try{
					char* tmp = (char*)malloc(el.value()["userAgent"].get<std::string>().length());
					strcpy(tmp, el.value()["userAgent"].get<std::string>().c_str());
					newFilter.SetUserAgent(tmp);
					free(tmp);
				}catch(const std::exception& e){}
				
				try{
					for (auto& el2 : el.value()["domain"].items())
					{
						std::string tmp = el2.value().get<std::string>();
						char* tmp2 = (char*)malloc(tmp.length() + 1);
						strcpy(tmp2, tmp.c_str());
					
						newFilter.AddDomain(tmp2);
						free(tmp2);
					}
				}catch(const std::exception& e){}
				
				try{
					char* tmp = (char*)malloc(el.value()["domain"].get<std::string>().length());
					strcpy(tmp, el.value()["domain"].get<std::string>().c_str());
					newFilter.AddDomain(tmp);
					free(tmp);
				}catch(const std::exception& e){}
								
				try{
					char* tmp = (char*)malloc(el.value()["refeer"].get<std::string>().length());
					strcpy(tmp, el.value()["refeer"].get<std::string>().c_str());
					newFilter.SetRefeer(tmp);
					free(tmp);
				}catch(const std::exception& e){}
				
				try{
					char* tmp = (char*)malloc(el.value()["content"].get<std::string>().length());
					strcpy(tmp, el.value()["content"].get<std::string>().c_str());
					newFilter.SetContent(tmp);
					free(tmp);
				}catch(const std::exception& e){}
				
				try{
					char* tmp = (char*)malloc(el.value()["request"].get<std::string>().length());
					strcpy(tmp, el.value()["request"].get<std::string>().c_str());
					newFilter.SetRequest(tmp);
					free(tmp);
				}catch(const std::exception& e){}
				
				try{
					char* tmp = (char*)malloc(el.value()["method"].get<std::string>().length());
					strcpy(tmp, el.value()["method"].get<std::string>().c_str());
					newFilter.SetMethod(tmp);
					free(tmp);
				}catch(const std::exception& e){}
				
				try{
					newFilter.SetStatusCode(el.value()["statusCode"]);
				}catch(const std::exception& e){}
				
				try{
					newFilter.SetUseRegex(el.value()["useRegex"]);
				}catch(const std::exception& e){}
				
				try{
					newFilter.SetScore(el.value()["score"]);
				}catch(const std::exception& e){}
				
				try{
					newFilter.SetApplyForCDN(el.value()["applyForCDN"]);
				}catch(const std::exception& e){}
				
				try{
					newFilter.SetApplyForBots(el.value()["applyForBots"]);
				}catch(const std::exception& e){}
				
				try{
					newFilter.SetApplyForAssets(el.value()["applyForAssets"]);
				}catch(const std::exception& e){}
				
				if(newFilter.GetContent() != NULL){
					// PostContent
					if(newFilter.GetDomains().Count() != 0)
					{
						for(int i = 0; i < newFilter.GetDomains().Count(); i++){
							domainFiltersPostContent[newFilter.GetDomains().Get(i)].Add(newFilter);
						}
					}
					else
						filtersPostContent.Add(newFilter);					
				}
				else if(newFilter.GetStatusCode() != 0){
					// PostProcessing
					if(newFilter.GetDomains().Count() != 0)
					{
						for(int i = 0; i < newFilter.GetDomains().Count(); i++){
							domainFiltersPostRequest[newFilter.GetDomains().Get(i)].Add(newFilter);
						}
					}
					else
						filtersPostRequest.Add(newFilter);
				} else {	
					// PreProcessing
					if(newFilter.GetDomains().Count() != 0)
					{
						for(int i = 0; i < newFilter.GetDomains().Count(); i++){
							domainFiltersPreRequest[newFilter.GetDomains().Get(i)].Add(newFilter);
						}
					}
					else
						filtersPreRequest.Add(newFilter);
				}

			}

			f.close();
		}
	
		static FilterList FiltersPreRequest(){
			return filtersPreRequest;
		}
	
		static FilterList FiltersPostRequest(){
			return filtersPostRequest;
		}

		static FilterList FiltersPostContent(){
			return filtersPostContent;
		}
	
		static FilterList DomainFiltersPreRequest(const char* domain){
			std::string str = domain;
			return domainFiltersPreRequest[domain];
		}
	
		static FilterList DomainFiltersPostRequest(const char* domain){
			std::string str = domain;
			return domainFiltersPostRequest[domain];
		}
	
		static FilterList DomainFiltersPostContent(const char* domain){
			std::string str = domain;
			return domainFiltersPostContent[domain];
		}
	
		static int TickDown(){
			return tickDown;	
		}
	
		static int GetBlockTime(){
			return blockTime;	
		}
	
		static char* GetBlockCommandFormat(){
			return blockCommandFormat;
		}	
	
		static char* GetUnblockCommandFormat(){
			return unblockCommandFormat;
		}
	
		static char* ConfigPath(){
			return configPath;
		}
	
		static char* ConfigPathLocal(){
			return configPathLocal;	
		}
	
		static int MaxHits(){
			return maxHits;
		}
	
		static char* RedisUrl(){
			if(redisUrl == NULL){
				redisUrl = (char*)malloc(strlen("127.0.0.1") + 1);
				strcpy(redisUrl, "127.0.0.1");
			}
			return redisUrl;	
		}
	
		static int RedisPort(){
			return redisPort;	
		}
	
		static int RedisProtocol(){
			return redisProtocol;	
		}
	
		static int RedisTimeout(){
			return redisTimeout;
		}
		
		static CharList Whitelist(){
			return whitelist;
		}
	
	private:
		static int maxHits;
		static int tickDown;
		static int blockTime;
		
		static FilterList filtersPreRequest;
		static FilterList filtersPostRequest;
		static FilterList filtersPostContent;
	
		static std::map<std::string, FilterList> domainFiltersPreRequest;
		static std::map<std::string, FilterList> domainFiltersPostRequest;
		static std::map<std::string, FilterList> domainFiltersPostContent;
	
		static Filter* defaults;
		static CharList whitelist;
	
		static char* blockCommandFormat;
		static char* unblockCommandFormat;
		static char* configPathLocal;
		static char* configPath;
		static char* redisUrl;
		static int redisPort;
		static int redisProtocol;
		static int redisTimeout;
};

int Config::maxHits = 1000;
int Config::tickDown = 5;
int Config::blockTime = 60;
Filter* Config::defaults;
FilterList Config::filtersPreRequest;
FilterList Config::filtersPostRequest;
FilterList Config::filtersPostContent;

std::map<std::string, FilterList> Config::domainFiltersPreRequest;
std::map<std::string, FilterList> Config::domainFiltersPostRequest;
std::map<std::string, FilterList> Config::domainFiltersPostContent;

CharList Config::whitelist;

char* Config::blockCommandFormat;
char* Config::unblockCommandFormat;
char* Config::configPathLocal;
char* Config::configPath;

char* Config::redisUrl;
int Config::redisPort = 6379;
int Config::redisProtocol = AF_INET;
int Config::redisTimeout = 1500;