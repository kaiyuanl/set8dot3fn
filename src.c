/*
utility created by v-kail to repro SE bug E14: 712895
this utility is used to change the short name (8.3 filename) of file selected.
usage:
    set8dot3fn <targetfile> <8dot3name>
example:
    set8dot3fn onlytestfile.txt OTF000~1.TXT
*/

#include<windows.h>
#include<tchar.h>
#define MAX_MSG 1000
LPCWSTR helpMsg = _T("\nUsage:\n\tset8dot3fn <targetfile> <8dot3name>\n\nExample:\n\tset8dot3fn onlytestfile.txt OTF000~1.TXT\n");

DWORD ModifyPrivilege(
    IN LPCTSTR szPrivilege,
    IN BOOL fEnable);

void PrintErrMsg(DWORD err);

int _tmain(int argc, TCHAR *argv[])
{
    LPCWSTR file;
    LPCWSTR _8dot3fn;
    HANDLE hFile;
    DWORD err;
    LPCWSTR privilege = _T("SeRestorePrivilege");

    if(argc == 1 || argc > 3)
    {
        _tprintf(helpMsg);
        return 1;
    }

    file = argv[1];
    _8dot3fn = argv[2];

    if((err = ModifyPrivilege(privilege, TRUE)) != ERROR_SUCCESS)
    {
        _tprintf(_T("Modified privilege failed, err code: %u\n"), err);
        PrintErrMsg(err);
        return err;
    }

    hFile = CreateFile(
        file,
        GENERIC_WRITE | DELETE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS ,
        NULL
        );
    
    if((err = GetLastError()) != 0)
    {
        _tprintf(_T("Open file %s failed, error code: %u\n"), file, err);
        PrintErrMsg(err);
        return err;
    }

    
    if(SetFileShortName(hFile, _8dot3fn) == FALSE)
    {
        err = GetLastError();
        _tprintf(_T("Set 8dot3 file name failed, error code: %u\n"), err);
        PrintErrMsg(err);
        if(err == ERROR_PRIVILEGE_NOT_HELD)
        {
            _tprintf(_T("Please run cmd as administrator\n"));
        }
        CloseHandle(hFile);
        return err;
    }
    _tprintf(_T("Set 8dot3 file name successfully!\n"));
    _tprintf(_T("Full name: %s\t8Dot3 name: %s"), file, _8dot3fn);
    CloseHandle(hFile);
    return 0;
}

DWORD ModifyPrivilege(
    IN LPCTSTR szPrivilege,
    IN BOOL fEnable)
{
    DWORD ret = ERROR_SUCCESS;
    TOKEN_PRIVILEGES NewState;
    LUID             luid;
    HANDLE hToken    = NULL;

    // Open the process token for this process.
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                          &hToken ))
    {
        _tprintf(_T("Failed OpenProcessToken %u\n"), ret = GetLastError());
        return ret;
    }

    // Get the local unique ID for the privilege.
    if ( !LookupPrivilegeValue( NULL,
                                szPrivilege,
                                &luid ))
    {
        CloseHandle( hToken );
        _tprintf(_T("Failed LookupPrivilegeValue %u\n"), ret = GetLastError());
        return ret;
    }

    // Assign values to the TOKEN_PRIVILEGE structure.
    NewState.PrivilegeCount = 1;
    NewState.Privileges[0].Luid = luid;
    NewState.Privileges[0].Attributes = 
              (fEnable ? SE_PRIVILEGE_ENABLED : 0);

    // Adjust the token privilege.
    if (!AdjustTokenPrivileges(hToken,
                               FALSE,
                               &NewState,
                               0,
                               NULL,
                               NULL))
    {
        _tprintf(_T("Failed AdjustTokenPrivileges %u\n"), ret = GetLastError());
    }

    // Close the handle.
    CloseHandle(hToken);

    return ret;
}

void PrintErrMsg(DWORD err)
{
    LPVOID errMsgBuff;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errMsgBuff,
        0,
        NULL);
    _tprintf(_T("Error description: %s\n"), errMsgBuff);
}
