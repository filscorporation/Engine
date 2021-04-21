#include <fstream>

#include "ScriptingSystem.h"
#include "ScriptingCore.h"
#include "../Core/Log.h"

// TODO: temp solution for development, later should link to Iron bins folder
#if defined(_WIN32) || defined(_WIN64)
#define DEBUG_MONO_LIB_PATH "C:\\Program Files (x86)\\Mono\\lib"
#define DEBUG_MONO_ETC_PATH "C:\\Program Files (x86)\\Mono\\etc"
#define DEBUG_API_DLL_PATH "..\\..\\..\\scripting\\IronCustom\\bin\\Debug\\IronCore.dll"
#define DEBUG_SCRIPTS_DLL_PATH "..\\..\\..\\scripting\\IronCustom\\bin\\Debug\\IronCustom.dll"
#define DISTRIBUTE_MONO_LIB_PATH "Mono\\lib"
#define DISTRIBUTE_MONO_ETC_PATH "Mono\\etc"
#define DISTRIBUTE_API_DLL_PATH "Bin\\IronCore.dll"
#define DISTRIBUTE_SCRIPTS_DLL_PATH "Bin\\IronCustom.dll"
#endif
#ifdef __unix__
#define DEBUG_MONO_LIB_PATH "/usr/lib/"
#define DEBUG_MONO_ETC_PATH "/usr/lib/mono"
#define DEBUG_API_DLL_PATH "../../../scripting/IronCustom/bin/Debug/IronCore.dll"
#define DEBUG_SCRIPTS_DLL_PATH "../../../scripting/IronCustom/bin/Debug/IronCustom.dll"
#endif

MonoDomain* domain;
MonoImage* coreAssemblyImage;
MonoImage* customAssemblyImage;

bool ScriptingSystem::isInitialized = false;

inline bool FileExists(const char* fileName)
{
    if (FILE *file = fopen(fileName, "r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

MonoImage* LoadAssembly(const char* fileName)
{
    if (!FileExists(fileName))
    {
        Log::LogError("Error loading assembly, file does not exist: " + std::string(fileName));
        return nullptr;
    }

    MonoAssembly* assembly = mono_domain_assembly_open(domain, fileName);
    if (assembly == nullptr)
    {
        Log::LogError("Error loading assembly " + std::string(fileName));
        return nullptr;
    }
    MonoImage* assemblyImage = mono_assembly_get_image(assembly);
    if (assemblyImage == nullptr)
    {
        Log::LogError("Error creating image for assembly " + std::string(fileName));
        return nullptr;
    }

    return assemblyImage;
}

void ScriptingSystem::Init()
{
    Log::LogInfo("Scripting system init");

    mono_config_parse(nullptr);
#ifdef DISTRIBUTE_BUILD
    mono_set_dirs(DISTRIBUTE_MONO_LIB_PATH, DISTRIBUTE_MONO_ETC_PATH);
#else
    mono_set_dirs(DEBUG_MONO_LIB_PATH, DEBUG_MONO_ETC_PATH);
#endif
    domain = mono_jit_init("IronDomain");

#ifdef DISTRIBUTE_BUILD
    coreAssemblyImage = LoadAssembly(DISTRIBUTE_API_DLL_PATH);
#else
    coreAssemblyImage = LoadAssembly(DEBUG_API_DLL_PATH);
#endif
    if (coreAssemblyImage == nullptr)
    {
        return;
    }

#ifdef DISTRIBUTE_BUILD
    customAssemblyImage = LoadAssembly(DISTRIBUTE_SCRIPTS_DLL_PATH);
#else
    customAssemblyImage = LoadAssembly(DEBUG_SCRIPTS_DLL_PATH);
#endif
    if (customAssemblyImage == nullptr)
    {
        Log::LogWarning("Could not find custom assembly at: " DEBUG_SCRIPTS_DLL_PATH);
        return;
    }

    ScriptingCore::Init(coreAssemblyImage);

    isInitialized = true;

    Log::LogInfo("Scripting system initialized");
}

void ScriptingSystem::Terminate()
{
    if (!isInitialized)
        return;

    ScriptingCore::Terminate();
    mono_jit_cleanup(domain);
}

void ScriptingSystem::CallEntryPoint()
{
    if (!isInitialized)
    {
        Log::LogWarning("Scripting system is not initialized");
        return;
    }

    if (customAssemblyImage == nullptr)
    {
        Log::LogWarning("Custom assembly is null");
        return;
    }

    ScriptingCore::FindAndCallEntryPoint(customAssemblyImage);
}

void ScriptingSystem::UpdateCoroutines()
{
    if (ScriptingSystem::IsInitialized())
        ScriptingCore::CallMethod(ScriptingCore::CoroutinesManagerCalls.callUpdate);
}

bool ScriptingSystem::IsInitialized()
{
    return isInitialized;
}
