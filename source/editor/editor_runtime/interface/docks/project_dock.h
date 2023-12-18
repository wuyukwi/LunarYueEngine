#pragma once

#include <core/filesystem/filesystem_cache.hpp>
#include <string>

class project_dock
{
public:
    project_dock(const std::string& dtitle);
    void render();

private:
    void context_menu();
    void context_create_menu();
    void set_cache_path(const fs::path& path);
    void import();

    fs::directory_cache cache_;
    fs::path            cache_path_with_protocol_;
    fs::path            root_;
    float               scale_ = 0.75f;
};
