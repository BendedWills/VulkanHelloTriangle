/*
 * Shader modules in Vulkan are literally just shaders.
 */

#pragma once

#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/ShaderType.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace Vulkan
{
    class ShaderModule : public IDisposable
    {
    public:
        // Init actually doesn't do anything for the ShaderModule
        // instead, initialization functionality is in the Load and Compile
        // functions. I have just defined it here for simplicity and to
        // maintain a code style as the other classes have
        // init functions.
        void Init() {}
        
        /**
         * @brief Compiles a shader from GLSL and creates the shader.
         * 
         * @param glsl The GLSL shader code.
         * @param type The type of shader.
         */
        bool FromGLSL(const char* glsl, ShaderType type);
        bool FromSpirV(const char* spv, ShaderType type); 

        /**
         * @brief Compiles a shader from GLSL.
         * 
         * @param glsl The GLSL shader code.
         * @param type The type of shader.
         * 
         * @returns An std::vector of SPIR-V shader data.
         */
        std::vector<uint32_t> CompileGLSL(const char* glsl, ShaderType type);
    protected:
        void OnDispose();
    private:
        VkShaderModule shaderModule;
    };
}