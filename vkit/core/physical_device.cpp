/* Copyright (c) 2020-2024, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "physical_device.h"
#include <utils/log.h>
#include <utils/error.h>
#include "instance.h"

namespace vkit
{
	PhysicalDevice::PhysicalDevice(Instance& instance, VkPhysicalDevice physical_device):
		instance{ instance },
		handle{ physical_device }
	{
		vkGetPhysicalDeviceFeatures(physical_device, &features);
		vkGetPhysicalDeviceProperties(physical_device, &properties);
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

		LOGI("Found GPU: %s", properties.deviceName);

		uint32_t queue_family_properties_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, nullptr);
		queue_family_properties = std::vector<VkQueueFamilyProperties>(queue_family_properties_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, queue_family_properties.data());
	}

	PhysicalDevice::~PhysicalDevice()
	{

	}

	Instance& PhysicalDevice::get_instance() const
	{
		return instance;
	}

	VkPhysicalDevice PhysicalDevice::get_handle() const
	{
		return handle;
	}

	const VkPhysicalDeviceFeatures& PhysicalDevice::get_features() const
	{
		return features;
	}

	const VkPhysicalDeviceProperties& PhysicalDevice::get_properties() const
	{
		return properties;
	}

	const VkPhysicalDeviceMemoryProperties& PhysicalDevice::get_memory_properties() const
	{
		return memory_properties;
	}

	const std::vector<VkQueueFamilyProperties>& PhysicalDevice::get_queue_family_properties() const
	{
		return queue_family_properties;
	}
}        // namespace vkit
