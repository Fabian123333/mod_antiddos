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
				std::cout << "added entry: " << content << "\n";
				return;
			}
			if(nextEntry == NULL){
				nextEntry = new CharList();
			}
			
			nextEntry->Add(c);
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
};