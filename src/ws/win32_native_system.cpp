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

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "win32_native_system.h"
#include "vulkan_state.h"
#include <stdexcept>
#include <string>

namespace
{
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}
}

Win32NativeSystem::Win32NativeSystem(vk::Extent2D vk_extent)
    : vk_extent{vk_extent}, hwnd{nullptr}, hinstance{GetModuleHandle(nullptr)}
{
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = hinstance;
    wc.lpszClassName = L"VkMarkWin32";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc))
        throw std::runtime_error("Failed to register Win32 window class");

    RECT rect = {0, 0, static_cast<LONG>(vk_extent.width), static_cast<LONG>(vk_extent.height)};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    std::string title = "vkmark " VKMARK_VERSION_STR;
    std::wstring wtitle(title.begin(), title.end());

    hwnd = CreateWindowExW(0, L"VkMarkWin32", wtitle.c_str(),
                           WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           rect.right - rect.left, rect.bottom - rect.top,
                           nullptr, nullptr, hinstance, nullptr);

    if (!hwnd)
        throw std::runtime_error("Failed to create Win32 window");
}

Win32NativeSystem::~Win32NativeSystem()
{
    if (hwnd)
        DestroyWindow(hwnd);
    UnregisterClassW(L"VkMarkWin32", hinstance);
}

VulkanWSI::Extensions Win32NativeSystem::required_extensions()
{
    return {{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME}, {}};
}

uint32_t Win32NativeSystem::get_presentation_queue_family_index(vk::PhysicalDevice const& pd)
{
    auto const queue_families = pd.getQueueFamilyProperties();

    for (auto i = 0u; i < queue_families.size(); ++i)
    {
        if (pd.getWin32PresentationSupportKHR(i))
            return i;
    }

    return invalid_queue_family_index;
}

bool Win32NativeSystem::should_quit()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return true;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return false;
}

vk::Extent2D Win32NativeSystem::get_vk_extent()
{
    return vk_extent;
}

ManagedResource<vk::SurfaceKHR> Win32NativeSystem::create_vk_surface(VulkanState& vulkan)
{
    auto const create_info = vk::Win32SurfaceCreateInfoKHR{}
        .setHinstance(hinstance)
        .setHwnd(hwnd);

    return ManagedResource<vk::SurfaceKHR>{
        vulkan.instance().createWin32SurfaceKHR(create_info),
        [vptr=&vulkan] (vk::SurfaceKHR& s) { vptr->instance().destroySurfaceKHR(s); }};
}
