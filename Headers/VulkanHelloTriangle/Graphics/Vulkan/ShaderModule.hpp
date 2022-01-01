/*
 * Shader modules in Vulkan are literally just shaders.
 */

#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/ShaderType.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#ifdef SHADER_COMPILATION
#include <glslang/SPIRV/GlslangToSpv.h>
#endif

namespace Vulkan
{
    class ShaderModule : public IDisposable
    {
    public:
        bool Init(Device* pDevice);
    
    #ifdef SHADER_COMPILATION
        /**
         * @brief Compiles a shader from GLSL and creates the shader.
         * 
         * @param glsl The GLSL shader code.
         * @param type The type of shader.
         * @param infoLog A pointer to a std::string to append info about the
         *  shader compilation
         * @param debugLog Same as info log but for debug messages.
         * 
         * @returns True if the shader compiled successfully. Otherwise, false.
         */
        bool FromGLSL(const char* glsl, ShaderType type,
            std::string* infoLog = nullptr, std::string* debugLog = nullptr);
    #endif
        /**
         * @brief Creates the shader module from SPIR-V data.
         * 
         * @param spv The SPIR-V bytecode.
         * @param spvSize The size of the SPIR-V data.
         * 
         * @returns True if creation was successful. Otherwise, false.
         */
        bool FromSpirV(uint32_t* spv, uint64_t spvSize); 

    #ifdef SHADER_COMPILATION
        /**
         * @brief Compiles a shader from GLSL. Requires GLSLANG to be initialized.
         * 
         * @param glsl The GLSL shader code.
         * @param type The type of shader.
         * 
         * @returns An std::vector of SPIR-V shader data.
         */
        bool CompileGLSL(
            const char* glsl, 
            ShaderType type,
            std::vector<uint32_t>& spv, 
            std::string& infoLog,
            std::string& debugLog
        );
    #endif
    protected:
        void OnDispose();
    private:
        void InitResources(TBuiltInResource& resources);

        Device* pDevice = nullptr;

        VkShaderModule shaderModule;
        bool shaderCreated = false;
    };
}