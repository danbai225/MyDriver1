#pragma once
#include <ntddk.h>
NTSTATUS get_process_name(int pid, PWCHAR* out_fullpath, PWCHAR* out_name);