
#include "stb_include/stb_include.h"
#include "Crioulo/internal/util/include_shaders.h"

std::unique_ptr<char, void(*)(void*)> Crioulo::Util::includeShaders(const char* sourceCode, std::span<const std::array<const char*, 2>> includeDict) {
	std::unique_ptr<char, void(*)(void*)> sourceCodeCopy(strdup(sourceCode), free);
	char error[256];
	char* preprocessedSourceCode = stb_include_string_preloaded(sourceCodeCopy.get(), 0, includeDict, "", error);
	return std::unique_ptr<char, void(*)(void*)>(preprocessedSourceCode, free);
}
