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

#include "cnvx/logger/logger.h"
#include "cnvx/renderer/Private/renderer_PRIVATE.h"
#include "cnvx/renderer/Private/vulkan_PRIVATE.h"
#include "cnvx/window/window.h"

#include "sprx/container/string.h"
#include "sprx/container/vector.h"
#include "sprx/core/assert.h"
#include "sprx/core/core.h"
#include "sprx/core/terminate.h"

#include "GLFW/glfw3.h"

#define CNVX_VULKAN_ERROR_ALLOCATION SPRX_ERROR_ALLOCATION("vulkan", NULL, NULL)
#define CNVX_VULKAN_ERROR_LOGIC(what, info, care) SPRX_ERROR_LOGIC(what, "vulkan", info, care)
#define CNVX_VULKAN_ERROR_ARGUMENT(care) SPRX_ERROR_ARGUMENT("vulkan", NULL, care)
#define CNVX_VULKAN_ERROR_NULL(info) SPRX_ERROR_NULL("vulkan", info)
#define CNVX_VULKAN_ERROR_ENUM(info) SPRX_ERROR_ENUM("vulkan", info, NULL)

#ifdef ___CNVX_DEBUG
    #define CNVX_VULKAN_SURFACE_LAYER_VALIDATION "VK_LAYER_KHRONOS_validation"
    #define CNVX_VULKAN_SURFACE_LAYER_VALIDATION_COUNT 1
#else
    #define CNVX_VULKAN_SURFACE_LAYER_VALIDATION ""
    #define CNVX_VULKAN_SURFACE_LAYER_VALIDATION_COUNT 0
#endif // ___CNVX_DEBUG

const char* canvas_vulkan_result_name_get_PRIVATE(const VkResult result_)
{
    SPRX_ASSERT(VK_RESULT_MAX_ENUM > result_, CNVX_VULKAN_ERROR_ENUM("invalid value of result"));

    switch (result_)
    {
    case VK_SUCCESS:
        return "VK_SUCCESS";
    case VK_NOT_READY:
        return "VK_NOT_READY";
    case VK_TIMEOUT:
        return "VK_TIMEOUT";
    case VK_EVENT_SET:
        return "VK_EVENT_SET";
    case VK_EVENT_RESET:
        return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
        return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN:
        return "VK_ERROR_UNKNOWN (check vulkan man)";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY/VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE/VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
    case VK_ERROR_FRAGMENTATION:
        return "VK_ERROR_FRAGMENTATION/VK_ERROR_FRAGMENTATION_EXT";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "VK_ERROR_INVALID_ - OPAQUE_CAPTURE_ADDRESS/DEVICE_ADDRESS_EXT/OPAQUE_CAPTURE_ADDRESS_KHR/OPAQUE_CAPTURE_ADDRESS_KHR";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return  "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
        return  "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_NOT_PERMITTED_EXT:
        return "VK_ERROR_NOT_PERMITTED_EXT";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_PIPELINE_COMPILE_REQUIRED_EXT:
        return "VK_PIPELINE_COMPILE_REQUIRED_EXT/VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT";
    default:
        SPRX_ABORT_ERROR(SPRX_ERROR_BOUNDS("vulkan", "invalid result", NULL));
        break;
    }
}
VkShaderStageFlagBits canvas_vulkan_shader_stage_flag_bit_get_PRIVATE(const CNVX_Renderer_Shader_Type type_)
{
    SPRX_ASSERT(___CNVX_RENDERER_SHADER_TYPE_MAX > type_, CNVX_VULKAN_ERROR_ENUM("invalid value of type"));

    switch (type_)
    {
    case CNVX_RENDERER_SHADER_TYPE_VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case CNVX_RENDERER_SHADER_TYPE_FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
        SPRX_ABORT_ERROR(SPRX_ERROR_BOUNDS("vulkan", "invalid type", NULL));
        break;
    }
}

