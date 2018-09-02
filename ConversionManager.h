#pragma once
class ConversionManager
{
public:
	static std::string ReveseHex(const std::string source);
	static std::string IntToHex(const std::string source,int len=4);
	static std::string IntToHex(const int val,int len=4);
	static void Byte2Hex(unsigned char* buffer,int bArray_len,std::string* strHex);
	static void Byte2Hex(std::vector<unsigned char>& vcCmd,int start,int bArray_len,std::string* strHex);
	static void Byte2HexReverse(std::vector<unsigned char>& vcCmd,int start,int bArray_len,std::string* strHex);
	static void Byte2Dec(std::vector<unsigned char>& vcCmd,int start,int bArray_len,std::string* strHex);
	static void HexStrToByte(const char* source, unsigned char* dest, int sourceLen);
	static std::string StringSingleToHex(std::string source);
	static std::string StringToHex(std::string source);
	static std::string GetStringRight(std::string source,int len);
	static void Split(const std::string source,char* spliter,std::vector<std::string>& vcDec);
	static std::string HexToIdentify(std::string strhex);
	static CString GetTimeString(int type=0);
private:
	ConversionManager(void);
	~ConversionManager(void);
	static int hex2int(char c);
};

