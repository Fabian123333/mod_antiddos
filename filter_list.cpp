#include <cstdlib>
#include <cstring>

#include "filter.cpp"

// dynamic list class

class FilterList{
	public:
		void Add(Filter f){
			count++;
			// if currently no entry
			if(!content.IsReady()){
				content = f;
				content.SetReady();
				return;
			}
			if(nextEntry == NULL){
				nextEntry = new FilterList();
			}

			nextEntry->Add(f);
		}

		Filter Get(int id){
			if(id == 0)
				return content;
			return nextEntry->Get(--id);
		}
	
		int Count(){
			return count;
		}
	
	private:
		int count = 0;
		FilterList* nextEntry;
		Filter content;
};