void canvas_vulkan_assert(void* const renderer_, bool suppress_is_, const VkResult result_, const char* const file_, const char* const func_, const int line_, const char* const date_, const char* const time_, const char* const what_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));
    SPRX_ASSERT(VK_RESULT_MAX_ENUM > result_, CNVX_VULKAN_ERROR_ENUM("invalid value of result"));
    SPRX_ASSERT(NULL != file_, CNVX_VULKAN_ERROR_NULL("file"));
    SPRX_ASSERT(NULL != func_, CNVX_VULKAN_ERROR_NULL("func"));
    SPRX_ASSERT(NULL != date_, CNVX_VULKAN_ERROR_NULL("date"));
    SPRX_ASSERT(NULL != time_, CNVX_VULKAN_ERROR_NULL("time"));
    SPRX_ASSERT(NULL != what_, CNVX_VULKAN_ERROR_NULL("what"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    if (VK_SUCCESS == result_)
    {
        if (!suppress_is_)
        {
            CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: %s succeded with VK_SUCCESS", what_);
        }
    }
    else
    {
        const char* const name = canvas_vulkan_result_name_get_PRIVATE(result_);

        CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_ERROR, spore_string_substr(renderer->name, 7), "...\n-------------------------\n\nvulkan assertion failed\n\nfile:\t%s\nfunc:\t%s\nline:\t%i\ndate:\t%s\ntime:\t%s\n\nwhat:\t%s failed with %s\n\n-------------------------\n", file_, func_, line_, date_, time_, what_, name);

        exit(-1);
    }
}

void canvas_vulkan_assertf(void* const renderer_, bool suppress_is_, const VkResult result_, const char* const file_, const char* const func_, const int line_, const char* const date_, const char* const time_, const char* const format_, ...)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));
    SPRX_ASSERT(VK_RESULT_MAX_ENUM > result_, CNVX_VULKAN_ERROR_ENUM("invalid value of result"));
    SPRX_ASSERT(NULL != file_, CNVX_VULKAN_ERROR_NULL("file"));
    SPRX_ASSERT(NULL != func_, CNVX_VULKAN_ERROR_NULL("func"));
    SPRX_ASSERT(NULL != date_, CNVX_VULKAN_ERROR_NULL("date"));
    SPRX_ASSERT(NULL != time_, CNVX_VULKAN_ERROR_NULL("time"));
    SPRX_ASSERT(NULL != format_, CNVX_VULKAN_ERROR_NULL("format"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    va_list args;
    va_start(args, format_);

    void* const string = spore_string_new_cstr("vulkan: ");
    spore_string_append_v(string, format_, args);

    va_end(args);

    if (VK_SUCCESS == result_)
    {
        if (!suppress_is_)
        {
            spore_string_append_cstr(string, " succeded with VK_SUCCESS");

            CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), spore_string_cstr(string));
        }
    }
    else
    {
        const char* const name = canvas_vulkan_result_name_get_PRIVATE(result_);

        CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_ERROR, spore_string_substr(renderer->name, 7), "...\n-------------------------\n\nvulkan assertion failed\n\nfile:\t%s\nfunc:\t%s\nline:\t%i\ndate:\t%s\ntime:\t%s\n\nwhat:\t%s failed with %s\n\n-------------------------\n", file_, func_, line_, date_, time_, spore_string_substr(string, 8), name);

        exit(-1);
    }

    spore_string_delete(string);
}

void canvas_vulkan_instance_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: instance creation");

    VkApplicationInfo application_info;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = NULL;
    application_info.pApplicationName = renderer->app_name;
    application_info.applicationVersion = renderer->app_version;
    application_info.pEngineName = renderer->engine_name;
    application_info.engineVersion = renderer->engine_version;
    application_info.apiVersion = VK_API_VERSION_1_2;

    uint32_t enabled_layers_count = 0 + CNVX_VULKAN_SURFACE_LAYER_VALIDATION_COUNT;
    const char* enabled_layers[] = { CNVX_VULKAN_SURFACE_LAYER_VALIDATION };

    uint32_t enabled_extentions_count = 0;
    const char** enabled_extentions = glfwGetRequiredInstanceExtensions(&enabled_extentions_count);

    VkInstanceCreateInfo instance_create_info;
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = NULL;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = enabled_layers_count;
    instance_create_info.ppEnabledLayerNames = enabled_layers;
    instance_create_info.enabledExtensionCount = enabled_extentions_count;
    instance_create_info.ppEnabledExtensionNames = enabled_extentions;

    VkResult result = vkCreateInstance(&instance_create_info, NULL, &renderer->vk.instance);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateInstance");
}

void canvas_vulkan_instance_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    vkDestroyInstance(renderer->vk.instance, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroyInstance");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: instance destruction");
}

