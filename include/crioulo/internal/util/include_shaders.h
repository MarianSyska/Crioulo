#ifndef CRIOULO_INCLUDER_H
#define CRIOULO_INCLUDER_H

#include <span>
#include <memory>

namespace Crioulo::Util {

	std::unique_ptr<char, void(*)(void*)> includeShaders(const char* sourceCode, std::span<const std::array<const char*, 2>> includeDict);
}

#endif CRIOULO_INCLUDER_H