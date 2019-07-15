#pragma once

#include <kutil/string.h>
using namespace msddk;
struct IP_Classify: public NonPagedObject
{
public:
	IP_Classify(const FWPS_INCOMING_VALUES* inFixedValues, const FWPS_INCOMING_METADATA_VALUES* inMetaValues);
public:
	WORD wProtocol;	//Э������
	
	WORD wLocalPort;	//���ض˿�
	UINT32 uLocalIp;	//����IP��ַ

	WORD wRemotePort;	//Զ�̶˿�
	ULONG uRemoteIp;	//Զ��IP��ַ

	INT ProcessID;		//�����߽���
	WCHAR ProcessImage[MAX_PATH];	//����·��
	
}; 
