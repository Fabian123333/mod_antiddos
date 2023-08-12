// std::cout
#include <iostream>
#include <cstdlib>

// pow
#include <cmath>

// dynamic list class

class CharList{
	public:
		void Add(char* c){
			// if currently no entry
			if(content == NULL){
				content = (char*)malloc(strlen(c) + 1);
				strcpy(content, c);
				return;
			}
			if(nextEntry == NULL){
				nextEntry = new CharList();
			}
			
			nextEntry->Add(c);
		}
		
		bool ContainsIP(char* ip){
			if(content == NULL){
				return false;
			} else {
				if(ContainsChar(content, '/'))
					if(ContainsChar(content, '.'))
						if(IPv4CIDRMatch(ip, content))
							return true;
					else
						{/* @todo implement IPv6 CIDR */}
				else
					if(strcmp(content, ip) == 0)
						return true;
			}
			
			if(nextEntry)
				return nextEntry->ContainsIP(ip);
			return false;
		}
	
		bool Contains(char* c){
			if(content == NULL){
				return false;	
			}
			if(strcmp(c, content) == 0){
				return true;	
			}
			if(nextEntry == NULL){
				return false;	
			}
			return nextEntry->Contains(c);
		}
	
	private:
		CharList* nextEntry;
		char* content;
		
		bool ContainsChar(const char* str, char c) {
			while (*str != '\0') {
				if (*str == c) {
					return true;
				}
				str++;
			}
			return false;
		}
		
		void Split(const char *s, char delimiter, char **tokens, int maxTokens) {
			int tokenCount = 0;
			while (*s && tokenCount < maxTokens) {
				while (*s && *s == delimiter) {
					++s;
				}
				if (*s) {
					const char *start = s;
					while (*s && *s != delimiter) {
						++s;
					}
					int tokenLength = s - start;
					tokens[tokenCount] = (char *)malloc(tokenLength + 1);
					strncpy(tokens[tokenCount], start, tokenLength);
					tokens[tokenCount][tokenLength] = '\0';
					++tokenCount;
				}
			}
		}
		
		std::bitset<32> IPv4ToBytes(const char* ip){
			unsigned short a, b, c, d;
			sscanf(ip, "%hu.%hu.%hu.%hu", &a, &b, &c, &d);
			
			std::bitset<8> b1(a);
			std::bitset<8> b2(b);
			std::bitset<8> b3(c);
			std::bitset<8> b4(d);
		
			std::bitset<32> output_bitset(b1.to_string() + b2.to_string() + b3.to_string() + b4.to_string());
			
			return output_bitset;
		}
		
		bool IPv4CIDRMatch(const char* ip, const char* cidr){			
			char *cidrParts[2];
			Split(cidr, '/', cidrParts, 2);
			
			std::bitset<32> ip_bitset = IPv4ToBytes(ip);
			std::bitset<32> cidr_bitset = IPv4ToBytes(cidrParts[0]);
			
			int netmask = atoi(cidrParts[1]);
			std::bitset<32> netmask_bitset(pow(2, 32 - netmask) - 1);
			
			std::bitset<32> network_ip = ip_bitset | netmask_bitset;
			std::bitset<32> network_cidr = cidr_bitset | netmask_bitset;
			
			return network_ip == network_cidr;
		}
};