void canvas_vulkan_physical_devices_enumerate(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: physical device enumeration");

    renderer->vk.physical_device_count = 0;
    VkResult result = vkEnumeratePhysicalDevices(renderer->vk.instance, &renderer->vk.physical_device_count, NULL);
    CNVX_VULKAN_ASSERT(renderer, result, "vkEnumeratePhysicalDevices (1/2)");

    SPRX_ASSERT(0 != renderer->vk.physical_device_count, CNVX_VULKAN_ERROR_LOGIC("could not continue", "physical device count has to be >0", NULL));

    renderer->vk.physical_device_all = malloc(sizeof(*renderer->vk.physical_device_all) * renderer->vk.physical_device_count);
    SPRX_ASSERT(NULL != renderer->vk.physical_device_all, CNVX_VULKAN_ERROR_ALLOCATION);

    result = vkEnumeratePhysicalDevices(renderer->vk.instance, &renderer->vk.physical_device_count, renderer->vk.physical_device_all);
    CNVX_VULKAN_ASSERT(renderer, result, "vkEnumeratePhysicalDevices (2/2)");

    renderer->vk.physical_device_properties_all = malloc(sizeof(*renderer->vk.physical_device_properties_all) * renderer->vk.physical_device_count);
    SPRX_ASSERT(NULL != renderer->vk.physical_device_properties_all, CNVX_VULKAN_ERROR_ALLOCATION);

    renderer->vk.physical_device_features_all = malloc(sizeof(*renderer->vk.physical_device_features_all) * renderer->vk.physical_device_count);
    SPRX_ASSERT(NULL != renderer->vk.physical_device_features_all, CNVX_VULKAN_ERROR_ALLOCATION);

    renderer->vk.physical_device_memory_properties_all = malloc(sizeof(*renderer->vk.physical_device_memory_properties_all) * renderer->vk.physical_device_count);
    SPRX_ASSERT(NULL != renderer->vk.physical_device_memory_properties_all, CNVX_VULKAN_ERROR_ALLOCATION);

    for (uint32_t i = 0; i < renderer->vk.physical_device_count; i++)
    {
        vkGetPhysicalDeviceProperties(renderer->vk.physical_device_all[i], &renderer->vk.physical_device_properties_all[i]);
        vkGetPhysicalDeviceFeatures(renderer->vk.physical_device_all[i], &renderer->vk.physical_device_features_all[i]);
        vkGetPhysicalDeviceMemoryProperties(renderer->vk.physical_device_all[i], &renderer->vk.physical_device_memory_properties_all[i]);
    }

    renderer->vk.physical_device_use_index = 0; //@TODO
}

void canvas_vulkan_physical_devices_denumerate(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    free(renderer->vk.physical_device_all);

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: physical device denumeration");
}

void canvas_vulkan_device_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: device creation");

    renderer->vk.queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(renderer->vk.physical_device_all[0], &renderer->vk.queue_family_count, NULL); //@TODO

    SPRX_ASSERT(0 != renderer->vk.queue_family_count, CNVX_VULKAN_ERROR_LOGIC("could not continue", "queue family count has to be >0", NULL));

    renderer->vk.queue_family_properties = malloc(sizeof(*renderer->vk.queue_family_properties) * renderer->vk.queue_family_count);
    SPRX_ASSERT(NULL != renderer->vk.queue_family_properties, CNVX_VULKAN_ERROR_ALLOCATION);

    vkGetPhysicalDeviceQueueFamilyProperties(renderer->vk.physical_device_all[0], &renderer->vk.queue_family_count, renderer->vk.queue_family_properties); //@TODO

    const float queue_priorities[] = { 1.0f };

    renderer->vk.queue_family_use_index = 0; //@TOD

    VkDeviceQueueCreateInfo device_queue_create_info;
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = NULL;
    device_queue_create_info.flags = 0;
    device_queue_create_info.queueFamilyIndex = renderer->vk.queue_family_use_index;
    device_queue_create_info.queueCount = 1;//@TODO
    device_queue_create_info.pQueuePriorities = queue_priorities;

    uint32_t enabled_layers_count = 0;
    const char* enabled_layers[] = { "" };

    uint32_t enabled_extentions_count = 1;
    const char* enabled_extentions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkPhysicalDeviceFeatures enabled_physical_device_features = { VK_FALSE };

    VkDeviceCreateInfo device_create_info;
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = NULL;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.enabledLayerCount = enabled_layers_count;
    device_create_info.ppEnabledLayerNames = enabled_layers;
    device_create_info.enabledExtensionCount = enabled_extentions_count;
    device_create_info.ppEnabledExtensionNames = enabled_extentions;
    device_create_info.pEnabledFeatures = &enabled_physical_device_features;

    VkResult result = vkCreateDevice(renderer->vk.physical_device_all[0], &device_create_info, NULL, &renderer->vk.device);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateDevice");

    vkGetDeviceQueue(renderer->vk.device, renderer->vk.queue_family_use_index, 0, &renderer->vk.queue); //@TODO
}

void canvas_vulkan_device_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    vkDestroyDevice(renderer->vk.device, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroyDevice");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: device destruction");
}

