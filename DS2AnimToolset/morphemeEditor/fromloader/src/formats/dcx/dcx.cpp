#include "dcx.h"

namespace cfr
{
	enum _DCX_TYPE_
	{
		unkown,
		none,
		zlib,
		dcp_edge,
		dcp_dflt,
		dcx_edge,
		dcx_dflt_10000_24_9,
		dcx_dflt_10000_44_9,
		dcx_dflt_11000_44_8,
		dcx_dflt_11000_44_9,
		dcx_krak
	};

	struct DCX_Header
	{
		char magic[4]; // DCX\0
		int32_t unk04; // 0x100000 or 0x11000
		int32_t dcsOffset; // 0x18
		int32_t dcpOffset; // 0x24
		int32_t unk10;     // 0x24 or 0x44
		int32_t unk14;
		char dcs[4]; // DCS\0
		uint32_t uncompressedSize;
		uint32_t   compressedSize;
		char dcp[4]; // DCP\0
		char format[4]; // DFLT, EDGE, KRAK
		int32_t unk2C; // 0x20
		int8_t unk30;  // 6, 8, 9
		int8_t unk31;  // 0
		int8_t unk32;  // 0
		int8_t unk33;  // 0
		int32_t unk34; // 0 or 0x10000
		int32_t unk38; // 0
		int32_t unk3C; // 0
		int32_t unk40;
		char dca[4]; // DCA\0
		int32_t dcaSize; //from DCA to dca end
	};

	struct DCX_Header_Edge
	{
		char egdt[4]; // EgdT
		int32_t unk50; // 0x10100
		int32_t unk54; // 0x24
		int32_t unk58; // 0x10
		int32_t unk5C; // 0x10000
		int32_t lastBlockUncompressedSize;
		int32_t egdtSize; //from before EgdT to dca end
		int32_t blockCount;
		int32_t unk6C; // 0x100000
	};

	struct DCX_Block
	{
		int32_t unk00; // 0
		int32_t dataOffset;
		int32_t dataLength;
		int32_t unk0C; // 1
	};

	int decompress_zlib(UMEM* src, UMEM* dest);

	int decompress_dcp_edge(UMEM* dest);

	int decompress_dcp_dflt(UMEM* src, UMEM* dest);

	int decompress_dcx_edge(UMEM* dest, _DCX_TYPE_ type);

	int decompress_dcx_dflt(UMEM* src, UMEM* dest, _DCX_TYPE_ type);

	int decompress_dcx_krak(UMEM* dest);

	UMEM* openDCX(const char* path)
	{
		//TODO: is this a memory leak?
		return openDCX(uopenFile(path,"rb"));
	};

	UMEM* openDCX(UMEM* src)
	{
		DCX_Header* h = (DCX_Header*)malloc(sizeof(DCX_Header));
		uread(h,sizeof(DCX_Header),1,src);

#ifdef VALIDATE_ALL
		assertMsg(strncmp(h->magic,"DCX\0",4) == 1,"DCX wrong magic bytes!\n");
#endif

		h->unk04 = switchEndian(h->unk04,true);
		h->unk10 = switchEndian(h->unk10,true);
		h->uncompressedSize = switchEndian(h->uncompressedSize,true);
		h->compressedSize   = switchEndian(h->compressedSize  ,true);
		h->unk30 = switchEndian(h->unk30,true);

		if(memcmp(h->format,"EDGE",4) == 0)
			throw std::runtime_error("EDGE compression not supported!\n");

		//call correct decompression util and copy output into dest
		char* data = (char*)malloc(h->uncompressedSize);
		UMEM* dst  = uopenMem(data,h->uncompressedSize);
		useek(dst,0,SEEK_SET);
		_DCX_TYPE_ type;

		if(memcmp(h->magic,"DCP\0",4) == 0)
		{
			if(memcmp(h->format,"DFLT",4) == 0)
			{
				type = dcp_dflt;
				decompress_dcp_dflt(src,dst);
			}
			else if(memcmp(h->format,"EDGE",4) == 0)
			{
				type = dcp_edge;
			}
			throw std::runtime_error("No DCP compression method format found!\n");
		}
		else if(memcmp(h->magic,"DCX",3) == 0)
		{
			if(memcmp(h->format,"DFLT",4) == 0)
			{
				if(h->unk10 == 0x24)
				{
					type = dcx_dflt_10000_24_9;
					decompress_dcx_dflt(src,dst,type);
				}
				else if(h->unk10 == 0x44)
				{
					if(h->unk04 == 0x10000)
					{
						type = dcx_dflt_10000_44_9;
						decompress_dcx_dflt(src,dst,type);
					}
					else if(h->unk04 == 0x11000)
					{
						if(h->unk30 == 0x8000000)
						{
							type = dcx_dflt_11000_44_8;
							decompress_dcx_dflt(src,dst,type);
						}
						else if(h->unk30 == 0x9000000)
						{
							type = dcx_dflt_11000_44_9;
							decompress_dcx_dflt(src,dst,type);
						}
					}
				}
				else
				{
					printf("unk04:%i\n",h->unk04);
					printf("unk10:%i\n",h->unk10);
					printf("unk30:%i\n",h->unk30);
					throw std::runtime_error("Failed to parse DCX format!\n");
				}
				//decompress_dcx_edge(file,type);
			}
			else if(memcmp(h->format,"EDGE",4) == 0)
			{
				type = dcx_edge;
				decompress_dcx_edge(dst,type);
			}
			else if(memcmp(h->format,"KRAK",4) == 0)
			{
				decompress_dcx_krak(dst);
			}
		}
		else
		{
			decompress_zlib(src,dst);
		}

		//is closing the src good?
		//uclose(src);
		//delete h; //probably good to do this?

		return dst;
	};

	int decompress_zlib(UMEM* src, UMEM* dest)
	{
		return inf(src,dest);
	};

	int decompress_dcp_edge(UMEM* dest)
	{
		throw std::runtime_error("DCP EDGE not implemented!\n");
	};

	int decompress_dcp_dflt(UMEM* src, UMEM* dest)
	{
		throw std::runtime_error("DCP DFLT not implemented!\n");
	};

	int decompress_dcx_edge(UMEM* dest, _DCX_TYPE_ type)
	{
		throw std::runtime_error("DCX EDGE not implemented!\n");
	};

	int decompress_dcx_dflt(UMEM* src, UMEM* dest, _DCX_TYPE_ type)
	{
		//useek(src,4,SEEK_CUR); //skip to beginning of dlft data
		int ret = inf(src,dest);

		if(uerror(src))
			throw std::runtime_error("Source file errored during inflation!\n");

		if(ret != 0)
		{
			throw std::runtime_error("Failed to inflate DCX DFLT file!\n");
		}

		return 0;
	};

	int decompress_dcx_krak(UMEM* dest)
	{
		throw std::runtime_error("DCX KRAK not implemented!\n");
	};
};