/************************************************************************************
*                                                                                   *
*   canvas - https://github.com/fkoppe/canvas                                       *
*   ************************************************************************        *
*                                                                                   *
*   Copyright (C) 2022 Felix Koppe <fkoppe@web.de>                                  *
*                                                                                   *
*   This program is free software: you can redistribute it and/or modify            *
*   it under the terms of the GNU Affero General Public License as published        *
*   by the Free Software Foundation, either version 3 of the License, or            *
*   (at your option) any later version.                                             *
*                                                                                   *
*   This program is distributed in the hope that it will be useful,                 *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   *
*   GNU Affero General Public License for more details.                             *
*                                                                                   *
*   You should have received a copy of the GNU Affero General Public License        *
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.          *
*                                                                                   *
************************************************************************************/

#ifndef ___CNVX___RENDERER_PRIVATE_H
#define ___CNVX___RENDERER_PRIVATE_H

#include "cnvx/renderer/renderer.h"

#include "vulkan/vulkan.h"

typedef struct CNVX_Renderer_Shader_PRIVATE
{
    CNVX_Renderer_Shader_Type type;
    uint32_t size;
    const char* data;
} CNVX_Renderer_Shader_PRIVATE;

typedef struct CNVX_Renderer_PRIVATE
{
    size_t id;
    void* name;
    const char* app_name;
    SPRX_VERSION app_version;
    const char* engine_name;
    SPRX_VERSION engine_version;
    size_t width;
    size_t height;
    void* shader_vec;
    void* logger;
    void* window;
    bool started_is;
    bool prepared_is;
    CNVX_Renderer_Settings settings;
    struct
    {
        VkFormat format_use;

        VkInstance instance;

        uint32_t physical_device_count;
        VkPhysicalDevice* physical_device_all;
        size_t physical_device_use_index;
        VkPhysicalDeviceProperties* physical_device_properties_all;
        VkPhysicalDeviceFeatures* physical_device_features_all;
        VkPhysicalDeviceMemoryProperties* physical_device_memory_properties_all;

        uint32_t queue_family_count;
        VkQueueFamilyProperties* queue_family_properties;
        VkDevice device;
        VkQueue queue;
        uint32_t queue_family_use_index;

        VkSurfaceKHR surface;
        VkBool32 surface_support_is;
        VkSurfaceCapabilitiesKHR surface_capabilities;
        VkSurfaceFormatKHR* surface_format_all;
        VkPresentModeKHR* surface_present_mode_all;

        VkSwapchainKHR swapchain;
        uint32_t swapchain_image_all_count;
        VkImage* swapchain_image_all;

        VkImageView* image_view_all;

        uint32_t shader_module_count;
        VkShaderModule* shader_module_all;

        VkPipelineLayout pipeline_layout;
        VkRenderPass renderer_pass;
        VkPipeline pipeline;

        VkFramebuffer* framebuffer_all;

        VkCommandPool commandpool;

        VkCommandBuffer* commandbuffer_all;

        VkSemaphore semaphore_image_available;
        VkSemaphore semaphore_rendering_done;
    } vk;
} CNVX_Renderer_PRIVATE;

#endif // ___CNVX___RENDERER_PRIVATE_H
