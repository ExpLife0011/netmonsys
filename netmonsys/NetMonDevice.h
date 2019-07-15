#pragma once
#define NDIS60 1
#include <kframe/unknown_device.h>
#include <kutil/map.h>
#include <kutil/vector.h>
#include <ksync/spinlock.h>
#include <ksync/semaphore.h>
#include "CalloutImpl.h"
#include "CFlowContext.h"
#include "ClassDefine.h"
#include "CTaskCenter.h"
using namespace msddk;

/*NonPagedObject ���ںܶ�ص�������������Ҫ������ʼ������ҳ�ڴ���*/
class CNetMonDevice : public CUnknownDevcie , public NonPagedObject
{
public:
	CNetMonDevice();
	~CNetMonDevice();

protected:
	virtual NTSTATUS OnAfterCreate();
	virtual NTSTATUS OnBeforeDelete();
	virtual LPCWSTR GetDeviceLinkName();
	virtual LPCWSTR GetDeviceName();


protected:
	BEGIN_DISPATCH_ROUTINE
		BEGIN_DISPATCH_IO_CONTROL
			IO_CONTROL_BUFFERED(IOCT_ADD_WHITE_RULE, IoCtrlAddWhiteRule)
			IO_CONTROL_BUFFERED(IOCT_CLEAR_WHITE_RULE, IoCtrlClearWhiteRule)

			IO_CONTROL_BUFFERED(IOCT_ADD_IP_RULE, IoCtrlAddIpRule)
			IO_CONTROL_BUFFERED(IOCT_CLEAR_IP_RULE, IoCtrlClearIpRule)

			IO_CONTROL_BUFFERED(IOCT_ADD_DOMAIN_RULE, IoCtrlAddDomainRule)
			IO_CONTROL_BUFFERED(IOCT_CLEAR_DOMAIN_RULE, IoCtrlClearDomainRule)
		END_DISPATCH_IO_CONTROL
	END_DISPATCH_ROUTINE;

	NTSTATUS IoCtrlAddWhiteRule( ULONG iocode, LPVOID inBuffer, ULONG inBufferLength, LPVOID outBuffer, ULONG outBufferLenght);
	NTSTATUS IoCtrlClearWhiteRule(ULONG iocode, LPVOID inBuffer, ULONG inBufferLength, LPVOID outBuffer, ULONG outBufferLenght);;
	
	NTSTATUS IoCtrlAddIpRule(ULONG iocode, LPVOID inBuffer, ULONG inBufferLength, LPVOID outBuffer, ULONG outBufferLenght);
	NTSTATUS IoCtrlClearIpRule(ULONG iocode, LPVOID inBuffer, ULONG inBufferLength, LPVOID outBuffer, ULONG outBufferLenght);
	
	NTSTATUS IoCtrlAddDomainRule(ULONG iocode, LPVOID inBuffer, ULONG inBufferLength, LPVOID outBuffer, ULONG outBufferLenght);
	NTSTATUS IoCtrlClearDomainRule(ULONG iocode, LPVOID inBuffer, ULONG inBufferLength, LPVOID outBuffer, ULONG outBufferLenght);;

private:
	NTSTATUS WfpCalloutRegister();
	NTSTATUS WfpCalloutUnRegister();

private:
	VOID ALE_AUTH_CONNECT_CALLOUT_V4_CLASSIFYFN(
		IN const FWPS_INCOMING_VALUES* inFixedValues,
		IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
		IN VOID* layerData,
		IN const void* classifyContext,
		IN const FWPS_FILTER* filter,
		IN UINT64 flowContext,
		OUT FWPS_CLASSIFY_OUT* classifyOut);
	
public://������ʵ�ֲ��֣���������IRQL����DISPATCH_LEVEL�������У����Խ�ֹʹ�÷�ҳ�ڴ��PASSIVE_LEVEL�ϵ�API
	NTSTATUS FilterIpRules(IP_Classify* lpParam);
	NTSTATUS FilterDnsRules(LPCSTR szDomain, IP_Classify* ipClassify);

	//ȥƥ������������ƥ�����˷��سɹ���
	NTSTATUS FilterWhiteRules(IP_Classify* ipClassify);

private:
	//��¼��������
	VOID LogIpConnect( IP_Classify* ipClassify, NTSTATUS ntAccess); 

	//��¼������������
	VOID LogDnsQuestion(LPCSTR szDomain, IP_Classify* ipClassify, NTSTATUS ntAccess);

	struct WHITE_RULE
	{
		CKeStringW Process;
	};

	struct IP_PROT_RULE
	{
		DWORD RemoteIp;
		WORD  RemotePort;
	};

	struct DOMAIN_RULE
	{
		CKeStringA Domain;
	};

	NTSTATUS InsertWhiteRule( const WHITE_RULE& Rule);
	VOID	 ClearWhiteRule();

	NTSTATUS InsertIpRule(const IP_PROT_RULE& Rule);
	VOID	 ClearIpRule();

	NTSTATUS InsertDomainRule(const DOMAIN_RULE& Rule);
	VOID	 ClearDomainRule();

private://����json��ȡ����
	NTSTATUS PraseWhiteRule(const char* json, WHITE_RULE& Rule);
	NTSTATUS PraseIpRule(const char* json, IP_PROT_RULE& Rule);
	NTSTATUS PraseDomainRule(const char* json, DOMAIN_RULE& Rule);
private:
	HANDLE m_hEngine;
	CalloutImpl FlowEstablishedCalloutV4;
	CalloutImpl DatagramCalloutV4;
	CalloutImpl StreamCalloutV4;
	CalloutImpl	ALE_AUTH_CONNECT_CALLOUT_V4;
	CKeMap<UINT64, CFlowContext*, NonPagedObject> m_FlowContextMap;
	CKeSpinLock m_FlowContextLock;

	BOOL m_bExit;

	CTaskCenter m_RecordCenter;

	//����
	CKeVector<WHITE_RULE> m_WhiteRules;
	CKeSpinLock			  m_WhiteRulesLock;

	CKeVector<DOMAIN_RULE>	m_DomainRules;
	CKeSpinLock				m_DomainRulesLock;


	CKeVector<IP_PROT_RULE>	m_IPRules;
	CKeSpinLock				m_IPRulesLock;
public:
	static CNetMonDevice* g_NetMonDevice;
	NTSTATUS  CalloutNotifyFn(
		_In_ FWPS_CALLOUT_NOTIFY_TYPE notifyType,
		_In_ const GUID* filterKey,
		_Inout_ FWPS_FILTER* filter);

	VOID CalloutFlowDeleteFn(
		IN UINT16  layerId,
		IN UINT32  calloutId,
		IN UINT64  flowContext
	);

	VOID  CalloutClassifyFn(
		IN const FWPS_INCOMING_VALUES* inFixedValues,
		IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
		IN VOID* layerData,
		IN const void* classifyContext,
		IN const FWPS_FILTER* filter,
		IN UINT64 flowContext,
		OUT FWPS_CLASSIFY_OUT* classifyOut
	);
};

