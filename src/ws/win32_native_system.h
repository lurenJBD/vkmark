/*
 * Copyright © 2025 Collabora Ltd.
 *
 * This file is part of vkmark.
 *
 * vkmark is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * vkmark is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with vkmark. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VKMARK_WIN32_NATIVE_SYSTEM_H_
#define VKMARK_WIN32_NATIVE_SYSTEM_H_

#include "ws/native_system.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Win32NativeSystem : public NativeSystem
{
public:
    Win32NativeSystem(vk::Extent2D vk_extent);
    ~Win32NativeSystem();

    VulkanWSI::Extensions required_extensions() override;
    uint32_t get_presentation_queue_family_index(vk::PhysicalDevice const& pd) override;
    bool should_quit() override;
    vk::Extent2D get_vk_extent() override;
    ManagedResource<vk::SurfaceKHR> create_vk_surface(VulkanState& vulkan) override;

private:
    vk::Extent2D vk_extent;
    HWND hwnd;
    HINSTANCE hinstance;
};

#endif
