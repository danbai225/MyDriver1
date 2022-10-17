
NTSTATUS get_process_name(int pid, PWCHAR* out_fullpath, PWCHAR* out_name)
{
    typedef NTSTATUS(*xxQUERY_INFO_PROCESS) (
        __in HANDLE ProcessHandle,
        __in PROCESSINFOCLASS ProcessInformationClass,
        __out_bcount(ProcessInformationLength) PVOID ProcessInformation,
        __in ULONG ProcessInformationLength,
        __out_opt PULONG ReturnLength
        );

    static xxQUERY_INFO_PROCESS ZwQueryInformationProcess = NULL;
    NTSTATUS     status = STATUS_UNSUCCESSFUL;
    ULONG        returnedLength;
    ULONG        bufferLength;
    PVOID        buffer;
    PUNICODE_STRING imageName;
    HANDLE       handle;

    *out_name = NULL; *out_fullpath = 0;

    if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
        DbgPrint("IRQL Must PASSIVE_LEVEL.\n");
        return status;
    }

    if (NULL == ZwQueryInformationProcess) {

        UNICODE_STRING routineName;

        RtlInitUnicodeString(&routineName, L"ZwQueryInformationProcess");

        ZwQueryInformationProcess =
            (xxQUERY_INFO_PROCESS)MmGetSystemRoutineAddress(&routineName);

        if (NULL == ZwQueryInformationProcess) {
            DbgPrint("Cannot resolve ZwQueryInformationProcess\n");
            return status;
        }
    }
    ///get process handle;
    OBJECT_ATTRIBUTES ObjectAttributes;
    CLIENT_ID clientid;
    InitializeObjectAttributes(&ObjectAttributes, 0, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
    clientid.UniqueProcess = (HANDLE)pid;
    clientid.UniqueThread = 0;
    status = ZwOpenProcess(&handle, PROCESS_ALL_ACCESS, &ObjectAttributes, &clientid);
    if (!NT_SUCCESS(status)) {
        return status;
    }
    //
    status = ZwQueryInformationProcess(handle,
        ProcessImageFileName,
        NULL, // buffer
        0, // buffer size
        &returnedLength);
    if (STATUS_INFO_LENGTH_MISMATCH != status) {

        ZwClose(handle);
        return status;
    }

    bufferLength = returnedLength - sizeof(UNICODE_STRING);

    buffer = ExAllocatePoolWithTag(NonPagedPool, returnedLength, 'FXSD');
    if (NULL == buffer) {

        ZwClose(handle);
        return STATUS_NO_MEMORY;
    }

    __try
    {

        status = ZwQueryInformationProcess(handle,
            ProcessImageFileName,
            buffer,
            returnedLength,
            &returnedLength);

        if (NT_SUCCESS(status)) {

            imageName = (PUNICODE_STRING)buffer;

            USHORT len = imageName->Length;
            RtlMoveMemory(buffer, imageName->Buffer, imageName->Length);
            RtlZeroMemory(((PUCHAR)buffer) + len, sizeof(WCHAR));

            *out_fullpath = (PWCHAR)buffer;

            PWCHAR ptr = wcsrchr((PWCHAR)buffer, L'\\');
            if (ptr) {
                *out_name = ptr + 1;
            }
            else {
                *out_name = (PWCHAR)buffer;
            }
            ZwClose(handle);
            return STATUS_SUCCESS;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        DbgPrint("get_process_name: Exception!!\n");
    }

    ZwClose(handle);
    ExFreePool(buffer);

    return status;
}