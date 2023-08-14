// FilterList
#include "filter_list.cpp"

// Config
#include "config.cpp"

// redis_interface_sync
#include "redis_interface_sync.cpp"

// request_rec
#include <httpd.h>

// unix timestamp
#include <chrono>

#include "http_config.h"  // Konfigurations-Header-Datei
#include "http_core.h"    // AP_CORE_MODULE_INDEX
#include "http_log.h"     // Error-Log-Header-Datei
#include "http_protocol.h"// HTTP-Protokoll-Header-Datei
#include "http_request.h" // Request-Header-Datei
#include "ap_config.h"    // Apache-Konfigurations-Header-Datei



char REDIS_IS_BLOCKED_KEY[] = "mod_antiddos_blocked_";
char REDIS_SCORE_KEY[] =  "mod_antiddos_score_";
char REDIS_CLEANUP_TIMESTAMP_KEY[] = "mod_antiddos_timestamp_";

void Log(char* line){
	// std::cout << line << "\n";	
}

void Log(char* part1, char* part2){
	char* msg = (char*)malloc(strlen(part1) + strlen(part2) + 1);
	strcpy(msg, part1);
	strcat(msg, part2);
	Log(msg);
	free(msg);
}

class AntiDDoSWorker{
	public:
		~AntiDDoSWorker(){
			DisconnectRedisServer();
		}
	
		AntiDDoSWorker(){
//			LoadConfig();
			ConnectRedisServer();
		}
		
		static int PostRequestHook(request_rec *r){
    		// ignore whitelist
			if(Config::Whitelist().ContainsIP(r->connection->client_ip)){			
				return DECLINED;
			}
			
			AntiDDoSWorker worker = AntiDDoSWorker();
			
			int score = 0;
			
			for(int i = 0; i < Config::FiltersPostRequest().Count(); i++){
				score += Config::FiltersPostRequest().Get(i).GetScore(r, true);
			}
			
			if(score > 0){
				ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, r->server,
                	"add score for %s from postrequest (%i)", r->connection->client_ip, score);

				worker.IncreaseScore(r->connection->client_ip, score);
					
				if(worker.GetScore(r->connection->client_ip) > Config::MaxHits())
				{
					worker.TickDownScore(r->connection->client_ip);
					if(worker.GetScore(r->connection->client_ip) > Config::MaxHits()){
						worker.Block(r->connection->client_ip);
							
						r->status = 429;
						
						ap_log_error(APLOG_MARK, APLOG_WARNING, 0, r->server,
                     		"block access, client exceeded score limit: %s (postrequest)", r->connection->client_ip);
						
						return OK;
					}
						
				}
			}
			
