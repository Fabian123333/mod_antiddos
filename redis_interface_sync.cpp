// AF_INET
#include <netinet/in.h>

// redisContext etc.
#include <hiredis/hiredis.h>

class redis_interface_sync{
	public:
		redis_interface_sync(int redisProtocol, char* redisUri, int redisPort, int t) {
			protocol = redisProtocol;
			port = redisPort;
			timeout = t;
			
			uri = (char*)malloc(strlen(redisUri) + 1);
			strcpy(uri, redisUri);
			
			connect();
		}
	
		~redis_interface_sync(){
			redisFree(redis_context);	
		}
	
		void Set(char* key, char* val){
			redisCommand(redis_context, "SET %s %s", key, val);
			return;
		}
	
		void Del(char* key){
			redisCommand(redis_context, "DEL %s", key);
			return;
		}	
	
		void Inc(char* key){
			redisCommand(redis_context, "INCR %s", key);
			return;
		}
		
		void Inc(char* key1, char* key2){
			char* search = (char*)malloc(strlen(key1) + strlen(key2) + 1);
			
			strcpy(search, key1);
			strcat(search, key2);	
			
			Inc(search);
			free(search);
		}
	
		void IncBy(char* key, int val){
			redisCommand(redis_context, "INCRBY %s %i", key, val);
			return;
		}
	
		void IncBy(char* key1, char* key2, int val){
			char* search = (char*)malloc(strlen(key1) + strlen(key2) + 1);
			
			strcpy(search, key1);
			strcat(search, key2);	
			
			IncBy(search, val);
			free(search);
		}	
	
		void Dec(char* key){
			redisCommand(redis_context, "DECR %s", key);
			return;
		}
	
		void Dec(char* key1, char* key2){
			char* search = (char*)malloc(strlen(key1) + strlen(key2) + 1);
			
			strcpy(search, key1);
			strcat(search, key2);	
			
			Dec(search);
			free(search);
		}	
	
		void DecBy(char* key, int val){
			redisCommand(redis_context, "DECRBY %s %i", key, val);
			return;
		}
	
		void DecBy(char* key1, char* key2, int val){
			char* search = (char*)malloc(strlen(key1) + strlen(key2) + 1);
			
			strcpy(search, key1);
			strcat(search, key2);	
			
			DecBy(search, val);
			free(search);
		}	
	
		void Del(char* key1, char* key2){
			char* search = (char*)malloc(strlen(key1) + strlen(key2) + 1);
			
			strcpy(search, key1);
			strcat(search, key2);	
			
			Del(search);
			free(search);
		}
	
		void SetExpire(char* key, int timeout){
			redisCommand(redis_context, "EXPIRE %s %i", key, timeout);
			return;
		}
	
		void SetWithExpire(char* key, char* val, int timeout){
			Set(key,val);
			SetExpire(key, timeout);
		}
	
		char* GetByKey(char * key){
			redisReply *reply;
			reply = (redisReply*)redisCommand(redis_context, "GET %s", key);
	
    		if(!reply->str){
    			freeReplyObject(reply);
				return NULL;
			}
			
			char* returnVal = (char*)malloc(strlen(reply->str) + 1);
			strcpy(returnVal, reply->str);
			freeReplyObject(reply);
			return returnVal;
		}
	
		char* GetByKey(char * key1, char * key2){
			char* search = (char*)malloc(strlen(key1) + strlen(key2) + 1);
			
			strcpy(search, key1);
			strcat(search, key2);
			
			char* returnVal = GetByKey(search);
			
			free(search);
			return returnVal;
		}
	
		timeval GetTimeout(){
			int sec = floor(timeout);
			struct timeval t = {sec, timeout - sec};
			return t;
		}
	
		int GetPort(){
			return port;	
		}
	
		int GetProtocol(){
			return protocol;
		}
	
		char* GetURI(){
			return uri;	
		}
	
		bool IsConnected(){
			return isConnected;
		}
	
	private:
		bool isConnected = false;
		int protocol;
		int port;
		int timeout;
		char* uri;
		redisContext * redis_context;
	
		bool connect(){
			if(IsConnected()){
				return false;	
			}
			
			redisReply *reply;
			
			if(GetProtocol() == AF_INET){
				// TCP IP connect
				redis_context = redisConnectWithTimeout(GetURI(), GetPort(), GetTimeout());
			} else {
				// Socket Connection
				redis_context = redisConnectUnixWithTimeout(GetURI(), GetTimeout());
			}
			
			if ( redis_context == NULL || redis_context->err ) {
				if (redis_context) {		
					redisFree(redis_context);
					return false;
				}
				else
				{
					return false;
				}
			}
			
			return true;
		}
};