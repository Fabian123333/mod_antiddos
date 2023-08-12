// std::cout
#include <iostream>
#include <cstdlib>

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
};