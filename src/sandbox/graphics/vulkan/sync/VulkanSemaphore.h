#ifndef SANDBOX_GRAPHICS_VULKAN_SYNC_SEMAPHORE_H_
#define SANDBOX_GRAPHICS_VULKAN_SYNC_SEMAPHORE_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"

namespace sandbox {

class VulkanSemaphore : public VulkanDeviceComponent {
public:
	VulkanSemaphore(bool isExternal = false) : isExternal(isExternal), initialized(false), externalHandle(0) { addType<VulkanSemaphore>(); }
	virtual ~VulkanSemaphore() {
        vkDestroySemaphore(getDevice().getDevice(), semaphore, nullptr);
	}

    void update() {
    	if (!initialized) {
            //std::cout << "isExternal " << isExternal << " " << externalHandle << std::endl;

            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            if (isExternal) {
                VkExportSemaphoreCreateInfo externalSemaphoreInfo{};
                externalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
#ifdef WIN32
                externalSemaphoreInfo.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else 
                externalSemaphoreInfo.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;
#endif
                semaphoreInfo.pNext = NULL;
                semaphoreInfo.pNext = &externalSemaphoreInfo;
            }
            else {
                semaphoreInfo.pNext = NULL;
            }

            if (vkCreateSemaphore(getDevice().getDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphore");
            }

            if (isExternal) {
#ifdef WIN32
                VkSemaphoreGetWin32HandleInfoKHR semaphoreGet;
                semaphoreGet.sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR;
                semaphoreGet.pNext = NULL;
                semaphoreGet.semaphore = semaphore;
                semaphoreGet.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT;
                if (getDevice().getInstance().getSemaphoreWin32KHR(getDevice().getDevice(), &semaphoreGet, &externalHandle) != VK_SUCCESS) {
                    throw std::runtime_error("failed to getSemaphoreWin32KHR!");
                }
#else 
                VkSemaphoreGetFdInfoKHR semaphoreGet;
                semaphoreGet.sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR;
                semaphoreGet.pNext = NULL;
                semaphoreGet.semaphore = semaphore;
                semaphoreGet.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT;

                if (getDevice().getInstance().getSemaphoreFdKHR(getDevice().getDevice(), &semaphoreGet, &externalHandle) != VK_SUCCESS) {
                    throw std::runtime_error("failed to vkGetSemaphoreFdKHR!");
                }
#endif
            }

    		initialized = true;
    	}
    }

    VkSemaphore getSemaphore() const { return semaphore; }

#ifdef WIN32
    HANDLE getExternalHandle() const {
        return externalHandle;
    }
#else 
    int getExternalHandle() const {
        return externalHandle;
    }
#endif

private:

#ifdef WIN32
    HANDLE externalHandle;
#else 
    int externalHandle;
#endif


    VkSemaphore semaphore;
    bool initialized;
    bool isExternal;
};

}

#endif