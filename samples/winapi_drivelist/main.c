#include <hal/debug.h>
#include <hal/video.h>
#include <nxdk/mount.h>
#include <windows.h>

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    // Mount some drives for demonstration purposes
    BOOL ret;
    DWORD error;
    ret = nxMountDrive('C', "\\Device\\Harddisk0\\Partition2\\");
    if (!ret) {
        // Additional error info can be retrieved with GetLastError()
        error = GetLastError();
        debugPrint("Failed to mount C: drive! Error code: %x\n", error);
        goto sleepForever;
    }

    ret = nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    if (!ret) {
        error = GetLastError();
        debugPrint("Failed to mount E: drive! Error code: %x\n", error);
        goto unmount_c;
    }

    // Retrieve drive bitmasks. Every bit represents one drive letter
    DWORD driveBits = GetLogicalDrives();
    error = GetLastError();
    if (driveBits == 0 && error != ERROR_SUCCESS) {
        debugPrint("Failed to retrieve drive bitmask! Error code: %x\n", error);
        goto cleanup;
    }

    debugPrint("Drive bitmask: 0x%x\n\n", driveBits);

    // Reserve buffer long enough for all possible drive strings plus null-terminator
    char buffer[26 * 4 + 1];
    // IMPORTANT: The size passed to GetLogicalDriveStringsA is WITHOUT the null-terminator, even though it gets written
    DWORD charsWritten = GetLogicalDriveStringsA(sizeof(buffer)-1, buffer);
    if (charsWritten == 0) {
        error = GetLastError();
        debugPrint("Failed to retrieve drive strings! Error code: %x\n", error);
        goto cleanup;
    }

    if (charsWritten > sizeof(buffer) - 1) {
        // Can't happen here as our buffer is large enough to cover all possibilities
        debugPrint("Buffer for GetLogicalDriveStringsA too small!\n");
        goto cleanup;
    }

    debugPrint("Drives found:\n");
    char *drive = buffer;
    while (drive < buffer + charsWritten) {
        debugPrint("%s\n", drive);
        while(*drive++);
    }

    debugPrint("\nDone!");

cleanup:
    ret = nxUnmountDrive('E');
    if (!ret) {
        error = GetLastError();
        debugPrint("\nFailed to unmount E: drive! Error code: %x\n", error);
    }

unmount_c:
    ret = nxUnmountDrive('C');
    if (!ret) {
        error = GetLastError();
        debugPrint("\nFailed to unmount C: drive! Error code: %x\n", error);
    }

sleepForever:
    while (1) {
        Sleep(2000);
    }
}
