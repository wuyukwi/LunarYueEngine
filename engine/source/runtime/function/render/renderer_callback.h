#pragma once

#include <bgfx/bgfx.h>

namespace LunarYue
{
    class RendererCallback : public bgfx::CallbackI
    {
    public:
        void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override;

        void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override;

        void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override;

        void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) override;

        void profilerEnd() override;

        uint32_t cacheReadSize(uint64_t _id) override;

        bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override;

        void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override;

        void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip)
            override;

        void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) override;

        void captureEnd() override;

        void captureFrame(const void* _data, uint32_t _size) override;
    };
} // namespace LunarYue
