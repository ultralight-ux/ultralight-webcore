#include "config.h"
#include "ResourceLoaderUltralight.h"
#include "StringUltralight.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/private/util/Debug.h>

namespace WebCore {
namespace ResourceLoader {

    ultralight::FileHandle openFile(const String& filePath)
    {
        ultralight::FileSystem* fs = ultralight::Platform::instance().file_system();
        ultralight::String16 filePath16 = "resources/" + ultralight::Convert(filePath);

        if (!fs) {
            ultralight::String16 err_msg = "Could not load resource: " + filePath16 + ", no FileSystem instance set, make sure that you've called ultralight::Platform::instance().set_file_system().";
            UL_LOG_ERROR(err_msg);
            return ultralight::invalidFileHandle;
        }

        if (!fs->FileExists(filePath16)) {
            ultralight::String16 err_msg = "Could not load resource: " + filePath16 + ", FileSystem::FileExists() returned false.";
            UL_LOG_ERROR(err_msg);
            return ultralight::invalidFileHandle;
        }

        ultralight::FileHandle handle = fs->OpenFile(filePath16, false);

        if (handle == ultralight::invalidFileHandle) {
            ultralight::String16 err_msg = "Could not load resource: " + filePath16 + ", FileSystem::OpenFile() returned an invalid file handle.";
            UL_LOG_ERROR(err_msg);
            return ultralight::invalidFileHandle;
        }

        return handle;
    }

    String readFileToString(const String& filePath)
    {
        ultralight::FileSystem* fs = ultralight::Platform::instance().file_system();
        if (!fs)
            return String();

        ultralight::FileHandle handle = openFile(filePath);
        if (handle == ultralight::invalidFileHandle)
            return String();

        int64_t fileSize = 0;
        Vector<char> buffer;

        if (!fs->GetFileSize(handle, fileSize) || fileSize == 0)
            goto FAIL_LOAD;

        buffer.resize(fileSize);
        if (fs->ReadFromFile(handle, buffer.data(), fileSize) != fileSize)
            goto FAIL_LOAD;

        fs->CloseFile(handle);

        return String(buffer.data(), static_cast<size_t>(fileSize));

    FAIL_LOAD:
        if (fs && handle != ultralight::invalidFileHandle)
            fs->CloseFile(handle);
        return String();
    }

} // namespace ResourceLoader
} // namespace WebCore