void canvas_vulkan_surface_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: surface creation");

    VkResult result = glfwCreateWindowSurface(renderer->vk.instance, canvas_window_handle_get(renderer->window), NULL, &renderer->vk.surface);
    CNVX_VULKAN_ASSERT(renderer, result, "glfwCreateWindowSurface");

    result = vkGetPhysicalDeviceSurfaceSupportKHR(renderer->vk.physical_device_all[renderer->vk.physical_device_use_index], renderer->vk.queue_family_use_index, renderer->vk.surface, &renderer->vk.surface_support_is);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetPhysicalDeviceSurfaceSupportKHR");

    if (renderer->vk.surface_support_is)
    {
        CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: surface support is available");
    }
    else
    {
        CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_ERROR, spore_string_substr(renderer->name, 7), "vulkan: no surface support is not available");
    }

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->vk.physical_device_all[renderer->vk.physical_device_use_index], renderer->vk.surface, &renderer->vk.surface_capabilities);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

    uint32_t surface_formats_count = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->vk.physical_device_all[renderer->vk.physical_device_use_index], renderer->vk.surface, &surface_formats_count, NULL);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetPhysicalDeviceSurfaceFormatsKHR (1/2)");

    SPRX_ASSERT(0 != surface_formats_count, CNVX_VULKAN_ERROR_LOGIC("could not continue", "surface formats count has to be >0", NULL));

    renderer->vk.surface_format_all = malloc(sizeof(*renderer->vk.surface_format_all) * surface_formats_count);
    SPRX_ASSERT(NULL != renderer->vk.surface_format_all, CNVX_VULKAN_ERROR_ALLOCATION);

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->vk.physical_device_all[renderer->vk.physical_device_use_index], renderer->vk.surface, &surface_formats_count, renderer->vk.surface_format_all);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetPhysicalDeviceSurfaceFormatsKHR (2/2)");

    renderer->vk.format_use = VK_FORMAT_B8G8R8A8_UNORM;

    uint32_t surface_present_modes_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->vk.physical_device_all[renderer->vk.physical_device_use_index], renderer->vk.surface, &surface_present_modes_count, NULL);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetPhysicalDeviceSurfacePresentModesKHR (1/2)");

    SPRX_ASSERT(0 != surface_present_modes_count, CNVX_VULKAN_ERROR_LOGIC("could not continue", "surface present modes count has to be >0", NULL));

    renderer->vk.surface_present_mode_all = malloc(sizeof(*renderer->vk.surface_present_mode_all) * surface_present_modes_count);
    SPRX_ASSERT(NULL != renderer->vk.surface_present_mode_all, CNVX_VULKAN_ERROR_ALLOCATION);

    vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->vk.physical_device_all[renderer->vk.physical_device_use_index], renderer->vk.surface, &surface_present_modes_count, renderer->vk.surface_present_mode_all);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetPhysicalDeviceSurfacePresentModesKHR (2/2)");
}

void canvas_vulkan_surface_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    free(renderer->vk.surface_present_mode_all);
    free(renderer->vk.surface_format_all);

    vkDestroySurfaceKHR(renderer->vk.instance, renderer->vk.surface, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroySurfaceKHR");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: surface destruction");
}

void canvas_vulkan_swapchain_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: swapchain creation");

    VkSwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = NULL;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = renderer->vk.surface;
    swapchain_create_info.minImageCount = 1; //@TODO
    swapchain_create_info.imageFormat = renderer->vk.format_use;
    swapchain_create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; //@TODO

    swapchain_create_info.imageExtent.width = renderer->width;
    swapchain_create_info.imageExtent.height = renderer->height;
    CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: image extend is %ux%u", renderer->width, renderer->height);

    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //@TODO
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //@TODO
    swapchain_create_info.queueFamilyIndexCount = 0; //@TODO
    swapchain_create_info.pQueueFamilyIndices = NULL; //@TODO
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR; //@TODO
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; //@TODO

    VkResult result = vkCreateSwapchainKHR(renderer->vk.device, &swapchain_create_info, NULL, &renderer->vk.swapchain);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateSwapchainKHR");

    renderer->vk.swapchain_image_all_count = 0;
    result = vkGetSwapchainImagesKHR(renderer->vk.device, renderer->vk.swapchain, &renderer->vk.swapchain_image_all_count, NULL);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetSwapchainImagesKHR (1/2)");

    SPRX_ASSERT(0 != renderer->vk.swapchain_image_all_count, CNVX_VULKAN_ERROR_LOGIC("could not continue", "swapchain image count has to be >0", NULL));

    renderer->vk.swapchain_image_all = malloc(sizeof(*renderer->vk.swapchain_image_all) * renderer->vk.swapchain_image_all_count);
    SPRX_ASSERT(NULL != renderer->vk.swapchain_image_all, CNVX_VULKAN_ERROR_ALLOCATION);

    vkGetSwapchainImagesKHR(renderer->vk.device, renderer->vk.swapchain, &renderer->vk.swapchain_image_all_count, renderer->vk.swapchain_image_all);
    CNVX_VULKAN_ASSERT(renderer, result, "vkGetSwapchainImagesKHR (2/2)");
}