			return DECLINED;
		}
		
		static int PreRequestHook(request_rec *r){
			if(Config::Whitelist().ContainsIP(r->connection->client_ip)){
				ap_log_error(APLOG_MARK, APLOG_INFO, 0, r->server,
                     "ignore whitelisted ip: %s", r->connection->client_ip);
			
				return DECLINED;
			}
			
			AntiDDoSWorker worker = AntiDDoSWorker();
			
			if(worker.CheckIfIsBlocked(r->connection->client_ip)){
				r->status = 429;
				ap_log_error(APLOG_MARK, APLOG_INFO, 0, r->server,
                     "detect blocked access, too many requests: %s", r->connection->client_ip);
				
				return OK;
			}
			
			int score = 0;
			
			for(int i = 0; i < Config::FiltersPreRequest().Count(); i++){
				score += Config::FiltersPreRequest().Get(i).GetScore(r);
			}
			
			if(score > 0){
				ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, r->server,
                	"add score for %s (%i)", r->connection->client_ip, score);

				worker.IncreaseScore(r->connection->client_ip, score);
					
				if(worker.GetScore(r->connection->client_ip) > Config::MaxHits())
				{
					worker.TickDownScore(r->connection->client_ip);
					if(worker.GetScore(r->connection->client_ip) > Config::MaxHits()){
						worker.Block(r->connection->client_ip);
							
						r->status = 429;
						
						ap_log_error(APLOG_MARK, APLOG_WARNING, 0, r->server,
                     		"block access, client exceeded score limit: %s", r->connection->client_ip);
						
						return OK;
					}
						
				}
			}
			   
			return DECLINED;
		}
	
		// Apache Post Config Module Hook
		static int PostConfigHook(apr_pool_t* pool_a, apr_pool_t* pool_b, apr_pool_t* pool_c, server_rec* s){
			LoadConfig();
			
			return OK;
		}
	
	private:
		void ConnectRedisServer(){
			redisInterface = new redis_interface_sync(Config::RedisProtocol(), Config::RedisUrl(), Config::RedisPort(), Config::RedisTimeout());
		}
	
		void DisconnectRedisServer(){
			delete redisInterface;
		}
	
		int TickDownScore(char* ip){
			int lastUpdate = GetTimestamp(ip);
			int now = GetCurrentUnixTimestamp();
			
			if(lastUpdate == now)
				return 0;
			
			int reduce = (now - lastUpdate) * Config::TickDown();
			DecreaseScore(ip, reduce);
			
			int newScore = GetScore(ip);
			
			if(newScore > 0){
				SetTimestamp(ip, now);
			} else {
				DelScoring(ip);
			}
			
			return newScore;
		}
	
		int GetCurrentUnixTimestamp(){
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
			return static_cast<int>(timestamp);
		}
	
		void DelScoring(char* ip){
			redisInterface->Del(REDIS_SCORE_KEY, ip);
			redisInterface->Del(REDIS_CLEANUP_TIMESTAMP_KEY, ip);
		}
	
		int GetScore(char* ip){
			char* res = redisInterface->GetByKey(REDIS_SCORE_KEY, ip);
			if(res){
				int ret = atoi(res);
				free(res);
			   	return ret;
			}
			
			IncreaseScore(ip, 0);
			SetTimestampToNow(ip);
			
			return 0;
		}
	
		int GetTimestamp(char* ip){
			char* res = redisInterface->GetByKey(REDIS_CLEANUP_TIMESTAMP_KEY, ip);
			if(res){
				int ret = atoi(res);
				free(res);
			   	return ret;
			}
			
			return GetCurrentUnixTimestamp();
		}
	
		void SetTimestampToNow(char* ip){
			int timestamp = GetCurrentUnixTimestamp();
			
			SetTimestamp(ip, timestamp);
		}
	
		void SetTimestamp(char* ip, int timestamp){
			char* redisKey = (char*)malloc(strlen(REDIS_CLEANUP_TIMESTAMP_KEY) + strlen(ip) + 1);
			
			strcpy(redisKey, REDIS_CLEANUP_TIMESTAMP_KEY);
			strcat(redisKey, ip);
			
			char* redisVal = (char*)malloc(11);
			strcpy(redisVal, std::to_string(timestamp).c_str());
			
			redisInterface->Set(redisKey, redisVal);
			free(redisKey);
			free(redisVal);
		}
	
		bool CheckIfIsBlocked(char* ip){
			if(redisInterface->GetByKey(REDIS_IS_BLOCKED_KEY, ip))
			   return true;
			return false;
		}
	
		void IncreaseScore(char* ip, int score){
			redisInterface->IncBy(REDIS_SCORE_KEY, ip, score);
		}	
	
		void DecreaseScore(char* ip, int score){
			redisInterface->DecBy(REDIS_SCORE_KEY, ip, score);
		}

		void Block(char* ip){
			char* msg = (char*)malloc(strlen("block ip: ") + strlen(ip) + 1);
			
			strcpy(msg, "block ip: ");
			strcat(msg, ip);
			Log(msg);
			free(msg);
			
			// block in redis
			char* redisKey = (char*)malloc(strlen(REDIS_IS_BLOCKED_KEY) + strlen(ip) + 1);
			
			strcpy(redisKey, REDIS_IS_BLOCKED_KEY);
			strcat(redisKey, ip);
			
			char* redisValue = (char*)malloc(2);
			strcpy(redisValue, "1");
			
			redisInterface->SetWithExpire(redisKey, redisValue, Config::GetBlockTime());
			
			free(redisValue);
			free(redisKey);
			
			// perform user defined block commands
			char* format = Config::GetBlockCommandFormat();
			
			if(format != NULL){
				char* cmd = (char*)malloc(strlen(format) + strlen(ip) - 1);
				sprintf(cmd, format, ip);
				
				/* begin log */
				char* logEntry = (char*)malloc(strlen("execute cmd :") + strlen(cmd) + 1);
				strcpy(logEntry, "execute cmd :");
				strcat(logEntry, cmd);
				
				Log(logEntry);
				free(logEntry);
				/* end log */
				
				if(system(cmd));
				free(cmd);
				
			}
		}

		void Unblock(char* ip){
			char* msg = (char*)malloc(strlen("unblock ip: ") + strlen(ip) + 1);
			
			strcpy(msg, "unblock ip: ");
			strcat(msg, ip);
			Log(msg);
			free(msg);
			
			char* redisKey = (char*)malloc(strlen(REDIS_IS_BLOCKED_KEY) + strlen(ip) + 1);
			
			strcpy(redisKey, REDIS_IS_BLOCKED_KEY);
			strcat(redisKey, ip);
			
			redisInterface->Del(redisKey);
			
			free(redisKey);
			
			// perform user defined unblock commands
			char* format = Config::GetUnblockCommandFormat();
			
			if(format != NULL){
				char* cmd = (char*)malloc(strlen(format) + strlen(ip) - 1);
				sprintf(cmd, format, ip);
				
				/* begin log */
				char* logEntry = (char*)malloc(strlen("execute cmd :") + strlen(cmd) + 1);
				strcpy(logEntry, "execute cmd :");
				strcat(logEntry, cmd);
				
				Log(logEntry);
				free(logEntry);
				/* end log */
				
				system(cmd);	
				free(cmd);
				
			}
		}
	
		static void LoadConfig(){
			Config::Parse(Config::ConfigPath());
			Config::Parse(Config::ConfigPathLocal());
		}
	
		redis_interface_sync* redisInterface;
};
