#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "utils.h"


//���̴���֪ͨ����
void ProcessCreationNotice( HANDLE ParentId,
	 HANDLE ProcessId,
	 BOOLEAN Create) {
	WCHAR* full = { 0 };
	WCHAR* name = { 0 };
    get_process_name(int(ProcessId), &full, &name);//����pid��ȡ������
	UNICODE_STRING ustr = { 0 };
	RtlInitUnicodeString(&ustr, name);//�ѿ��ַ�ת��Unicode�ַ�
	UNICODE_STRING upStrs = { 0 };
	WCHAR szBuffer[100] = L"";
	RtlInitUnicodeString(&upStrs, szBuffer);
	RtlUpcaseUnicodeString(&upStrs, &ustr, true);//ת��д
	DbgPrint("%d --> %ws %wZ\n", ProcessId, name,upStrs);

}

//�ļ�ɾ��
NTSTATUS FileDel(PWCHAR file_path) {
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING filepath = { 0 };
	OBJECT_ATTRIBUTES obja = { 0 };
	RtlInitUnicodeString(&filepath, file_path);
	InitializeObjectAttributes(&obja, &filepath, OBJ_CASE_INSENSITIVE, NULL, NULL);//��ʼ����������
	status = ZwDeleteFile(&obja);
	return status;
}

void DrvUnload(PDRIVER_OBJECT pdriver) {
	DbgPrint("DrvUnload");
	PsSetCreateProcessNotifyRoutine(ProcessCreationNotice, true);//ȡ�����̴���֪ͨ����
}

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)//�������
{
	DbgPrint("DriverEntry");
	driver->DriverUnload = DrvUnload;//����ж�ش�����
	PsSetCreateProcessNotifyRoutine(ProcessCreationNotice,false);//���ý��̴���֪ͨ����
	FileDel(L"\\??\\C:\\1.txt");
	return 0;
}
