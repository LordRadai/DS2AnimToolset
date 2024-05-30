#include "mtd.h"
#include <locale.h>

namespace cfr
{
	MTD::Marker::Marker(){};

	MTD::Marker::Marker(UMEM* src)
	{
		uread(&this->value,sizeof(char),1,src);

		//realignment code
		long pos = utell(src);
		if(pos % 4 != 0)
			useek(src,4-(pos%4),SEEK_CUR);
	};

	MTD::Block::Block(){};

	MTD::Block::Block(UMEM* src)
	{
		//realignment code
		/*long pos = utell(src);
		if(pos % 4 > 0)
			useek(src,4-(pos%4),SEEK_CUR);*/

		uread(&this->zero,sizeof(int32_t),1,src);
		uread(&this->length,sizeof(int32_t),1,src);

		//int32_t end;
		//uread(&end,sizeof(int32_t),1,src);

		uread(&this->type,sizeof(int32_t),1,src);
		uread(&this->version,sizeof(int32_t),1,src);
		
		this->marker = Marker(src);
	};

	MTD::MarkedString::MarkedString(){;}

	MTD::MarkedString::MarkedString(UMEM* src)
	{
		uread(&this->length,sizeof(int32_t),1,src);
		if(this->length > 0)
		{
			this->str = (char*)malloc(this->length*sizeof(char));
			uread(&this->str[0],this->length,1,src);
		}

		this->marker = Marker(src);
	};

	MTD::Texture::Texture(UMEM* src)
	{
		this->block = Block(src);
		this->type = MarkedString(src);
		uread(&this->uvNumer,sizeof(int32_t),1,src);
		this->marker = Marker(src);
		uread(&this->shaderDataIndex,sizeof(int32_t),1,src);
		
		if(this->block.version == 5)
		{
			uread(&this->unkB0C,sizeof(int32_t),1,src);
			this->path = MarkedString(src);
			uread(&this->floatCount,sizeof(float),1,src);
			this->floats = (float*)malloc(this->floatCount * sizeof(float));
			uread(&this->floats[0],sizeof(float),this->floatCount,src);
		}
	};

	MTD::Value::Value(){};

	MTD::Value::Value(UMEM* src)
	{
		this->block = Block(src);
		uread(&this->valueCount,sizeof(int32_t),1,src);
		
		if(this->block.type == 0x1000)
		{
			this->byteValues = (int8_t*)malloc(this->valueCount * sizeof(int8_t));
			uread(&this->byteValues[0],sizeof(int8_t),this->valueCount,src);
		}
		else if(this->block.type == 0x1001)
		{
			this->intValues = (int32_t*)malloc(this->valueCount * sizeof(int32_t));
			uread(&this->intValues[0],sizeof(int32_t),this->valueCount,src);
		}
		else if(this->block.type == 0x1002)
		{
			this->floatValues = (float*)malloc(this->valueCount * sizeof(float));
			uread(&this->floatValues[0],sizeof(float),this->valueCount,src);
		}
	};

	MTD::Param::Param(UMEM* src)
	{
		this->block = Block(src);
		this->name = MarkedString(src);
		this->type = MarkedString(src);
		uread(&this->unkB00,sizeof(int32_t),1,src);
		this->value = Value(src);
		this->end = Marker(src);
		uread(&this->unkC00,sizeof(int32_t),1,src);
	};

	MTD::Lists::Lists(){};

	MTD::Lists::Lists(UMEM* src)
	{
		this->block = Block(src);
		uread(&this->unkA04,sizeof(int32_t),1,src);
		this->marker = Marker(src);
		uread(&this->paramCount,sizeof(int32_t),1,src);
		this->params = (Param*)malloc(this->paramCount * sizeof(Param));
		for(int i = 0; i < this->paramCount; i++)
			this->params[i] = Param(src);
		this->paramsEnd = Marker(src);
		uread(&this->textureCount,sizeof(int32_t),1,src);
		this->textures = (Texture*)malloc(this->textureCount * sizeof(Texture));
		for(int i = 0; i < this->textureCount; i++)
			this->textures[i] = Texture(src);
		this->end = Marker(src);
		uread(&this->unkC00,sizeof(int32_t),1,src);
	};

	MTD::Data::Data(){};

	MTD::Data::Data(UMEM* src)
	{
		this->block = Block(src);
		this->shader = MarkedString(src);
		this->description = MarkedString(src);
		uread(&this->unkB00,sizeof(int32_t),1,src);
		this->lists = Lists(src);
		this->end = Marker(src);
		uread(&this->unkC00,sizeof(int32_t),1,src);
	};

	MTD::Header::Header(){};

	MTD::Header::Header(UMEM* src)
	{
		this->block = Block(src);
		this->magic = MarkedString(src);
		uread(&this->unkB00,sizeof(int32_t),1,src);
	};

	void initMTD(MTD* f, UMEM* src);

	MTD::MTD(const char* path) : File(path)
	{
		UMEM* src;
		if (src = uopenFile(path,"rb"))
		{
			initMTD(this,src);
		}
		else
		{
			throw std::runtime_error("Failed to open file!");
		}
	}

	MTD::MTD(UMEM* src) : File(src)
	{
		useek(src,0,SEEK_SET);
		initMTD(this,src);
	};

	void initMTD(MTD* m, UMEM* src)
	{
		m->data = src;

		printf("beginning block\n");
		m->block = MTD::Block(src);
		printf("block: %i\n",m->block.length);
		m->header = MTD::Header(src);
		printf("hdr: %i\n",m->header.magic.length);
		m->headerEnd = MTD::Marker(src);
		printf("hdre: %i\n",m->headerEnd.value);
		m->mtdData = MTD::Data(src);
		printf("data p count: %i\n",m->mtdData.lists.paramCount);
		m->end = MTD::Marker(src);
		printf("end: %i\n",m->end.value);
		uread(&m->unkC00,sizeof(int32_t),1,src);
		printf("unkc00: %i\n",m->unkC00);

		useek(m->data,0,SEEK_SET);
	};

	void MTD::MarkedString::print()
	{
		printf("%s",&this->str);
	};

	char* MTD::MarkedString::toUtf8(std::map<int,int>* charMap, int* length)
	{
		return jisToUtf8(this->str,this->length,charMap,length);
	};

	void MTD::Param::print()
	{
		printf("name: ");
		this->name.print();
		printf("\n");

		printf("type: ");
		this->type.print();
		printf("\n");

		printf("value: idk\n");	
	};

	void MTD::print()
	{
		int i = 0;
		printf("params:\n");
		printf("\tname: %s\n",&this->mtdData.lists.params[i].name.str);
		//printf("\tvalue: %0",this->mtdData.lists.params[i].value.floatValues);
	};
};