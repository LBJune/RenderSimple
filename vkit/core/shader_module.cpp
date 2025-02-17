

#include "shader_module.h"
#include "device.h"
#include <filesystem>
#include <fstream>

#include <utils/log.h>
#include <utils//strings.h>
#include <utils//glsl_compiler.h>
#include <utils/spirv_reflection.h>

namespace vkit
{
	/**
	 * @brief Pre-compiles project shader files to include header code
	 * @param source The shader file
	 * @returns A byte array of the final shader
	 */
	inline std::vector<std::string> precompile_shader(const std::string& source)
	{
		std::vector<std::string> final_file;

		auto lines = split(source, '\n');

		for (auto& line : lines)
		{
			if (line.find("#include \"") == 0)
			{
				// Include paths are relative to the base shader directory
				std::string include_path = line.substr(10);
				size_t      last_quote = include_path.find("\"");
				if (!include_path.empty() && last_quote != std::string::npos)
				{
					include_path = include_path.substr(0, last_quote);
				}
				if (std::filesystem::exists(include_path)) {
					std::ifstream file(include_path);
					std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

					auto include_file = precompile_shader(content);
					for (auto& include_file_line : include_file)
					{
						final_file.push_back(include_file_line);
					}
				}
			}
			else
			{
				final_file.push_back(line);
			}
		}

		return final_file;
	}

	inline std::vector<uint8_t> convert_to_bytes(std::vector<std::string>& lines)
	{
		std::vector<uint8_t> bytes;

		for (auto& line : lines)
		{
			line += "\n";
			std::vector<uint8_t> line_bytes(line.begin(), line.end());
			bytes.insert(bytes.end(), line_bytes.begin(), line_bytes.end());
		}

		return bytes;
	}

	ShaderModule::ShaderModule(Device& device, VkShaderStageFlagBits stage, const ShaderSource& glsl_source, const std::string& entry_point, const ShaderVariant& shader_variant) :
		device{ device },
		stage{ stage },
		entry_point{ entry_point }
	{
		// Compiling from GLSL source requires the entry point
		if (entry_point.empty())
		{
			throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
		}

		auto& source = glsl_source.get_source();

		// Check if application is passing in GLSL source code to compile to SPIR-V
		if (source.empty())
		{
			throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
		}

		// Precompile source into the final spirv bytecode
		auto glsl_final_source = precompile_shader(source);

		// Compile the GLSL source
		GLSLCompiler glsl_compiler;

		if (!glsl_compiler.compile_to_spirv(stage, convert_to_bytes(glsl_final_source), entry_point, shader_variant, spirv, info_log))
		{
			LOGE("Shader compilation failed for shader \"{}\"", glsl_source.get_filename());
			LOGE("{}", info_log);
			throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
		}

		SPIRVReflection spirv_reflection;

		// Reflect all shader resources
		if (!spirv_reflection.reflect_shader_resources(stage, spirv, resources, shader_variant))
		{
			throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
		}

		// Generate a unique id, determined by source and variant
		std::hash<std::string> hasher{};
		id = hasher(std::string{ reinterpret_cast<const char*>(spirv.data()),
								reinterpret_cast<const char*>(spirv.data() + spirv.size()) });

		create_info.codeSize = glsl_source.get_source().size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(glsl_source.get_source().data());

		if (vkCreateShaderModule(device.get_handle(), &create_info, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	}

	ShaderModule::ShaderModule(ShaderModule&& other) :
		device{ other.device },
		handle{ other.get_handle()},
		id{ other.id },
		stage{ other.stage },
		entry_point{ other.entry_point },
		spirv{ other.spirv },
		resources{ other.resources },
		info_log{ other.info_log }
	{
		other.stage = {};
	}


	ShaderModule::~ShaderModule()
	{
		if (handle != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device.get_handle(), handle, nullptr);
		}
	}


	VkShaderModule ShaderModule::get_handle() const
	{
		return handle;
	}


	ShaderSource::ShaderSource(const std::string& filename) :
		filename{ filename }
	{
		std::filesystem::path file_path = filename;

		if (std::filesystem::exists(file_path)) {
			std::ifstream file(file_path);
			source.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		}
		else {
			LOGE("File %s, does not exist", filename.c_str());
		}

		std::hash<std::string> hasher{};
		id = hasher(std::string{ this->source.cbegin(), this->source.cend() });
	}

	size_t ShaderSource::get_id() const
	{
		return id;
	}

	const std::string& ShaderSource::get_filename() const
	{
		return filename;
	}

	void ShaderSource::set_source(const std::string& source_)
	{
		source = source_;
		std::hash<std::string> hasher{};
		id = hasher(std::string{ this->source.cbegin(), this->source.cend() });
	}

	const std::string& ShaderSource::get_source() const
	{
		return source;
	}
}        // namespace vkit
