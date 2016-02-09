//
// Created by Lukas on 8/19/2015.
//

#include "../util/list.h"
#include "trappoint_interface.h"
#include "../util/memory.h"
#include "../util/error.h"
#include "../logging.h"
#include "../abi/abi_interface.h"
#include "self_patching_trappoint.h"

static struct list_head installed_self_patching_trappoints;

struct SelfPatchingTrappointInfo
{
    struct list_head        installed_entry;;
    bool                    enabled;

    HOOKCALLBACK            PostTrigger_Handler;
    void*                   PostTrigger_Arg;
    HOOKCALLBACK            PrePatch_Handler;
    void*                   PrePatch_Arg;

    uint32_t                trap_method;
    void*                   trap_addr;

    void*                   primary_trappoint;
    void*                   patch_trappoint;

    PATCH_ADDRESS_EXTRACTOR patch_at;
};

void init_self_patching_trappoints()
{
    INIT_LIST_HEAD(&installed_self_patching_trappoints);
}
void destroy_self_patching_trappoints()
{

}

static void patch_handler       (void* address, ucontext_t* ctx, void* arg);
static void primary_handler     (void *address, ucontext_t *ctx, void *arg);

static void patch_handler       (void* address, ucontext_t* ctx, void* arg)
{
    struct SelfPatchingTrappointInfo * self = arg;

    // We are now uninstalled, remove our reference so we don't get any horrible ideas
    self->patch_trappoint = NULL;

    if(!self->enabled)
    {
        // If the breakpoint was disabled and not re-enabled we simply do nothing. It's up to
        // whoever disabled the breakpoint to re-enable it.
        return;
    }

    // Execute our handler before reinstalling the primary-trappoint so the handler gets to see
    // the unmodified state.
    if(self->PrePatch_Handler != NULL)
    {
        self->PrePatch_Handler(address, ctx, self->PrePatch_Arg);
    }

    // Re-enable the primary trappoint to reset our breakpoint.
    void* reset = trappoint_Install(self->trap_addr, self->trap_method, primary_handler, self);
    if(reset == NULL)
    {
        set_last_error("Could not reinstall our primary trappoint, I surrender.");
        return;
    }
    // the primary trappoint should have been deleted as soon as the breakpoint was triggered.
    CHECK(self->primary_trappoint == NULL);
    self->primary_trappoint = reset;
}
static void primary_handler(void *address, ucontext_t *ctx, void *arg)
{
    struct SelfPatchingTrappointInfo * self = arg;

    // We are now uninstalled, remove our reference so we don't get any horrible ideas
    self->primary_trappoint = NULL;

    // If we just triggered how can we not be enabled?? If this triggers we have a bug
    CHECK(self->enabled);

    // Execute our handler before installing the patch-trappoint so the handler gets to see
    // the unmodified state.
    if(self->PostTrigger_Handler != NULL)
    {
        self->PostTrigger_Handler(address, ctx, self->PostTrigger_Arg);
    }

    void* patch_instruction_pointer = self->patch_at(ctx);
    void* patcher = trappoint_Install(patch_instruction_pointer, self->trap_method, patch_handler,
                                      self);
    if(patcher == NULL)
    {
        set_last_error("Could not install patch trappoint to reset the original one, I surrender.");
        return;
    }
    // We should not be able to have a patch trappoint if we just triggered our original one.
    CHECK(self->patch_trappoint == NULL);
    self->patch_trappoint = patcher;
}

void* SelfPatchingTrappoint_Install(void *address, uint32_t trap_method,
                                    HOOKCALLBACK PostTrigger_Handler, void *PostTrigger_Arg,
                                    HOOKCALLBACK PrePatch_Handler, void* PrePatch_Arg,
                                    PATCH_ADDRESS_EXTRACTOR patch_at)
{
    CHECK(address != NULL);
    CHECK(patch_at != NULL);

    struct SelfPatchingTrappointInfo * new_self = allocate_memory_chunk(sizeof(struct SelfPatchingTrappointInfo));
    if(new_self != NULL)
    {
        void *entry_trappoint = trappoint_Install(address, trap_method, primary_handler,
                                                  (void *) new_self);
        if (entry_trappoint != NULL)
        {
            new_self->PostTrigger_Handler = PostTrigger_Handler;
            new_self->PostTrigger_Arg = PostTrigger_Arg;
            new_self->PrePatch_Handler = PrePatch_Handler;
            new_self->PrePatch_Arg = PrePatch_Arg;
            new_self->patch_trappoint = NULL;
            new_self->primary_trappoint = entry_trappoint;
            new_self->enabled = true;
            new_self->patch_at = patch_at;
            list_add(&new_self->installed_entry, &installed_self_patching_trappoints);

            // Succesful creation
            return new_self;
        }
        set_last_error("Error creating the initial trappoint for the breakpoint.");
        free_memory_chunk(new_self);
        return NULL;
    }
    set_last_error("Error while allocating memory for breakpoint structure.");
    return NULL;
}

bool SelfPatchingTrappoint_Enable(void *p)
{
    CHECK(p != NULL);
    struct SelfPatchingTrappointInfo * self = p;
    if(self->enabled)
    {
        // Already enabled nothing to do here.
        return false;
    }
    // if we are disabled that means our primary and secondary
    // trappoints should have been cleaned up
    CHECK(self->primary_trappoint == NULL && self->patch_trappoint == NULL);

    void* trap = trappoint_Install(self->trap_addr, self->trap_method, primary_handler, self);
    if(trap == NULL)
    {
        set_last_error("Error re-installing primary-trappoint to re-enable the breakpoint.");
        return false;
    }
    self->primary_trappoint = trap;
    self->enabled = true;
    return true;
}
bool SelfPatchingTrappoint_Disable(void *p)
{
    CHECK(p != NULL);
    struct SelfPatchingTrappointInfo * self = p;
    if(!self->enabled)
    {
        // Already disabled, don't do anything.
        return false;
    }

    if(self->primary_trappoint != NULL)
    {
        trappoint_Uninstall(self->primary_trappoint);
        self->primary_trappoint = NULL;
    }
    if(self->patch_trappoint != NULL)
    {
        trappoint_Uninstall(self->patch_trappoint);
        self->primary_trappoint = NULL;
    }

    self->enabled = false;
    return true;
}

void SelfPatchingTrappoint_Uninstall(void *p)
{
    if(p == NULL)
    {
        LOGW("Why are you uninstalling breakpoints that are NULL? Are you sure this is what you want?");
        return;
    }
    struct SelfPatchingTrappointInfo * self = p;
    if(self->primary_trappoint != NULL)
    {
        trappoint_Uninstall(self->primary_trappoint);
    }
    if(self->patch_trappoint != NULL)
    {
        trappoint_Uninstall(self->patch_trappoint);
    }
    free_memory_chunk(self);
}