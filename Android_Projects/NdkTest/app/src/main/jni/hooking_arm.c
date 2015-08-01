#include "hooking_arm.h"

static struct list_head installed_function_hooks;
static struct list_head enabled_function_hooks;
static struct list_head disabled_function_hooks;


void init_inline_function_hooking()
{
     INIT_LIST_HEAD(&installed_function_hooks);
     INIT_LIST_HEAD(&enabled_function_hooks);
     INIT_LIST_HEAD(&disabled_function_hooks);
}

void destroy_inline_function_hooking()
{

}

bool arguments_are_valid(JNIEnv* env, const unsigned char* targetName, void* targetAddress, const unsigned char* hookName, void* hookAddress)
{
    if(targetAddress == NULL)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "The address of the target function was NULL.");
        return false;
    }
    if(hookAddress == NULL)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "The address of the hook function was NULL.");
        return false;
    }
    if(!isFunctionProperlyCodeAligned(targetAddress))
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "The target address was not properly aligned for ARM/THUMB.");
        return false;
    }
    if(!isFunctionProperlyCodeAligned(hookAddress))
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "The target address was not properly aligned for ARM/THUMB.");
        return false;
    }
    return true;
}

InlineFunctionHook* allocate_new_inline_function_hook( JNIEnv* env )
{
    if(hasExceptionOccurred(env))
    {
        return NULL;
    }
    InlineFunctionHook* hook = malloc(sizeof(InlineFunctionHook));
    if(hook == NULL)
    {
        LOGE("Error allocating memory for InlineFunctionHook object. Error: %s", strerror(errno));
        throwNewJNIException(env, "java/lang/RuntimeException", "Allocation of InlineFunctionHook struct failed.");
        return NULL;
    }
    return hook;
}

InlineFunctionHook* new_inline_function_hook ( JNIEnv* env, const unsigned char* targetName, void* targetAddress, const unsigned char* hookName, void* hookAddress )
{
    if(hasExceptionOccurred(env))
    {
        return NULL;
    }

    LOGI("Creating hook for function \"%s\"("PRINT_PTR") with hookFunction \"%s\""PRINT_PTR, targetName, (uintptr_t)targetAddress, hookName, (uintptr_t)hookAddress);

    if(!arguments_are_valid(env, targetName, targetAddress, hookName, hookAddress))
    {
        LOGE("Invalid arguments.");
        return NULL;
    }

    InlineFunctionHook* hook = allocate_new_inline_function_hook(env);
    if(hook == NULL)
    {
        LOGE("Error allocating memory: %s", strerror(errno));
        return NULL;
    }

    hook->env = env;

    hook->overwrite_size = HOOK_CODE_SIZE;

    hook->target_function.name              =   targetName;
    hook->target_function.address           =   targetAddress;
    hook->target_function.memory_location   =   getCodeBaseAddress(targetAddress);
    hook->target_function.uses_thumb_mode   =   isFunctionThumbMode(targetAddress);

    hook->hook_function.name                =   hookName;
    hook->hook_function.address             =   hookAddress;
    hook->hook_function.memory_location     =   getCodeBaseAddress(hookAddress);
    hook->hook_function.uses_thumb_mode     =   isFunctionThumbMode(hookAddress);

    if(!setMemoryProtection(env, getCodeBaseAddress(targetAddress), HOOK_CODE_SIZE, true, true, true))
    {
        LOGE("Error setting memory protections: %s", strerror(errno));
        return NULL;
    }

    // Preserve first bytes of function
    memcpy(hook->preserved_code, hook->target_function.memory_location, hook->overwrite_size);
    // Initialize Trampoline code
    memcpy(hook->hook_code, (hook->target_function.uses_thumb_mode ? hook_stub_thumb : hook_stub_arm), hook->overwrite_size);
    // Here we write our hook function address. The trampoline codes are designed to have the same offset.

    void** addrTarget = (void**)(hook->hook_code + 12);
    *addrTarget = hookAddress;

    list_add_tail(&hook->installed_entry, &installed_function_hooks);
    list_add_tail(&hook->disabled_entry, &disabled_function_hooks);
    INIT_LIST_HEAD(&hook->enabled_entry);

    return hook;
}

InlineFunctionHook* install_inline_function_hook( JNIEnv* env, const unsigned char* funcName, void* funcAddress, const unsigned char* hookName, void* hookFunction)
{
    return new_inline_function_hook(env, funcName, funcAddress, hookName, hookFunction);
}

bool uninstall_inline_function_hook  ( InlineFunctionHook * hook )
{
    if(hook == NULL)
    {
        LOGE("The hook pointer was NULL. Throwing Exception ... ");
        return false;
    }
    if(hasExceptionOccurred(hook->env))
    {
        return false;
    }
    LOGD("Uninstalling Hook. Don't use this after calling this, the hook memory was freed if this function succeeded.");
    disable_inline_function_hook(hook);
    if(hasExceptionOccurred(hook->env))
    {
        list_del_init(&hook->installed_entry);
        list_del_init(&hook->disabled_entry);
        list_del_init(&hook->enabled_entry);
        free(hook);
        return false;
    }
    list_del_init(&hook->disabled_entry);
    list_del_init(&hook->installed_entry);
    free(hook);
    return true;
}

