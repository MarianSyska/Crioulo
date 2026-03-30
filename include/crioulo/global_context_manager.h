#ifndef CRIOULO_GLOBAL_CONTEXT_MANAGER
#define CRIOULO_GLOBAL_CONTEXT_MANAGER

#include <unordered_set>

namespace Crioulo {
	class IContext {
	public:
		virtual void makeCurrent() = 0;
		virtual void doneCurrent() = 0;
	};

	class GlobalContextManager {

	public:
		static GlobalContextManager& getInstance();

		GlobalContextManager(const GlobalContextManager& other) = delete;
		GlobalContextManager(GlobalContextManager&& other) = delete;

		GlobalContextManager& operator=(const GlobalContextManager& other) = delete;
		GlobalContextManager& operator=(GlobalContextManager&& other) = delete;

		void makeCurrent(IContext& context);
		void doneCurrent();

	private:

		GlobalContextManager();

		IContext* current;
	};
}
#endif
