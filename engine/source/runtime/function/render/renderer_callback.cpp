#include "runtime/function/render/renderer_callback.h"
#include "runtime/core/base/macro.h"

namespace LunarYue
{

    void RendererCallback::fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str)
    {
        LOG_FATAL("BGFX Fatal Error at {}:{}", _filePath, _line);
        LOG_FATAL("Error code: {}", _code);
        LOG_FATAL("Error message: {}", _str);
    }

    void RendererCallback::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList)
    {
        char buffer[512];
        vsprintf(buffer, _format, _argList);

        LOG_INFO("{}:{} \nFormat: {}", _filePath, _line, buffer);
    }

    void RendererCallback::profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line)
    {
        LOG_INFO("\n _name: {} _abgr:{} _filePath: {} _line :{}", _name, _abgr, _filePath, _line);
    }

    void RendererCallback::profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line)
    {
        LOG_INFO("\n _name: {} _abgr:{} _filePath: {} _line :{}", _name, _abgr, _filePath, _line);
    }

    void RendererCallback::profilerEnd() { LOG_INFO("profilerEnd"); }

    uint32_t RendererCallback::cacheReadSize(uint64_t _id) { return _id; }

    bool RendererCallback::cacheRead(uint64_t _id, void* _data, uint32_t _size) { return true; }

    void RendererCallback::cacheWrite(uint64_t _id, const void* _data, uint32_t _size) {}

    void RendererCallback::screenShot(const char* _filePath,
                                      uint32_t    _width,
                                      uint32_t    _height,
                                      uint32_t    _pitch,
                                      const void* _data,
                                      uint32_t    _size,
                                      bool        _yflip)
    {}

    void RendererCallback::captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip)
    {
        LOG_INFO("\n _width: {} _height:{} _pitch: {} _format :{} _yflip :{}", _width, _height, _pitch, _format, _yflip);
    }

    void RendererCallback::captureEnd() { LOG_INFO("captureEnd"); }

    void RendererCallback::captureFrame(const void* _data, uint32_t _size) { LOG_INFO("data size : {}", _size); }

} // namespace LunarYue