void canvas_vulkan_swapchain_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    free(renderer->vk.swapchain_image_all);

    vkDestroySwapchainKHR(renderer->vk.device, renderer->vk.swapchain, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroySwapchainKHR");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: swapchain destruction");
}

void canvas_vulkan_imageviews_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: imageviews creation");

    VkImageViewCreateInfo image_view_create_info;
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = NULL;
    image_view_create_info.flags = 0;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = renderer->vk.format_use;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    renderer->vk.image_view_all = malloc(sizeof(*renderer->vk.image_view_all) * renderer->vk.swapchain_image_all_count);
    SPRX_ASSERT(NULL != renderer->vk.image_view_all, CNVX_VULKAN_ERROR_ALLOCATION);

    for (uint32_t i = 0; i < renderer->vk.swapchain_image_all_count; i++)
    {
        image_view_create_info.image = renderer->vk.swapchain_image_all[i];

        VkResult result = vkCreateImageView(renderer->vk.device, &image_view_create_info, NULL, &renderer->vk.image_view_all[i]);
        CNVX_VULKAN_ASSERTF(renderer, result, "vkGetSwapchainImagesKHR (%u/%u)", i + 1, renderer->vk.swapchain_image_all_count);
    }
}

void canvas_vulkan_imageviews_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    for (uint32_t i = 0; i < renderer->vk.swapchain_image_all_count; i++)
    {
        vkDestroyImageView(renderer->vk.device, renderer->vk.image_view_all[i], NULL);
        CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vkGetSwapchainImagesKHR (%u/%u)", i + 1, renderer->vk.swapchain_image_all_count);
    }

    free(renderer->vk.image_view_all);

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: imageviews destruction");
}

void canvas_vulkan_shader_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: shader creation");

    renderer->vk.shader_module_count = SPRX_MIN(spore_vector_capacity(renderer->shader_vec), UINT32_MAX);

    if (renderer->vk.shader_module_count < 2)
    {
        CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_ERROR, spore_string_substr(renderer->name, 7), "vulkan: required shader missing");
    }

    renderer->vk.shader_module_all = malloc(sizeof(*renderer->vk.shader_module_all) * renderer->vk.shader_module_count);
    SPRX_ASSERT(renderer->vk.shader_module_all, CNVX_VULKAN_ERROR_ALLOCATION);

    VkShaderModuleCreateInfo shader_module_create_info;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = NULL;
    shader_module_create_info.flags = 0;

    for (size_t i = 0; i < renderer->vk.shader_module_count; i++)
    {
        shader_module_create_info.codeSize = SPRX_VECTOR_AT(renderer->shader_vec, i, CNVX_Renderer_Shader_PRIVATE)->size;
        shader_module_create_info.pCode = (const uint32_t*)SPRX_VECTOR_AT(renderer->shader_vec, i, CNVX_Renderer_Shader_PRIVATE)->data;

        VkResult result = vkCreateShaderModule(renderer->vk.device, &shader_module_create_info, NULL, &renderer->vk.shader_module_all[i]);
        CNVX_VULKAN_ASSERTF(renderer, result, "vkCreateShaderModule (%u/%u)", i + 1, renderer->vk.shader_module_count);
    }
}

void canvas_vulkan_shader_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    for (size_t i = 0; i < renderer->vk.shader_module_count; i++)
    {
        vkDestroyShaderModule(renderer->vk.device, renderer->vk.shader_module_all[i], NULL);
        CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vkDestroyShaderModule (%u/%u)", i + 1, renderer->vk.shader_module_count);
    }

    free(renderer->vk.shader_module_all);

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: shader destruction");
}

