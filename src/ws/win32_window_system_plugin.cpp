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

#include "win32_native_system.h"
#include "window_system_plugin.h"
#include "swapchain_window_system.h"
#include "window_system_priority.h"
#include "options.h"

extern "C"
{

VKMARK_PLUGIN_API void vkmark_window_system_load_options(Options&)
{
}

VKMARK_PLUGIN_API int vkmark_window_system_probe(Options const&)
{
    return VKMARK_WIN32_WINDOW_SYSTEM_PRIORITY;
}

VKMARK_PLUGIN_API std::unique_ptr<WindowSystem> vkmark_window_system_create(Options const& options)
{
    auto native_system = std::make_unique<Win32NativeSystem>(
        vk::Extent2D{static_cast<uint32_t>(options.size.first),
                     static_cast<uint32_t>(options.size.second)});
    return std::make_unique<SwapchainWindowSystem>(
        std::move(native_system),
        options.present_mode,
        options.pixel_format);
}

}
