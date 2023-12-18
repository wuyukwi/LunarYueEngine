#include "inspector_dock.h"
#include "../../editing/editing_system.h"
#include "../inspectors/inspectors.h"

#include <core/system/subsystem.h>

void inspector_dock::render()
{
    auto& es = core::get_subsystem<editor::editing_system>();

    auto& selected = es.selection_data.object;
    if (selected)
    {
        inspect_var(selected);
    }
}

inspector_dock::inspector_dock(const std::string& dtitle) {}
