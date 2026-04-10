#ifndef CRIOULO_INIT_H
#define CRIOULO_INIT_H

#include "Crioulo/global_context_manager.h"

namespace Crioulo {

    inline bool isInitialized = false;

	using GetProcAdress = void* (*)(const char*);

	void initialize(GetProcAdress getProcAddress, IContext& context);
}

#endif