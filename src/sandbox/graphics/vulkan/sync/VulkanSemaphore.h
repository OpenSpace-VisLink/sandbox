#ifndef SANDBOX_GRAPHICS_VULKAN_SYNC_SEMAPHORE_H_
#define SANDBOX_GRAPHICS_VULKAN_SYNC_SEMAPHORE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"

namespace sandbox {

class VulkanSemaphore : public VulkanDeviceComponent {
public:
	VulkanSemaphore() : initialized(false) { addType<VulkanSemaphore>(); }
	virtual ~VulkanSemaphore() {
        vkDestroySemaphore(getDevice().getDevice(), semaphore, nullptr);
	}

    void update() {
    	if (!initialized) {
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            if (vkCreateSemaphore(getDevice().getDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }

    		initialized = true;
    	}
    }

    VkSemaphore getSemaphore() const { return semaphore; }

#ifdef WIN32
    HANDLE getExternalHandle() {
        HANDLE externalHandle;
        VkSemaphoreGetWin32InfoKHR semaphoreGet;
        semaphoreGet.sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_INFO_KHR;
        semaphoreGet.pNext = NULL;
        semaphoreGet.semaphore = semaphore;
        semaphoreGet.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        getSemaphoreFdKHR(getDevice().getDevice(), &semaphoreGet, &externalHandle);
        return externalHandle;
    }
#else 
    int getExternalHandle() {
        int externalHandle;
        VkSemaphoreGetFdInfoKHR semaphoreGet;
        semaphoreGet.sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR;
        semaphoreGet.pNext = NULL;
        semaphoreGet.semaphore = semaphore;
        semaphoreGet.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;
        getSemaphoreFdKHR(getDevice().getDevice(), &semaphoreGet, &externalHandle);
        return externalHandle;
    }
#endif

private:

#ifdef WIN32
    VkResult getSemaphoreWin32HandleKHR (const VkSemaphoreGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) {
        auto func = (PFN_vkGetSemaphoreWin32HandleKHR) vkGetInstanceProcAddr(getDevice().getInstance().getInstance(), "vkGetSemaphoreWin32HandleKHR");//VkDevice device, const VkMemoryGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle
        if (func != nullptr) {
            return func(device, pGetWin32HandleInfo, pHandle);
        }
        return VK_SUCCESS;
    }
#else 
    VkResult getSemaphoreFdKHR(VkDevice device, VkSemaphoreGetFdInfoKHR* pGetFdInfo, int* handle) {
        auto func = (PFN_vkGetSemaphoreFdKHR) vkGetInstanceProcAddr(getDevice().getInstance().getInstance(), "vkGetSemaphoreFdKHR");
        if (func != nullptr) {
            return func(device, pGetFdInfo, handle);
        }    
        return VK_SUCCESS;  
    }

#endif


    VkSemaphore semaphore;
    bool initialized;
};

}

#endif