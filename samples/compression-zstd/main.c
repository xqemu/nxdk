#include <stdlib.h>
#include <string.h>

#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>

#include <windows.h>

#include <zstd.h>

const char* example = "This text has been compressed on the Microsoft Xbox using Zstandard. The application was built with the nxdk.";

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    debugPrint("Compressing now:\n\"%s\"\n", example);

    size_t size = strlen(example), compressed_size = ZSTD_compressBound(strlen(example));
    debugPrint("Original size: %lu.\n", size);

    char* compressed_buffer = calloc(compressed_size + 1, sizeof(char));
    size_t end_size = ZSTD_compress(compressed_buffer, compressed_size, example, size, 16);
    debugPrint("New size: %lu.\n", end_size);

    char* minified_compressed_buffer = calloc(end_size + 1, sizeof(char));
    memcpy(minified_compressed_buffer, compressed_buffer, sizeof(char) * end_size);
    free(compressed_buffer);

    debugPrint("Compressed data:\n");
    debugPrintHex(minified_compressed_buffer, end_size);
    debugPrint("\n");

    char* decompressed_buffer = calloc(1, sizeof(char));
    size_t new_size = ZSTD_decompress(decompressed_buffer, size, minified_compressed_buffer, end_size);
    debugPrint("Decompressed size: %d.\n", new_size);
    debugPrint("Decompressed data:\n\"%s\"\n", decompressed_buffer);

    free(minified_compressed_buffer);
    free(decompressed_buffer);

    debugPrint("Done.");

    while (TRUE) SwitchToThread();

    return 0;
}
