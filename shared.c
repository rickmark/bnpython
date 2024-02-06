#include <dlfcn.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <wchar.h>

#include "include/shared.h"

#define SEP "/"
#define DIR_PARENT "/.."
#define PY_SEP L":"

PyConfig* pythonConfig = NULL;

void* bnpython = NULL;
wchar_t* bnbase = NULL;
wchar_t* bnpyprefix = NULL;
wchar_t* bnpypath = NULL;
wchar_t* pyhome = NULL;
wchar_t* bnpyexecprefix = NULL;
Py_InitializeFunc Py_Initialize = NULL;
Py_RunMainFunc Py_RunMain = NULL;
PyConfig_InitIsolatedConfigFunc PyConfig_InitIsolatedConfig = NULL;
PyConfig_SetArgvFunc PyConfig_SetArgv = NULL;
PyConfig_SetStringFunc PyConfig_SetString = NULL;
Py_InitializeFromConfigFunc Py_InitializeFromConfig = NULL;
PyConfig_SetWideStringListFunc PyConfig_SetWideStringList = NULL;
PyConfig_SetBytesArgvFunc PyConfig_SetBytesArgv = NULL;
PyPreConfig_InitIsolatedConfigFunc PyPreConfig_InitIsolatedConfig = NULL;
Py_PreInitializeFromBytesArgsFunc Py_PreInitializeFromBytesArgs = NULL;

const wchar_t* PYTHON_HOME = L"/Library/Application Support/Binary Ninja/python310";
const wchar_t* OFFICIAL_PLUGIN_SPEC = L"/Library/Application Support/Binary Ninja/repositories/official/plugins";
const wchar_t* COMMUNITY_PLUGIN_SPEC = L"/Library/Application Support/Binary Ninja/repositories/community/plugins";

const wchar_t* BASE_EXEC_PREFIX_SPEC = L"/Resources/bundled-python3";
const wchar_t* BASE_PYTHON_DYNLOAD_SPEC = L"/Resources/bundled-python3/lib/python3.10/lib-dynload";
const wchar_t* BASE_PYTHON_LIB_SPEC = L"/Resources/bundled-python3/lib/python3.10";
const wchar_t* BASE_PYTHON_ZIP_SPEC = L"/Resources/bundled-python3/lib/python310.zip";
const wchar_t* BASE_PYTHON_SPEC = L"/Resources/python";
const wchar_t* BASE_PYTHON3_SPEC = L"/Resources/python3";

void* directly_load_binja_python() {
    if (bnpython == NULL) {
        bnpython = dlopen("/Applications/Binary Ninja.app/Contents/Frameworks/Python.framework/Versions/3.10/Python", RTLD_NOW | RTLD_GLOBAL);
    }
    return bnpython;
}

wchar_t* get_user_path() {
    uid_t userId = getuid();
    struct passwd *pw = getpwuid(userId);

    const char *homedir = pw->pw_dir;
    wchar_t* widehomedir = malloc(strlen(homedir) * sizeof(wchar_t) + 1);
    mbstowcs(widehomedir, homedir, strlen(homedir));
    return widehomedir;
}

char *concat(const char *s0, ...)
{
    if (!s0) return "";

    va_list va;
    va_start(va, s0);

    size_t size = strlen(s0);

    const char *s = va_arg(va, const char *);
    while (s)
    {
        size += strlen(s);
        s = va_arg(va, const char *);
    }

    char *result = malloc(size + 1);

    strcpy(result, s0);

    va_end(va);
    va_start(va, s0);

    s = va_arg(va, const char *);
    while (s)
    {
        strcat(result, s);
        s = va_arg(va, const char *);
    }

    va_end(va);
    return result;
}

wchar_t *wconcat(const wchar_t *s0, ...)
{
    if (!s0) return L"";

    va_list va;
    va_start(va, s0);

    size_t size = wcslen(s0) * sizeof(wchar_t);

    const wchar_t *s = va_arg(va, const wchar_t *);
    while (s)
    {
        size += wcslen(s) * sizeof(wchar_t);
        s = va_arg(va, const wchar_t *);
    }

    wchar_t *result = malloc(size + 1);

    wcscpy(result, s0);

    va_end(va);
    va_start(va, s0);

    s = va_arg(va, const wchar_t *);
    while (s)
    {
        wcscat(result, s);
        s = va_arg(va, const wchar_t *);
    }

    va_end(va);
    return result;
}

void configure_python_symbols();

wchar_t* get_binja_base_dir() {
    if (bnbase) {
        return bnbase;
    }

    configure_python_symbols();

    Dl_info info;
    dladdr(Py_Initialize, &info);

    char* pybasename = strdup(info.dli_fname);

    char* binja_path_spec = concat(dirname(pybasename), DIR_PARENT, DIR_PARENT, DIR_PARENT, DIR_PARENT, NULL);
    char* binary_ninja_base = realpath(binja_path_spec, NULL);
    free(binja_path_spec);
    free(pybasename);

    bnbase = malloc((strlen(binary_ninja_base) * sizeof(wchar_t)) + 1);
    mbstowcs(bnbase, binary_ninja_base, strlen(binary_ninja_base));
    free(binary_ninja_base);
    return bnbase;
}