bool patch_original_code ( InlineFunctionHook * hook, void* code, int size)
{
    if(hasExceptionOccurred(hook->env))
    {
        return false;
    }
    if(hook == NULL)
    {
        throwNewJNIException(hook->env, "java/lang/NullPointerException", "The hook pointer was NULL. This is not allowed.");
        return false;
    }
    if(size != hook->overwrite_size)
    {
        throwNewJNIException(hook->env, "IllegalArgumentException", "Cannot patch original code with different size from overwritten code.");
        return false;
    }

    LOGD("Patching Original Code for function \"%s\"("PRINT_PTR").", hook->target_function.name, (uintptr_t)hook->target_function.address);

    LOGD("Before: ");
    hexdump_aligned(hook->env, hook->preserved_code, hook->overwrite_size, 16, 4);

    memcpy(hook->preserved_code, code, hook->overwrite_size);

    LOGD("After: ");
    hexdump_aligned(hook->env, hook->preserved_code, hook->overwrite_size, 16, 4);
    return true;
}

bool enable_inline_function_hook ( InlineFunctionHook * hook )
{
    if(hook == NULL)
    {
        return false;
    }

    if(!list_empty(&hook->enabled_entry) || list_empty(&hook->disabled_entry))
    {
        LOGD("Ignoring enable request, hook already enabled.");
        return true;
    }

    if(!setMemProtectFull(hook->env, hook->target_function.memory_location, hook->overwrite_size))
    {
        return false;
    }
    memcpy(hook->target_function.memory_location, hook->hook_code, hook->overwrite_size);

    __builtin___clear_cache(hook->target_function.memory_location, hook->target_function.memory_location + hook->overwrite_size);

    // Update our List structs
    list_del_init(&hook->disabled_entry);
    list_add(&hook->enabled_entry, &enabled_function_hooks);

    return true;
}
bool disable_inline_function_hook    ( InlineFunctionHook * hook )
{
    if(hook == NULL)
    {
        return false;
    }

    if(!list_empty(&hook->disabled_entry) || list_empty(&hook->enabled_entry))
    {
        LOGD("Ignoring disable request, hook already disabled.");
        return true;
    }

    if(!setMemProtectFull(hook->env, hook->target_function.memory_location, hook->overwrite_size))
    {
        return false;
    }
    memcpy(hook->target_function.memory_location, hook->preserved_code, hook->overwrite_size);
    __builtin___clear_cache(hook->target_function.memory_location, hook->target_function.memory_location + hook->overwrite_size);

    // Update our List structs
    list_del_init(&hook->enabled_entry);
    list_add(&hook->disabled_entry, &disabled_function_hooks);

    return true;
}

void print_full_inline_function_hook_info(JNIEnv* env)
{
    InlineFunctionHook* current;

    LOGI("###################################################################################");
    LOGI("Installed Hooks:");
    list_for_each_entry(current, &installed_function_hooks, installed_entry)
    {
        LOGI("%s", current->target_function.name);
        LOGD("-->Target Function:");
        LOGD("----->Name:               %s", current->target_function.name);
        LOGD("----->Address:            "PRINT_PTR, (uintptr_t)current->target_function.address);
        LOGD("----->MemoryLocation:     "PRINT_PTR, (uintptr_t)current->target_function.memory_location);
        LOGD("----->UsesThumbMode:      %s", current->target_function.uses_thumb_mode ? "true" : "false");
        LOGD("-->Hook Function:");
        LOGD("----->Name:               %s", current->hook_function.name);
        LOGD("----->Address:            "PRINT_PTR, (uintptr_t)current->hook_function.address);
        LOGD("----->MemoryLocation:     "PRINT_PTR, (uintptr_t)current->hook_function.memory_location);
        LOGD("----->UsesThumbMode:      %s", current->hook_function.uses_thumb_mode ? "true" : "false");
    }
    LOGI("Enabled Hooks:");
    list_for_each_entry(current, &enabled_function_hooks, enabled_entry)
    {
        LOGI("%s", current->target_function.name);
        LOGD("-->Target Function:");
        LOGD("----->Name:               %s", current->target_function.name);
        LOGD("----->Address:            "PRINT_PTR, (uintptr_t)current->target_function.address);
        LOGD("----->MemoryLocation:     "PRINT_PTR, (uintptr_t)current->target_function.memory_location);
        LOGD("----->UsesThumbMode:      %s", current->target_function.uses_thumb_mode ? "true" : "false");
        LOGD("-->Hook Function:");
        LOGD("----->Name:               %s", current->hook_function.name);
        LOGD("----->Address:            "PRINT_PTR, (uintptr_t)current->hook_function.address);
        LOGD("----->MemoryLocation:     "PRINT_PTR, (uintptr_t)current->hook_function.memory_location);
        LOGD("----->UsesThumbMode:      %s", current->hook_function.uses_thumb_mode ? "true" : "false");
    }
    LOGI("Disabled Hooks:");
    list_for_each_entry(current, &disabled_function_hooks, disabled_entry)
    {
        LOGI("%s", current->target_function.name);
        LOGD("-->Target Function:");
        LOGD("----->Name:               %s", current->target_function.name);
        LOGD("----->Address:            "PRINT_PTR, (uintptr_t)current->target_function.address);
        LOGD("----->MemoryLocation:     "PRINT_PTR, (uintptr_t)current->target_function.memory_location);
        LOGD("----->UsesThumbMode:      %s", current->target_function.uses_thumb_mode ? "true" : "false");
        LOGD("-->Hook Function:");
        LOGD("----->Name:               %s", current->hook_function.name);
        LOGD("----->Address:            "PRINT_PTR, (uintptr_t)current->hook_function.address);
        LOGD("----->MemoryLocation:     "PRINT_PTR, (uintptr_t)current->hook_function.memory_location);
        LOGD("----->UsesThumbMode:      %s", current->hook_function.uses_thumb_mode ? "true" : "false");
    }
    LOGI("###################################################################################");
}





