// std::cout
#include <iostream>
#include <cstdlib>

// uint16_t
#include <cstdint>

// pow
#include <cmath>

// dynamic list class

class CharList{
	public:
		int Count(){
			return count;	
		}
	
		void Add(char* c){
			count++;
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
						if(IPv6CIDRMatch(ip, content))
							return true;
				else
					if(strcmp(content, ip) == 0)
						return true;
			}
			
			if(nextEntry)
				return nextEntry->ContainsIP(ip);
			return false;
		}
	
		bool Contains(const char* c){
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
		int count = 0;
		
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
		
		std::bitset<128> IPv6ToBytes(const char* ip){
			char* currentBuffer = (char*)malloc(5);
			strcpy(currentBuffer, "");
			char lastCharacter;
			
			std::bitset<16> splitBitset[8];
			int splitid = 0;
			
			for(int i = 0;; i++){
				if(ip[i] == ':' || ip[i] == '\0'){
					// add leading zeros
					while(strlen(currentBuffer) != 4){
						char* newBuffer = (char*)malloc(strlen(currentBuffer) + 1);
						strcpy(newBuffer, "0");
						strcat(newBuffer, currentBuffer);
						free(currentBuffer);
						currentBuffer = newBuffer;
					}
				
					if(lastCharacter == ':'){
						int remainingParts = 0;
						
						for(int v = i; ip[v] != '\0'; v++){
							if(ip[v] == ':')
								remainingParts++;
						}
						
						int skips = 7 - remainingParts - splitid;
							
						for(int v = 0; v < skips; v++){
							uint16_t part_as_int16 = 0;
							splitBitset[splitid++] = part_as_int16;
					
							continue;
						}
					}
		
					char* endPtr;
					uint16_t part_as_int16 = static_cast<uint16_t>(std::strtoul(currentBuffer, &endPtr, 16));
					
					splitBitset[splitid++] = part_as_int16;
					
					free(currentBuffer);
					
					if(ip[i] == '\0')
						break;
					
					currentBuffer = (char*)malloc(5);
					strcpy(currentBuffer, "");
				}
				else{
					char newChar[2];
					newChar[0] = ip[i];
					newChar[1] = '\0';
					strcat(currentBuffer, &newChar[0]);
				}
				lastCharacter = ip[i];
			}
			
			std::bitset<128> out_bitset(splitBitset[0].to_string() + splitBitset[1].to_string() + splitBitset[2].to_string() + splitBitset[3].to_string() + splitBitset[4].to_string() + splitBitset[5].to_string() + splitBitset[6].to_string() + splitBitset[7].to_string());
			
			return out_bitset;
		}

		bool IPv6CIDRMatch(const char* ip, const char* cidr){	
			char *cidrParts[2];
			Split(cidr, '/', cidrParts, 2);
			
			std::bitset<128> ip_bitset = IPv6ToBytes(ip);
			std::bitset<128> cidr_bitset = IPv6ToBytes(cidrParts[0]);
			
			int netmask = atoi(cidrParts[1]);
			std::bitset<128> netmask_bitset(pow(2, 128 - netmask) - 1);
			
			std::bitset<128> network_ip = ip_bitset | netmask_bitset;
			std::bitset<128> network_cidr = cidr_bitset | netmask_bitset;
			
			return network_ip == network_cidr;
		}
};
