#include "breakpoints_arm.h"

static struct list_head installed;
static struct list_head enabled;
static struct list_head disabled;

void init_breakpoints()
{
     INIT_LIST_HEAD(&installed);
     INIT_LIST_HEAD(&enabled);
     INIT_LIST_HEAD(&disabled);
}

void destroy_inline_function_hooking()
{

}

bool arguments_are_valid(JNIEnv* env, void* target, BREAKPOINT_CALLBACK callback)
{
    return env != NULL && target == NULL && callback != NULL;
}

Breakpoint* allocate_new_breakpoint( JNIEnv* env, uint32_t bp_bytes )
{
    if(hasExceptionOccurred(env))
    {
        return NULL;
    }
    Breakpoint* bp = (Breakpoint*)malloc(sizeof(Breakpoint) + sizeof(unsigned char[bp_bytes]));
    if(bp == NULL)
    {
        LOGE("Error allocating memory for Breakpoint object. Error: %s", strerror(errno));
        throwNewJNIException(env, "java/lang/RuntimeException", "Allocation of Breakpoint struct failed.");
        return NULL;
    }
    bp->code_size = bp_bytes;
    return bp;
}

Breakpoint* new_breakpoint ( JNIEnv* env, void* targetAddress, BREAKPOINT_CALLBACK callback )
{
    if(hasExceptionOccurred(env))
    {
        return NULL;
    }

    uint32_t bp_size = isFunctionThumbMode(targetAddress) ? 2 : 4;

    LOGI("Creating breakpoint at address "PRINT_PTR" with callback "PRINT_PTR, (uintptr_t)targetAddress, (uintptr_t)callback);

    if(!arguments_are_valid(env, targetAddress, callback))
    {
        LOGE("Invalid arguments.");
        return NULL;
    }

    Breakpoint* bp = allocate_new_breakpoint(env, bp_size);
    if(bp == NULL)
    {
        LOGE("Error allocating memory: %s", strerror(errno));
        return NULL;
    }

    bp->env = env;

    bp->code_size = bp_size;
    bp->callback = callback;

    bp->target.code_address       = targetAddress;
    bp->target.memory_location    = getCodeBaseAddress(targetAddress);
    bp->target.uses_thumb_mode    = isFunctionThumbMode(targetAddress);

    if(!setMemoryProtection(env, bp->target.memory_location, bp->code_size, true, true, true))
    {
        LOGE("Error setting memory protections: %s", strerror(errno));
        return NULL;
    }

    memcpy(bp->preserved_code, bp->target.memory_location, bp->code_size);

    list_add_tail(&hook->installed_entry, &installed_function_hooks);
    list_add_tail(&hook->disabled_entry, &disabled_function_hooks);
    INIT_LIST_HEAD(&hook->enabled_entry);

    return hook;
}

Breakpoint* install_breakpoint( JNIEnv* env, void* targetAddress, BREAKPOINT_CALLBACK callback )
{
    Breakpoint* bp = new_inline_function_hook(env, targetAddress, callback);
    return bp;
}

bool uninstall_inline_function_hook  ( InlineFunctionHook * hook )
{
    if(hasExceptionOccurred(hook->env))
    {
        return false;
    }
    if(hook == NULL)
    {
        LOGE("The hook pointer was NULL. Throwing Exception ... ");
        throwNewJNIException(hook->env, "java/lang/NullPointerException", "The hook pointer was NULL. This is not allowed.");
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





