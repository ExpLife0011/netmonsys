#include "stdafx.h"
#include "CDnsDomainParse.h"
#include "WfpHelp.h"

CDnsDomainParse::CDnsDomainParse()
{
}


CDnsDomainParse::~CDnsDomainParse()
{
}

NTSTATUS CDnsDomainParse::DecodeDotStr(char *szEncodedStr, USHORT *pusEncodedStrLen, char *szDotStr, USHORT nDotStrSize, char *szPacketStartPos)
{
	if (szEncodedStr == NULL || pusEncodedStrLen == NULL || szDotStr == NULL)
	{
		return FALSE;
	}

	char *pDecodePos = szEncodedStr;
	USHORT usPlainStrLen = 0;
	BYTE nLabelDataLen = 0;
	*pusEncodedStrLen = 0;

	while ((nLabelDataLen = *pDecodePos) != 0x00)
	{
		if ((nLabelDataLen & 0xc0) == 0) //��ͨ��ʽ��LabelDataLen + Label
		{
			if (usPlainStrLen + nLabelDataLen + 1 > nDotStrSize)
			{
				return FALSE;
			}
			memcpy(szDotStr + usPlainStrLen, pDecodePos + 1, nLabelDataLen);
			memcpy(szDotStr + usPlainStrLen + nLabelDataLen, ".", 1);
			pDecodePos += (nLabelDataLen + 1);
			usPlainStrLen += (nLabelDataLen + 1);
			*pusEncodedStrLen += (nLabelDataLen + 1);
		}
		else //��Ϣѹ����ʽ��11000000 00000000�������ֽڣ�ǰ2λΪ��ת��־����14λΪ��ת��ƫ��
		{
			if (szPacketStartPos == NULL)
			{
				return FALSE;
			}
			USHORT usJumpPos = s_ntohs(*(USHORT*)(pDecodePos)) & 0x3fff;
			USHORT nEncodeStrLen = 0;
			if (!DecodeDotStr(szPacketStartPos + usJumpPos, &nEncodeStrLen, szDotStr + usPlainStrLen, nDotStrSize - usPlainStrLen, szPacketStartPos))
			{
				return FALSE;
			}
			else
			{
				*pusEncodedStrLen += 2;
				return TRUE;
			}
		}
	}

	szDotStr[usPlainStrLen - 1] = '\0';
	*pusEncodedStrLen += 1;

	return TRUE;
}
