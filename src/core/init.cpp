#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Crioulo/init.h"


void Crioulo::initialize(GetProcAdress getProcAddress, IContext& context) {

    if (isInitialized) return;

    GlobalContextManager::getInstance().makeCurrent(context);

    if (!gladLoadGLLoader((GLADloadproc)getProcAddress))
    {
        throw std::runtime_error("Failed to initialize OpenGL functions");
    }
    isInitialized = true;

    // Initalize logging
    spdlog::init_thread_pool(8192, 1);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto logger = std::make_shared<spdlog::async_logger>(
        "console",
        console_sink,
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::overrun_oldest
    );

    spdlog::set_default_logger(logger);

    logger->set_level(spdlog::level::trace);
}