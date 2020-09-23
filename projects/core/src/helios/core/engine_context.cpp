#include <helios/core/engine_context.hpp>

#include <helios/io/file.hpp>

#include <nlohmann/json.hpp>

namespace helios
{
    EngineContext* EngineContext::_ctx = nullptr;

    EngineContext& EngineContext::instance()
    {
        return *_ctx;
    }

    IWindow& EngineContext::window()
    {
        return *_win;
    }

    void EngineContext::_initialize()
    {
        using nlohmann::json;

        json configuration = json::parse(File::read_text("assets/config.json"));
        auto& appConfiguration = configuration["application"];
        auto& windowConfiguration = appConfiguration["window"];

        _ctx = new EngineContext;
        _ctx->_win = WindowBuilder().title(windowConfiguration["title"])
            .width(windowConfiguration["width"])
            .height(windowConfiguration["height"])
            .resizable(windowConfiguration["resize"])
            .build();
    }

    void EngineContext::_close()
    {
        delete _ctx->_win;
        delete _ctx;
    }
} // namespace helios