#include "paramdef.h"

namespace cfr
{
	PARAMDEF::i_Header::i_Header(UMEM* src)
	{
		//read ahead to get version and endianess
		long pos = utell(src);

		int16_t tempVer = 0;
		int8_t endian = 0;

		useek(src,0x2C,SEEK_CUR);
		uread(&endian,sizeof(int8_t),1,src);

		bool bigEndian = false;
		if(endian == -1)
			bigEndian = true;
		else
			bigEndian = false;

		useek(src,1,SEEK_CUR); //0x2E
		ureade(&tempVer,sizeof(int16_t),1,src,bigEndian);

#ifdef VALIDATE_ALL
		assertMsg(
			tempVer == 101 || tempVer == 102 ||
			tempVer == 103 || tempVer == 104 ||
			tempVer == 201 || tempVer == 202,
			"PARAMDEF::i_Header tempVer returned incorrect value!\n"
		);
#endif
		useek(src,pos,SEEK_SET);

		ureade(&this->fileSize,    sizeof(int32_t),1,src,bigEndian);
		ureade(&this->headerSize,  sizeof(int16_t),1,src,bigEndian);
		ureade(&this->dataVersion, sizeof(int16_t),1,src,bigEndian);
		ureade(&this->fieldCount,  sizeof(int16_t),1,src,bigEndian);
		ureade(&this->fieldSize,   sizeof(int16_t),1,src,bigEndian);

#ifdef VALIDATE_ALL
		assertMsg(
			(tempVer == 101 && this->fieldSize == 0x8C) || 
			(tempVer == 102 && this->fieldSize == 0xAC) ||
			(tempVer == 103 && this->fieldSize == 0x6C) || 
			(tempVer == 104 && this->fieldSize == 0xB0) ||
			(tempVer == 201 && this->fieldSize == 0xD0) || 
			(tempVer == 202 && this->fieldSize == 0x68),
			"PARAMDEF::i_Header tempVer fieldSize mismatch!\n"
		);
#endif

		if(tempVer >= 202)
		{
			ureade(&this->typePad1,sizeof(int32_t),1,src,bigEndian);
			ureade(&this->paramTypeOffset,sizeof(int64_t),1,src,bigEndian);
			ureade(&this->typePad2,sizeof(int64_t),1,src,bigEndian);
			ureade(&this->typePad3,sizeof(int64_t),1,src,bigEndian);
			ureade(&this->typePad4,sizeof(int32_t),1,src,bigEndian);
		}
		else
		{
			uread(this->paramID,sizeof(char),0x20,src);
		}

		uread(&this->m_bigEndian,sizeof(char),1,src);
		uread(&this->unicode,sizeof(char),1,src);
		
		ureade(&this->formatVersion,sizeof(uint16_t),1,src,bigEndian);

		if(this->formatVersion >= 201)
		{
			ureade(&this->fieldsOffset,sizeof(int64_t),1,src,bigEndian);
		}
	};

	PARAMDEF::i_Field::i_Field(UMEM* src, PARAMDEF* parent)
	{
		bool e = parent->bigEndian;

		if(parent->i_header.formatVersion >= 202)
			ureade(&this->displayNameOffset,sizeof(int64_t),1,src,e);
		else if(parent->i_header.unicode)
			ureade(&this->displayNameW,sizeof(wchar_t),20,src,e);
		else
			uread(&this->displayName,sizeof(char),40,src);

		uread(this->displayType,  sizeof(char),8,src);
		uread(this->displayFormat,sizeof(char),8,src);

		ureade(&this->defaultValue,sizeof(float),1,src,e);
		ureade(&this->minValue,sizeof(float),1,src,e);
		ureade(&this->maxValue,sizeof(float),1,src,e);
		ureade(&this->increment,sizeof(float),1,src,e);

		ureade(&this->editFlags,sizeof(int32_t),1,src,e);

		ureade(&this->byteCount,sizeof(int32_t),1,src,e);

		if(parent->i_header.formatVersion >= 200)
			ureade(&this->descriptionOffset64,sizeof(int64_t),1,src,e);
		else
			ureade(&this->descriptionOffset32,sizeof(int32_t),1,src,e);

		if(parent->i_header.formatVersion >= 202)
			ureade(&this->internalTypeOffset,sizeof(int64_t),1,src,e);
		else
			uread(this->internalType,sizeof(char),20,src);

		if(parent->i_header.formatVersion >= 202)
			ureade(&this->internalNameOffset,sizeof(int64_t),1,src,e);
		else
			uread(this->internalName,sizeof(char),20,src);

		if(parent->i_header.formatVersion >= 104)
			ureade(&this->sortId,sizeof(int32_t),1,src,e);

		if(parent->i_header.formatVersion >= 201)
		{
			ureade(&this->unkB4,sizeof(int32_t),1,src,e);
			ureade(&this->offsetB8,sizeof(int64_t),1,src,e);
			ureade(&this->offsetC0,sizeof(int64_t),1,src,e);
			ureade(&this->offsetC8,sizeof(int64_t),1,src,e);
		}

		long pos = utell(src);

		if(this->descriptionOffset32 + this->descriptionOffset64 != 0)
		{
			useek(src,descriptionOffset32+descriptionOffset64,SEEK_SET);

			if(parent->i_header.unicode)
				this->descriptionW = readWString(src,e);
			else
				this->description = readString(src);
		}

		if(parent->i_header.formatVersion >=201)
		{
			if(this->offsetB8 != 0)
			{
				useek(src,this->offsetB8,SEEK_SET);
				this->unkB8 = readString(src);
			}
			if(this->offsetC0 != 0)
			{
				useek(src,this->offsetC0,SEEK_SET);
				this->unkC0 = readString(src);
			}
			if(this->offsetC8 != 0)
			{
				useek(src,this->offsetC8,SEEK_SET);
				this->unkC8 = readWString(src,e);
			}
		}

		if(parent->i_header.formatVersion >= 202)
		{
			useek(src,this->displayNameOffset,SEEK_SET);
			this->displayNameWStr = readWString(src,e);
			useek(src,this->internalTypeOffset,SEEK_SET);
			this->internalTypeStr = readString(src);
			useek(src,this->displayNameOffset,SEEK_SET);
			this->internalNameStr = readString(src);
		}

		useek(src,pos,SEEK_SET);
	};

	PARAMDEF::PARAMDEF(UMEM* src) : File(src)
	{
		this->i_header = i_Header(src);

		for(int i = 0; i < this->i_header.fieldCount; i++)
		{
			this->i_fields.push_back(i_Field(src,this));
		}
	};
};