#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "utils.h"


//进程创建通知处理
void ProcessCreationNotice( HANDLE ParentId,
	 HANDLE ProcessId,
	 BOOLEAN Create) {
	WCHAR* full = { 0 };
	WCHAR* name = { 0 };
    get_process_name(int(ProcessId), &full, &name);//根据pid获取进程名
	UNICODE_STRING ustr = { 0 };
	RtlInitUnicodeString(&ustr, name);//把宽字符转成Unicode字符
	UNICODE_STRING upStrs = { 0 };
	WCHAR szBuffer[100] = L"";
	RtlInitUnicodeString(&upStrs, szBuffer);
	RtlUpcaseUnicodeString(&upStrs, &ustr, true);//转大写
	DbgPrint("%d --> %ws %wZ\n", ProcessId, name,upStrs);

}

//文件删除
NTSTATUS FileDel(PWCHAR file_path) {
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING filepath = { 0 };
	OBJECT_ATTRIBUTES obja = { 0 };
	RtlInitUnicodeString(&filepath, file_path);
	InitializeObjectAttributes(&obja, &filepath, OBJ_CASE_INSENSITIVE, NULL, NULL);//初始化对象属性
	status = ZwDeleteFile(&obja);
	return status;
}

void DrvUnload(PDRIVER_OBJECT pdriver) {
	DbgPrint("DrvUnload");
	PsSetCreateProcessNotifyRoutine(ProcessCreationNotice, true);//取消进程创建通知处理
}

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)//驱动入口
{
	DbgPrint("DriverEntry");
	driver->DriverUnload = DrvUnload;//驱动卸载处理函数
	PsSetCreateProcessNotifyRoutine(ProcessCreationNotice,false);//设置进程创建通知处理
	FileDel(L"\\??\\C:\\1.txt");
	return 0;
}
