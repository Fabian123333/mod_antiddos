// std::regex
#include <regex>

#include "char_list.cpp"

// request_rec
#include <httpd.h>

// Anti DDoS Filter Class, conains individual Filters

class Filter{
	public:
		// Getter definition
		char* GetUserAgent(){
			return userAgent;
		}
		char* GetRefeer(){
			return refeer;
		}
		CharList GetDomains(){
			return domains;
		}
		char* GetRequest(){
			return request;
		}
		char* GetMethod(){
			return method;
		}
		char* GetContent(){ 
			return content;
		}
		int GetStatusCode(){
			return statusCode;
		}
		int GetScore(){
			return score;	
		}
		bool IsUseRegex(){
			return useRegex;
		}
		bool ApplyForBots(){
			return applyForBots;
		}
		bool ApplyForCDN(){
			return applyForCDN;
		}
		bool ApplyForAssets(){
			return applyForAssets;
		}
	
		// Setter definition	
		void SetUserAgent(char* value){
			userAgent = (char*)malloc(strlen(value) + 1);
			strcpy(userAgent, value);
		}
	
		void AddDomain(char* value){
			domains.Add(value);
		}	
	
		void SetRefeer(char* value){
			refeer = (char*)malloc(strlen(value) + 1);
			strcpy(refeer, value);			
		}	
	
		void SetRequest(char* value){
			request = (char*)malloc(strlen(value) + 1);
			strcpy(request, value);			
		}
	
		void SetMethod(char* value){
			method = toLowerCase(value);
		}
	
		void SetContent(const char* c){
			content = (char*)malloc(strlen(c));
			strcpy(content, c);
		}
	
		void SetStatusCode(int value){
			statusCode = value;	
		}	
		void SetScore(int value){
			score = value;	
		}
		void SetUseRegex(bool value){
			useRegex = value;	
		}
		void SetApplyForBots(bool value){
			applyForBots = value;	
		}
		void SetApplyForCDN(bool value){
			applyForCDN = value;	
		}
		void SetApplyForAssets(bool value){
			applyForAssets = value;	
		}
	
		bool IsReady(){
			return isReady;	
		}
		void SetReady(){
			isReady = true;	
		}
	
		bool UrlIsAsset(const char* url){
			std::regex pattern(".*\\.(jpg|jpeg|png|gif|svg|css|js|ttf|otf|woff|woff2)(\\?.*)*$");
			return std::regex_match(url, pattern);
		}
	
		bool UserAgentIsBot(const char* ua){
			std::regex botPattern("bot|crawler|spider|yahoo|google|bing", std::regex_constants::icase);
			return std::regex_search(ua, botPattern);
		}
	
		int GetScore(request_rec *r, bool postRequest = false){
			if(!ApplyForAssets()){
				if(UrlIsAsset(r->unparsed_uri)){
//					std::cerr << "skipped asset request on " << r->unparsed_uri << "\n";
					return 0;
				}
			}
			
			if(!ApplyForBots()){
				const apr_array_header_t    *fields;
    			int                         i;
    			apr_table_entry_t           *e = 0;
				
   			    fields = apr_table_elts(r->headers_in);
    			e = (apr_table_entry_t *) fields->elts;
				
    			for(i = 0; i < fields->nelts; i++) {
					char* headerlowercase = toLowerCase(e[i].key);
														
    				if(e[i].key && strcmp(headerlowercase, "user-agent") == 0)
    			    	if(UserAgentIsBot(e[i].val)){
//							std::cerr << "skipped bot request on " << r->unparsed_uri << "\n";
							free(headerlowercase);
							return 0;
						}
					
					free(headerlowercase);
    			}
			}
			
			if(GetMethod() != NULL){
				char * m = toLowerCase(r->method);
				if(!(strcmp(method, m)) == 0){
//					std::cerr << "skipped filter with 'wrong' method " << r->method << " filter would be " << method << "\n";
					free(m);
					return 0;
				}
				free(m);
			}
			
			if(GetDomains().Count() != 0){
				if(!GetDomains().Contains(r->hostname)){
//					std::cerr << "skipped filter with 'wrong' domain " << r->hostname << " domain would be " << GetDomain() << "\n";
					return 0;
				}
					
			}
			
			if(GetRequest() != NULL){
				if(!CompareValues(r->unparsed_uri, GetRequest(), IsUseRegex())){
//					std::cerr << "skipped request does not match\n";
					return 0;
				}
			}
			
			if(GetRefeer() != NULL){
				const apr_array_header_t    *fields;
    			int                         i;
    			apr_table_entry_t           *e = 0;
				
   			    fields = apr_table_elts(r->headers_in);
    			e = (apr_table_entry_t *) fields->elts;
				
    			for(int i = 0; i < fields->nelts; i++) {
					char* headerlowercase = toLowerCase(e[i].key);
														
    				if(e[i].key && strcmp(headerlowercase, "referer") == 0)
    			    	if(!CompareValues(e[i].val, GetRefeer(), IsUseRegex())){
//							std::cerr << "skipped bot referer does not match\n";
							free(headerlowercase);
							return 0;
						}
					
					free(headerlowercase);
    			}
			}
			
			if(GetUserAgent() != NULL){
				if(!CompareValues(r->unparsed_uri, GetUserAgent(), IsUseRegex())){
//					std::cerr << "skipped request useragent does not match\n";
					return 0;
				}
			}
		
			return GetScore();
		}
	
	private:
		char* userAgent;
		char* refeer;
		char* request;
		char* method;
		char* content;
		int statusCode = 0;
		int score = 1;
		bool useRegex = false;
		bool applyForBots = true;
		bool applyForCDN = true; // TODO implement CDN policies
		bool applyForAssets = true;
	
		CharList domains;
		CharList whitelist;
	
		bool isReady;
	
		bool CompareValues(const char* target, const char* pattern, bool useRegex){
			if(useRegex	== false){
				return (strcmp(target, pattern) == 0);
			}
			
			std::regex p(pattern);
			return std::regex_match(target, p);
		}
	
		char* toLowerCase(const char* str) {
    		size_t length = std::strlen(str);
    		char* result = new char[length + 1];
    		
    		std::transform(str, str + length, result, [](unsigned char c) {
    		    return std::tolower(c);
    		});
    		
    		result[length] = '\0'; // Null-Zeichen hinzufügen
    		
    		return result;
		}

};