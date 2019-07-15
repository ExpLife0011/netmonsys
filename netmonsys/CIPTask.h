#pragma once
#include "CTaskBase.h"
#include "ClassDefine.h"
class CIPTask :
	public CTaskBase
{
public:
	CIPTask(IP_Classify* ipClassify, NTSTATUS ntAccess);
	~CIPTask();

	virtual VOID DoTask();

protected:
	WORD wProtocol;		//Э������
	DWORD wLocalPort;	//���ض˿�
	UINT32 uLocalIp;	//����IP��ַ
	DWORD wRemotePort;	//Զ�̶˿�
	ULONG uRemoteIp;	//Զ��IP��ַ
	INT ProcessID;		//�����߽���
	NTSTATUS	Action;			//��Ȩ���
	WCHAR ProcessImage[MAX_PATH];	//����·��
};

