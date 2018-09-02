#include "StdAfx.h"
#include "ConversionManager.h"


ConversionManager::ConversionManager(void)
{
}


ConversionManager::~ConversionManager(void)
{
}

std::string ConversionManager::ReveseHex(const std::string source)
{
	std::string strhex=source;
	int n=0;
	for(int i=strlen(source.c_str())-1;i>0;i-=2)
	{
		strhex[n]=source[i-1];
		strhex[n+1]=source[i];
		n+=2;
	}

	return strhex;
}

std::string ConversionManager::IntToHex(const std::string source,int len)
{
	std::string strhex="0000";
	int val = std::atoi(source.c_str());
	unsigned char buf[2]={0};
	memcpy(buf,  &val, 2);
	ConversionManager::Byte2Hex(buf,2,&strhex);
	return strhex.substr(0,len);
}

std::string ConversionManager::IntToHex(const int val,int len)
{
	std::string strhex="0000";
	unsigned char buf[2]={0};
	memcpy(buf,  &val, 2);
	ConversionManager::Byte2Hex(buf,2,&strhex);
	return strhex.substr(0,len);
}

void ConversionManager::Byte2Hex(unsigned char* buffer,int bArray_len,std::string* strHex)
{
	*strHex="";
	for (int i=0;i<bArray_len;i++)
    {
        char hex1;
        char hex2;
        int value=buffer[i];
        int v1=value/16;
        int v2=value % 16;

        if (v1>=0&&v1<=9)
            hex1=(char)(48+v1);
        else
            hex1=(char)(55+v1);


        if (v2>=0&&v2<=9)
            hex2=(char)(48+v2);
        else
            hex2=(char)(55+v2);

         *strHex=*strHex+hex1+hex2;
    }
}

void ConversionManager::Byte2Hex(std::vector<unsigned char>& vcCmd,int start,int bArray_len,std::string* strHex)
{
	*strHex="";
	for (int i=start;i<bArray_len;i++)
    {
        char hex1;
        char hex2;
        int value=vcCmd[i];
        int v1=value/16;
        int v2=value % 16;

        if (v1>=0&&v1<=9)
            hex1=(char)(48+v1);
        else
            hex1=(char)(55+v1);


        if (v2>=0&&v2<=9)
            hex2=(char)(48+v2);
        else
            hex2=(char)(55+v2);

         *strHex=*strHex+hex1+hex2;
    }
}

void ConversionManager::Byte2HexReverse(std::vector<unsigned char>& vcCmd,int start,int bArray_len,std::string* strHex)
{
	*strHex="";
	for (int i=bArray_len-1;i>=start;i--)
    {
        char hex1;
        char hex2;
        int value=vcCmd[i];
        int v1=value/16;
        int v2=value % 16;

        if (v1>=0&&v1<=9)
            hex1=(char)(48+v1);
        else
            hex1=(char)(55+v1);


        if (v2>=0&&v2<=9)
            hex2=(char)(48+v2);
        else
            hex2=(char)(55+v2);

         *strHex=*strHex+hex1+hex2;
    }
}

void ConversionManager::Byte2Dec(std::vector<unsigned char>& vcCmd,int start,int bArray_len,std::string* strHex)
{
	*strHex="";
	long long  ret=0;
	int j=0,n=8;
	for (int i=start;i<bArray_len;i++)
	{
		ret = ret+(((unsigned int)vcCmd[i])<<(j*n));
		j++;
	}

	*strHex=std::to_string(ret);
}


//十六进制字符串转换为字节流
void ConversionManager::HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
	int len = sourceLen*2;
    unsigned char highByte, lowByte;
    for (i = 0; i<len; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i+1]);
 
        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;
 
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
 
        dest[i / 2] = (highByte << 4) | lowByte;
	}
}

//一般字符串按单个16进制字符转十六进制字符串
std::string ConversionManager::StringSingleToHex(std::string source)
{
	int len = source.length();
	std::string ret(len*2,'0');
	for(unsigned int i=0;i<source.size();i++)
    {
		std::string tmp=source.substr(i,1);
		tmp = "0" + tmp;
		ret+=tmp;
    }

	return GetStringRight(ret,len*2);
}

//一般字符串转十六进制字符串
std::string ConversionManager::StringToHex(std::string source)
{
	std::string result;
    std::string tmp;
    std::stringstream ss;
    for(unsigned int i=0;i<source.size();i++)
    {
		unsigned int n = static_cast<unsigned int>(source[i]);
        ss<<std::hex<<n<<std::endl;
        ss>>tmp;
        result+=tmp;
    }
    return result;
}

std::string ConversionManager::GetStringRight(std::string source,int len)
{
	int strlen = source.length();
	return source.substr(strlen-len,len);
}

void ConversionManager::Split(const std::string source,char* spliter,std::vector<std::string>& vcDec)
{
	char *str = const_cast<char*>(source.c_str());
	char *pch;
	pch = strtok(str, spliter );
	while( pch != NULL )
	{
		std::string s(pch);
		vcDec.push_back(s);
		pch = strtok( NULL, spliter );
	}
}

CString ConversionManager::GetTimeString(int type)
{
	CTime nowtime;
	nowtime = CTime::GetCurrentTime();
	CString   strmsg;
	CString   strtime;

	if (type==0)
	{
		strtime.Format("%.2d-%.2d-%.2d %.2d:%.2d:%.2d：", nowtime.GetYear(), nowtime.GetMonth(), nowtime.GetDay(),
			nowtime.GetHour(), nowtime.GetMinute(), nowtime.GetSecond());
	}else if(type==1)
	{
		strtime.Format("%.2d%.2d%.2d", nowtime.GetYear(), nowtime.GetMonth(), nowtime.GetDay());
	}else if(type==2)
	{
		strtime.Format("%.2d%.2d%.2d%.2d", nowtime.GetYear(), nowtime.GetMonth(), nowtime.GetDay(),nowtime.GetHour());
	}else if(type==3)
	{
		strtime.Format("%.2d%.2d%.2d%.2d%.2d%.2d%.02d", nowtime.GetSecond(),nowtime.GetMinute(),nowtime.GetHour(),nowtime.GetDayOfWeek()-1, nowtime.GetDay(), nowtime.GetMonth(),nowtime.GetYear()-2000);
	}else if(type==4)
	{
		strtime.Format("%.2d", nowtime.GetYear());
	}
	

	return strtime;
}


int ConversionManager::hex2int(char c)
{
	if ((c >= 'A') && (c <= 'Z'))
	{
		return c - 'A' + 10;
	}
	else if ((c >= 'a') && (c <= 'z'))
	{
		return c - 'a' + 10;
	}
	else if ((c >= '0') && (c <= '9'))
	{
		return c - '0';
	}

	return 0;
}


std::string ConversionManager::HexToIdentify(std::string strhex)
{
	std::string head = strhex.substr(0,1);
	if(head=="8")
	{
		strhex = "0" + strhex.substr(1,strhex.length()-1);
	}

	char *offset;
	_ULonglong dec = _strtoui64(strhex.c_str(),&offset,16);

	std::string ret = std::to_string(dec);
	if(head=="8"){
		ret = ret.substr(0,ret.length()-1)+"X";
	}
	return ret;

}