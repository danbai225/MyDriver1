#include <ntddk.h>
#include "utils.h"

void nothing( HANDLE ParentId,
	 HANDLE ProcessId,
	 BOOLEAN Create) {
	WCHAR* full = { 0 };
	WCHAR* name = { 0 };
    get_process_name(int(ProcessId), &full, &name);
	DbgPrint("%d --> %ws\n", ProcessId, name);
	UNICODE_STRING ustr = { 0 };
	RtlInitUnicodeString(&ustr, name);
	UNICODE_STRING upStrs = { 0 };
	WCHAR szBuffer[100] = L"";
	RtlInitUnicodeString(&upStrs, szBuffer);
	RtlUpcaseUnicodeString(&upStrs, &ustr, true);
	DbgPrint("%wZ", upStrs);

}
void DrvUnload(PDRIVER_OBJECT pdriver) {
	DbgPrint("DrvUnload");
	PsSetCreateProcessNotifyRoutine(nothing, true);
}

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)//Çý¶¯Èë¿Ú
{
	DbgPrint("DriverEntry");
	driver->DriverUnload = DrvUnload;
	PsSetCreateProcessNotifyRoutine(nothing,false);
	return 0;
}

NTSTATUS FileDel(PWCHAR file_path) {
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING filepath = { 0 };
	RtlInitUnicodeString(&filepath, file_path);
	return status;
}