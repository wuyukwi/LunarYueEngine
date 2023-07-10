#include "runtime/function/render/renderer_callback.h"

namespace LunarYue
{

    void RendererCallback::fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) {}

    void RendererCallback::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) {}

    void RendererCallback::profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) {}

    void RendererCallback::profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) {}

    void RendererCallback::profilerEnd() {}

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

    void RendererCallback::captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) {}

    void RendererCallback::captureEnd() {}

    void RendererCallback::captureFrame(const void* _data, uint32_t _size) {}

} // namespace LunarYue
