#ifdef __cplusplus
extern "C" {
#endif

// Windows-specific extension required by libc++
int _snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);

#ifdef __cplusplus
}
#endif