void configure_python_symbols() {
    if (!Py_Initialize) {
        Py_Initialize = dlsym(directly_load_binja_python(), "Py_Initialize");
    }
    if (!Py_RunMain) {
        Py_RunMain = dlsym(directly_load_binja_python(), "Py_RunMain");
    }
    if (!PyConfig_InitIsolatedConfig) {
        PyConfig_InitIsolatedConfig = dlsym(directly_load_binja_python(), "PyConfig_InitIsolatedConfig");
    }
    if (!PyConfig_SetArgv) {
        PyConfig_SetArgv = dlsym(directly_load_binja_python(), "PyConfig_SetArgv");
    }
    if (!Py_InitializeFromConfig) {
        Py_InitializeFromConfig = dlsym(directly_load_binja_python(), "Py_InitializeFromConfig");
    }
    if (!PyConfig_SetWideStringList) {
        PyConfig_SetWideStringList = dlsym(directly_load_binja_python(), "PyConfig_SetWideStringList");
    }
    if (!PyConfig_SetBytesArgv) {
        PyConfig_SetBytesArgv = dlsym(directly_load_binja_python(), "PyConfig_SetBytesArgv");
    }
    if (!PyConfig_SetString) {
        PyConfig_SetString = dlsym(directly_load_binja_python(), "PyConfig_SetString");
    }
    if (!PyPreConfig_InitIsolatedConfig) {
        PyPreConfig_InitIsolatedConfig = dlsym(directly_load_binja_python(), "PyPreConfig_InitIsolatedConfig");
    }
    if (!Py_PreInitializeFromBytesArgs) {
        Py_PreInitializeFromBytesArgs = dlsym(directly_load_binja_python(), "Py_PreInitializeFromBytesArgs");
    }
}

void setup_binja_paths() {
    configure_python_symbols();

    wchar_t* pyprefex_spec = wconcat(get_binja_base_dir(), L"/Resources/bundled-python3", NULL);
    char* narrow_pyprefex = malloc(wcslen(pyprefex_spec) + 1);
    wcstombs(narrow_pyprefex, pyprefex_spec, wcslen(pyprefex_spec));
    char* narrow_bnpyprefix = realpath(narrow_pyprefex, NULL);
    free(pyprefex_spec);

    size_t wide_bnpyprefix_length = (strlen(narrow_bnpyprefix) * sizeof(wchar_t)) + 1;
    bnpyprefix = malloc(wide_bnpyprefix_length);
    mbstowcs(bnpyprefix, narrow_bnpyprefix, strlen(narrow_bnpyprefix));
    free(narrow_bnpyprefix);
    free(narrow_pyprefex);

    bnpyexecprefix = wconcat(wcsdup(bnbase), BASE_EXEC_PREFIX_SPEC, NULL);

    wchar_t* homedir = get_user_path();

    bnpypath = wconcat(
        homedir, OFFICIAL_PLUGIN_SPEC, PY_SEP, 
        homedir, COMMUNITY_PLUGIN_SPEC, PY_SEP, 
        bnbase, BASE_PYTHON_DYNLOAD_SPEC, PY_SEP, 
        bnbase, BASE_PYTHON_LIB_SPEC, PY_SEP,
        bnbase, BASE_PYTHON_ZIP_SPEC, PY_SEP,
        bnbase, BASE_PYTHON3_SPEC, PY_SEP,
        bnbase, BASE_PYTHON_SPEC, NULL);

    pyhome = wconcat(homedir, PYTHON_HOME, NULL);
}

int binja_python_main(int argc, char *const *argv) {
    configure_python_symbols();
    setup_binja_paths();

    PyStatus status;
    PyPreConfig preconfig;
    PyConfig pythonConfig;

    PyPreConfig_InitIsolatedConfig(&preconfig);
    preconfig.utf8_mode = 1;
    preconfig.allocator = 3; // PYMEM_ALLOCATOR_MALLOC
    Py_PreInitializeFromBytesArgs(&preconfig, argc, argv);

    PyConfig_InitIsolatedConfig(&pythonConfig);

    pythonConfig.parse_argv = 1;
    pythonConfig.parser_debug = 1;

    PyConfig_SetString(&pythonConfig, &pythonConfig.exec_prefix, bnpyexecprefix);

    PyConfig_SetString(&pythonConfig, &pythonConfig.pythonpath_env, bnpypath);
    PyConfig_SetString(&pythonConfig, &pythonConfig.program_name, L"bnpython");
    PyConfig_SetString(&pythonConfig, &pythonConfig.home, pyhome);




    Py_InitializeFromConfig(&pythonConfig);

    return Py_RunMain();
}