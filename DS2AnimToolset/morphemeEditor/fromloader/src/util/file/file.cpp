#include "file.h"

namespace cfr
{
	File::File(const char* path)
	{
		this->data = uopenFile(path,"rb");
	};

	File::File(UMEM* src)
	{
		this->data = src;
	};


	int File::close()
	{
		if(this->children.size() > 0)
			return this->children.size();
		else
			delete this;
			return 0;
	};

	int File::forceClose()
	{
		int k = 0; //kills

		if(this->children.size() > 0)
		{
			for(auto vk : this->children)
			{
				if(vk.second->children.size() > 0)
					k += vk.second->forceClose();

				//orphan the child
				vk.second->parent = nullptr;

				vk.second->close();
				this->children.erase(this->children.find(vk.first));
				
				k += 1;
			}
			
			delete this;
			return k;
		}
		else
		{
			delete this;
			return 0;
		}
	};

	File::~File()
	{
		if(this->parent != nullptr)
			this->parent->children.erase(this->parent->children.find(this->uid));
		uclose(this->data);
	};
};