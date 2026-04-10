#include <spdlog/spdlog.h>

#include <Crioulo/global_context_manager.h>

using namespace Crioulo;

GlobalContextManager& GlobalContextManager::getInstance() {
	static GlobalContextManager instance{};

	return instance;
}

GlobalContextManager::GlobalContextManager() :
	current(nullptr){

}

void GlobalContextManager::makeCurrent(IContext& context) {
	
	if (&context == current) {
		SPDLOG_TRACE("Current OpenGL Context was kept.");
		return;
	}

	SPDLOG_DEBUG("Current OpenGL Context was updated.");
	if (current) {
		current->doneCurrent();
	}
	current = &context;
	current->makeCurrent();
}

void GlobalContextManager::doneCurrent() {
	current->doneCurrent();
}