void canvas_vulkan_pipeline_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: pipeline creation");

    VkPipelineShaderStageCreateInfo* const pipeline_shader_stage_create_info_all = malloc(sizeof(*pipeline_shader_stage_create_info_all) * renderer->vk.shader_module_count);
    SPRX_ASSERT(NULL != pipeline_shader_stage_create_info_all, CNVX_VULKAN_ERROR_ALLOCATION);

    for (size_t i = 0; i < renderer->vk.shader_module_count; i++)
    {
        VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info;
        pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_shader_stage_create_info.pNext = NULL;
        pipeline_shader_stage_create_info.flags = 0;
        pipeline_shader_stage_create_info.stage = canvas_vulkan_shader_stage_flag_bit_get_PRIVATE(SPRX_VECTOR_AT(renderer->shader_vec, i, CNVX_Renderer_Shader_PRIVATE)->type);
        pipeline_shader_stage_create_info.module = renderer->vk.shader_module_all[i];
        pipeline_shader_stage_create_info.pName = "main";
        pipeline_shader_stage_create_info.pSpecializationInfo = NULL;

        pipeline_shader_stage_create_info_all[i] = pipeline_shader_stage_create_info;
    }

    VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info;
    pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipeline_vertex_input_state_create_info.pNext = NULL;
    pipeline_vertex_input_state_create_info.flags = 0;
    pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
    pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = NULL;
    pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = NULL;

    VkPipelineInputAssemblyStateCreateInfo pipelien_input_assembly_state_create_info;
    pipelien_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelien_input_assembly_state_create_info.pNext = NULL;
    pipelien_input_assembly_state_create_info.flags = 0;
    pipelien_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelien_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = renderer->width;
    viewport.height = renderer->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = renderer->width;
    scissor.extent.height = renderer->height;

    VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info;
    pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipeline_viewport_state_create_info.pNext = NULL;
    pipeline_viewport_state_create_info.flags = 0;
    pipeline_viewport_state_create_info.viewportCount = 1;
    pipeline_viewport_state_create_info.pViewports = &viewport;
    pipeline_viewport_state_create_info.scissorCount = 1;
    pipeline_viewport_state_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo pipeline_rasterisation_state_create_info;
    pipeline_rasterisation_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipeline_rasterisation_state_create_info.pNext = NULL;
    pipeline_rasterisation_state_create_info.flags = 0;
    pipeline_rasterisation_state_create_info.depthClampEnable = VK_FALSE;
    pipeline_rasterisation_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    pipeline_rasterisation_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    pipeline_rasterisation_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    pipeline_rasterisation_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipeline_rasterisation_state_create_info.depthBiasEnable = VK_FALSE;
    pipeline_rasterisation_state_create_info.depthBiasConstantFactor = 0.0f;
    pipeline_rasterisation_state_create_info.depthBiasClamp = 0.0f;
    pipeline_rasterisation_state_create_info.depthBiasSlopeFactor = 0.0f;
    pipeline_rasterisation_state_create_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info;
    pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipeline_multisample_state_create_info.pNext = NULL;
    pipeline_multisample_state_create_info.flags = 0;
    pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    pipeline_multisample_state_create_info.minSampleShading = 1.0f;
    pipeline_multisample_state_create_info.pSampleMask = NULL;
    pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state;
    pipeline_color_blend_attachment_state.blendEnable = VK_TRUE;
    pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info;
    pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipeline_color_blend_state_create_info.pNext = NULL;
    pipeline_color_blend_state_create_info.flags = 0;
    pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
    pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_NO_OP;
    pipeline_color_blend_state_create_info.attachmentCount = 1;
    pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;
    pipeline_color_blend_state_create_info.blendConstants[0] = 0.0f;
    pipeline_color_blend_state_create_info.blendConstants[1] = 0.0f;
    pipeline_color_blend_state_create_info.blendConstants[2] = 0.0f;
    pipeline_color_blend_state_create_info.blendConstants[3] = 0.0f;

    VkDynamicState dynamic_state_all[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info;
    pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipeline_dynamic_state_create_info.pNext = NULL;
    pipeline_dynamic_state_create_info.flags = 0;
    pipeline_dynamic_state_create_info.dynamicStateCount = 2;
    pipeline_dynamic_state_create_info.pDynamicStates = dynamic_state_all;

    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = NULL;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = NULL;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = NULL;

    VkResult result = vkCreatePipelineLayout(renderer->vk.device, &pipeline_layout_create_info, NULL, &renderer->vk.pipeline_layout);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreatePipelineLayout");

    VkAttachmentDescription attachment_description;
    attachment_description.flags = 0;
    attachment_description.format = renderer->vk.format_use;
    attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachment_refference;
    attachment_refference.attachment = 0;
    attachment_refference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description;
    subpass_description.flags = 0;
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.inputAttachmentCount = 0;
    subpass_description.pInputAttachments = NULL;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &attachment_refference;
    subpass_description.pResolveAttachments = NULL;
    subpass_description.pDepthStencilAttachment = NULL;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pPreserveAttachments = NULL;

    VkSubpassDependency subpass_dependency;
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo render_pass_create_info;
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.pNext = NULL;
    render_pass_create_info.flags = 0;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &attachment_description;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &subpass_dependency;

    result = vkCreateRenderPass(renderer->vk.device, &render_pass_create_info, NULL, &renderer->vk.renderer_pass);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateRenderPass");

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pNext = NULL;
    graphics_pipeline_create_info.flags = 0;
    graphics_pipeline_create_info.stageCount = spore_vector_capacity(renderer->shader_vec);
    graphics_pipeline_create_info.pStages = pipeline_shader_stage_create_info_all;
    graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &pipelien_input_assembly_state_create_info;
    graphics_pipeline_create_info.pTessellationState = NULL;
    graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterisation_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = NULL;
    graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = &pipeline_dynamic_state_create_info;
    graphics_pipeline_create_info.layout = renderer->vk.pipeline_layout;
    graphics_pipeline_create_info.renderPass = renderer->vk.renderer_pass;
    graphics_pipeline_create_info.subpass = 0;
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_create_info.basePipelineIndex = -1;

    result = vkCreateGraphicsPipelines(renderer->vk.device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, NULL, &renderer->vk.pipeline);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateGraphicsPipelines");

    free(pipeline_shader_stage_create_info_all);
}

void canvas_vulkan_pipeline_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: pipeline destruction");

    vkDestroyPipeline(renderer->vk.device, renderer->vk.pipeline, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroyPipeline");

    vkDestroyRenderPass(renderer->vk.device, renderer->vk.renderer_pass, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroyRenderPass");

    vkDestroyPipelineLayout(renderer->vk.device, renderer->vk.pipeline_layout, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroyPipelineLayout");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: pipeline destruction");
}

void canvas_vulkan_framebuffer_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: framebuffer destruction");

    renderer->vk.framebuffer_all = malloc(sizeof(*renderer->vk.framebuffer_all) * renderer->vk.swapchain_image_all_count);
    SPRX_ASSERT(NULL != renderer->vk.framebuffer_all, CNVX_VULKAN_ERROR_ALLOCATION);

    VkFramebufferCreateInfo frame_buffer_create_info;
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.pNext = NULL;
    frame_buffer_create_info.flags = 0;
    frame_buffer_create_info.renderPass = renderer->vk.renderer_pass;
    frame_buffer_create_info.attachmentCount = 1;
    frame_buffer_create_info.width = renderer->width;
    frame_buffer_create_info.height = renderer->height;
    frame_buffer_create_info.layers = 1;

    for (uint32_t i = 0; i < renderer->vk.swapchain_image_all_count; i++)
    {
        frame_buffer_create_info.pAttachments = &renderer->vk.image_view_all[i];

        VkResult result = vkCreateFramebuffer(renderer->vk.device, &frame_buffer_create_info, NULL, &renderer->vk.framebuffer_all[i]);
        CNVX_VULKAN_ASSERTF(renderer, result, "vkCreateFramebuffer (%u/%u)", i + 1, renderer->vk.swapchain_image_all_count);
    }
}

void canvas_vulkan_framebuffer_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    for (uint32_t i = 0; i < renderer->vk.swapchain_image_all_count; i++)
    {
        vkDestroyFramebuffer(renderer->vk.device, renderer->vk.framebuffer_all[i], NULL);
        CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vkDestroyFramebuffer (%u/%u)", i + 1, renderer->vk.swapchain_image_all_count);
    }

    free(renderer->vk.framebuffer_all);

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: framebuffer destruction");
}

void canvas_vulkan_commandpool_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: commandpool creation");

    VkCommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = NULL;
    command_pool_create_info.flags = 0;
    command_pool_create_info.queueFamilyIndex = 0; //@TODO

    VkResult result = vkCreateCommandPool(renderer->vk.device, &command_pool_create_info, NULL, &renderer->vk.commandpool);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateCommandPool");
}

void canvas_vulkan_commandpool_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    vkDestroyCommandPool(renderer->vk.device, renderer->vk.commandpool, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroyCommandPool");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: commadpool destruction");
}

void canvas_vulkan_commandbuffer_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: commandbuffer creation");

    renderer->vk.commandbuffer_all = malloc(sizeof(*renderer->vk.commandbuffer_all) * renderer->vk.swapchain_image_all_count);
    SPRX_ASSERT(NULL != renderer->vk.commandbuffer_all, CNVX_VULKAN_ERROR_ALLOCATION);

    VkCommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = NULL;
    command_buffer_allocate_info.commandPool = renderer->vk.commandpool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = renderer->vk.swapchain_image_all_count;

    VkResult result = vkAllocateCommandBuffers(renderer->vk.device, &command_buffer_allocate_info, renderer->vk.commandbuffer_all);
    CNVX_VULKAN_ASSERT(renderer, result, "vkAllocateCommandBuffers");
}

void canvas_vulkan_commandbuffer_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    vkFreeCommandBuffers(renderer->vk.device, renderer->vk.commandpool, renderer->vk.swapchain_image_all_count, renderer->vk.commandbuffer_all);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkFreeCommandBuffers");

    free(renderer->vk.commandbuffer_all);

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: commandbuffer destruction");
}

void canvas_vulkan_commandbuffer_record(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: commandbuffer recording");

    VkCommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = NULL;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    command_buffer_begin_info.pInheritanceInfo = NULL;

    for (uint32_t i = 0; i < renderer->vk.swapchain_image_all_count; i++)
    {
        VkResult result = vkBeginCommandBuffer(renderer->vk.commandbuffer_all[i], &command_buffer_begin_info);
        CNVX_VULKAN_ASSERTF(renderer, result, "vkBeginCommandBuffer (%u/%u)", i + 1, renderer->vk.swapchain_image_all_count);

        VkClearValue clear_value = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderPassBeginInfo render_pass_begin_info;
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = NULL;
        render_pass_begin_info.renderPass = renderer->vk.renderer_pass;
        render_pass_begin_info.framebuffer = renderer->vk.framebuffer_all[i];
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.renderArea.extent.width = renderer->width;
        render_pass_begin_info.renderArea.extent.height = renderer->height;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(renderer->vk.commandbuffer_all[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(renderer->vk.commandbuffer_all[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->vk.pipeline);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = renderer->width;
        viewport.height = renderer->height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(renderer->vk.commandbuffer_all[i], 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = renderer->width;
        scissor.extent.height = renderer->height;

        vkCmdSetScissor(renderer->vk.commandbuffer_all[i], 0, 1, &scissor);

        vkCmdDraw(renderer->vk.commandbuffer_all[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(renderer->vk.commandbuffer_all[i]);

        result = vkEndCommandBuffer(renderer->vk.commandbuffer_all[i]);
        CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vkEndCommandBuffer (%u/%u)", i + 1, renderer->vk.swapchain_image_all_count);
    }
}

void canvas_vulkan_semaphore_create(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: semaphore creation");

    VkSemaphoreCreateInfo semaphore_create_info;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = NULL;
    semaphore_create_info.flags = 0;

    VkResult result = vkCreateSemaphore(renderer->vk.device, &semaphore_create_info, NULL, &renderer->vk.semaphore_image_available);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateSemaphore (1/2)");

    result = vkCreateSemaphore(renderer->vk.device, &semaphore_create_info, NULL, &renderer->vk.semaphore_rendering_done);
    CNVX_VULKAN_ASSERT(renderer, result, "vkCreateSemaphore (2/2)");
}

void canvas_vulkan_semaphore_destroy(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    vkDestroySemaphore(renderer->vk.device, renderer->vk.semaphore_rendering_done, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroySemaphore(1/2)");

    vkDestroySemaphore(renderer->vk.device, renderer->vk.semaphore_image_available, NULL);
    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "vulkan: vkDestroySemaphore(2/2)");

    CNVX_NLOG(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "vulkan: semaphore destruction");
}

void canvas_vulkan_frame_draw(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_VULKAN_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    uint32_t image_index = 0;

    VkResult result = vkAcquireNextImageKHR(renderer->vk.device, renderer->vk.swapchain, INT64_MAX, renderer->vk.semaphore_image_available, VK_NULL_HANDLE, &image_index);
    CNVX_VULKAN_QASSERT(renderer, result, "vkAcquireNextImageKHR");

    VkPipelineStageFlags wait_stage_mask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = NULL;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &renderer->vk.semaphore_image_available;
    submit_info.pWaitDstStageMask = wait_stage_mask;
    submit_info.commandBufferCount = renderer->vk.swapchain_image_all_count;
    submit_info.pCommandBuffers = &renderer->vk.commandbuffer_all[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &renderer->vk.semaphore_rendering_done;
    
    result = vkQueueSubmit(renderer->vk.queue, 1, &submit_info, VK_NULL_HANDLE);
    CNVX_VULKAN_QASSERT(renderer, result, "vkQueueSubmit");

    VkPresentInfoKHR present_info;
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = NULL;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &renderer->vk.semaphore_rendering_done;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &renderer->vk.swapchain;
    present_info.pImageIndices = &image_index;
    present_info.pResults = NULL;

    result = vkQueuePresentKHR(renderer->vk.queue, &present_info);
    CNVX_VULKAN_QASSERT(renderer, result, "vkQueuePresentKHR");
}
