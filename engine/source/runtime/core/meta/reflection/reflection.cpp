#include "reflection.h"
#include <cstring>
#include <map>

namespace LunarYue
{
    namespace Reflection
    {
        const char* k_unknown_type = "UnknownType";
        const char* k_unknown      = "Unknown";

        static std::map<std::string, ClassFunctionStruct*>       m_class_map;
        static std::multimap<std::string, FieldFunctionStruct*>  m_field_map;
        static std::multimap<std::string, MethodFunctionStruct*> m_method_map;
        static std::map<std::string, ArrayFunctionStruct*>       m_array_map;

        void TypeMetaRegisterInterface::registerToFieldMap(const char* name, FieldFunctionStruct* value)
        {
            m_field_map.insert(std::make_pair(name, value));
        }
        void TypeMetaRegisterInterface::registerToMethodMap(const char* name, MethodFunctionStruct* value)
        {
            m_method_map.insert(std::make_pair(name, value));
        }
        void TypeMetaRegisterInterface::registerToArrayMap(const char* name, ArrayFunctionStruct* value)
        {
            if (m_array_map.find(name) == m_array_map.end())
            {
                m_array_map.insert(std::make_pair(name, value));
            }
            else
            {
                delete value;
            }
        }

        void TypeMetaRegisterInterface::registerToClassMap(const char* name, ClassFunctionStruct* value)
        {
            if (m_class_map.find(name) == m_class_map.end())
            {
                m_class_map.insert(std::make_pair(name, value));
            }
            else
            {
                delete value;
            }
        }

        void TypeMetaRegisterInterface::unRegisterAll()
        {
            for (const auto& itr : m_field_map)
            {
                delete itr.second;
            }
            m_field_map.clear();
            for (const auto& itr : m_class_map)
            {
                delete itr.second;
            }
            m_class_map.clear();
            for (const auto& itr : m_array_map)
            {
                delete itr.second;
            }
            m_array_map.clear();
        }

        TypeMeta::TypeMeta(const std::string& type_name) : m_type_name(type_name)
        {
            m_is_valid = false;
            m_fields.clear();
            m_methods.clear();

            auto [field_fst, field_snd] = m_field_map.equal_range(type_name);
            while (field_fst != field_snd)
            {
                FieldAccessor f_field(field_fst->second);
                m_fields.emplace_back(f_field);
                m_is_valid = true;

                ++field_fst;
            }

            auto [method_fst, method_snd] = m_method_map.equal_range(type_name);
            while (method_fst != method_snd)
            {
                MethodAccessor f_method(method_fst->second);
                m_methods.emplace_back(f_method);
                m_is_valid = true;

                ++method_fst;
            }
        }

        TypeMeta::TypeMeta() : m_type_name(k_unknown_type), m_is_valid(false)
        {
            m_fields.clear();
            m_methods.clear();
        }

        TypeMeta TypeMeta::newMetaFromName(const std::string& type_name)
        {
            TypeMeta f_type(type_name);
            return f_type;
        }

        bool TypeMeta::newArrayAccessorFromName(const std::string& array_type_name, ArrayAccessor& accessor)
        {
            auto iter = m_array_map.find(array_type_name);

            if (iter != m_array_map.end())
            {
                const ArrayAccessor new_accessor(iter->second);
                accessor = new_accessor;
                return true;
            }

            return false;
        }

        ReflectionInstance TypeMeta::newFromNameAndJson(const std::string& type_name, const Json& json_context)
        {
            const auto iter = m_class_map.find(type_name);

            if (iter != m_class_map.end())
            {
                return {TypeMeta(type_name), (*iter->second).constructorWithJson(json_context)};
            }
            return {};
        }

        ReflectionInstance TypeMeta::newFromName(const std::string& type_name)
        {
            const auto iter = m_class_map.find(type_name);

            if (iter != m_class_map.end())
            {
                return {TypeMeta(type_name), (*iter->second).constructorEmpty()};
            }
            return {};
        }

        Json TypeMeta::writeByName(const std::string& type_name, void* instance)
        {
            const auto iter = m_class_map.find(type_name);

            if (iter != m_class_map.end())
            {
                return (*iter->second).writeJsonByName(instance);
            }
            return {};
        }

        std::string TypeMeta::getTypeName() { return m_type_name; }

        int TypeMeta::getFieldsList(FieldAccessor*& out_list) const
        {
            const int count = m_fields.size();
            out_list        = new FieldAccessor[count];
            for (int i = 0; i < count; ++i)
            {
                out_list[i] = m_fields[i];
            }
            return count;
        }

        int TypeMeta::getMethodsList(MethodAccessor*& out_list) const
        {
            const int count = m_methods.size();
            out_list        = new MethodAccessor[count];
            for (int i = 0; i < count; ++i)
            {
                out_list[i] = m_methods[i];
            }
            return count;
        }

        int TypeMeta::getBaseClassReflectionInstanceList(ReflectionInstance*& out_list, void* instance) const
        {
            if (const auto iterate = m_class_map.find(m_type_name); iterate != m_class_map.end())
            {
                return (*iterate->second).getBaseClassReflectionInstanceList(out_list, instance);
            }

            return 0;
        }

        FieldAccessor TypeMeta::getFieldByName(const char* name)
        {
            const auto it = std::find_if(m_fields.begin(), m_fields.end(), [&](const auto& i) { return std::strcmp(i.getFieldName(), name) == 0; });
            if (it != m_fields.end())
                return *it;
            return {nullptr};
        }

        MethodAccessor TypeMeta::getMethodByName(const char* name)
        {
            const auto it =
                std::find_if(m_methods.begin(), m_methods.end(), [&](const auto& i) { return std::strcmp(i.getMethodName(), name) == 0; });
            if (it != m_methods.end())
                return *it;
            return {nullptr};
        }

        TypeMeta& TypeMeta::operator=(const TypeMeta& dest)
        {
            if (this == &dest)
            {
                return *this;
            }
            m_fields.clear();
            m_fields = dest.m_fields;

            m_methods.clear();
            m_methods = dest.m_methods;

            m_type_name = dest.m_type_name;
            m_is_valid  = dest.m_is_valid;

            return *this;
        }
        FieldAccessor::FieldAccessor()
        {
            m_field_type_name = k_unknown_type;
            m_field_name      = k_unknown;
            m_functions       = nullptr;
        }

        FieldAccessor::FieldAccessor(FieldFunctionStruct* functions) : m_functions(functions)
        {
            m_field_type_name = k_unknown_type;
            m_field_name      = k_unknown;
            if (m_functions == nullptr)
            {
                return;
            }

            m_field_type_name = (*m_functions).getClassName();
            m_field_name      = (*m_functions).getFieldName();
        }

        void* FieldAccessor::get(void* instance) const
        {
            // todo: should check validation
            return (*m_functions).get(instance);
        }

        void FieldAccessor::set(void* instance, void* value) const
        {
            // todo: should check validation
            (*m_functions).set(instance, value);
        }

        TypeMeta FieldAccessor::getOwnerTypeMeta() const
        {
            // todo: should check validation
            TypeMeta f_type((*m_functions).getClassName());
            return f_type;
        }

        bool FieldAccessor::getTypeMeta(TypeMeta& field_type) const
        {
            const TypeMeta f_type(m_field_type_name);
            field_type = f_type;
            return f_type.m_is_valid;
        }

        const char* FieldAccessor::getFieldName() const { return m_field_name; }
        const char* FieldAccessor::getFieldTypeName() const { return m_field_type_name; }

        bool FieldAccessor::isArrayType() const
        {
            // todo: should check validation
            return (*m_functions).isArray();
        }

        FieldAccessor& FieldAccessor::operator=(const FieldAccessor& dest)
        {
            if (this == &dest)
            {
                return *this;
            }
            m_functions       = dest.m_functions;
            m_field_name      = dest.m_field_name;
            m_field_type_name = dest.m_field_type_name;
            return *this;
        }

        MethodAccessor::MethodAccessor()
        {
            m_method_name = k_unknown;
            m_functions   = nullptr;
        }

        MethodAccessor::MethodAccessor(MethodFunctionStruct* functions) : m_functions(functions)
        {
            m_method_name = k_unknown;
            if (m_functions == nullptr)
            {
                return;
            }

            m_method_name = (*m_functions).getMethodName();
        }
        const char*     MethodAccessor::getMethodName() const { return (*m_functions).getMethodName(); }
        MethodAccessor& MethodAccessor::operator=(const MethodAccessor& dest)
        {
            if (this == &dest)
            {
                return *this;
            }
            m_functions   = dest.m_functions;
            m_method_name = dest.m_method_name;
            return *this;
        }
        void MethodAccessor::invoke(void* instance) const { (*m_functions).invoke(instance); }
        ArrayAccessor::ArrayAccessor() : m_func(nullptr), m_array_type_name("UnKnownType"), m_element_type_name("UnKnownType") {}

        ArrayAccessor::ArrayAccessor(ArrayFunctionStruct* array_func) : m_func(array_func)
        {
            m_array_type_name   = k_unknown_type;
            m_element_type_name = k_unknown_type;
            if (m_func == nullptr)
            {
                return;
            }

            m_array_type_name   = (*m_func).getArrayTypeName();
            m_element_type_name = (*m_func).getElementTypeName();
        }
        const char* ArrayAccessor::getArrayTypeName() const { return m_array_type_name; }
        const char* ArrayAccessor::getElementTypeName() const { return m_element_type_name; }
        void        ArrayAccessor::set(int index, void* instance, void* element_value) const
        {
            const int count = getSize(instance);
            if (index < 0 || index >= count)
            {
                return;
            }

            (*m_func).set(index, instance, element_value);
        }

        void* ArrayAccessor::get(int index, void* instance) const
        {
            const int count = getSize(instance);
            if (index < 0 || index >= count)
            {
                return nullptr;
            }

            return (*m_func).get(index, instance);
        }

        int ArrayAccessor::getSize(void* instance) const
        {
            if (instance == nullptr)
            {
                return -1;
            }

            return (*m_func).getSize(instance);
        }

        ArrayAccessor& ArrayAccessor::operator=(const ArrayAccessor& dest)
        {
            if (this == &dest)
            {
                return *this;
            }
            m_func              = dest.m_func;
            m_array_type_name   = dest.m_array_type_name;
            m_element_type_name = dest.m_element_type_name;
            return *this;
        }

        ReflectionInstance& ReflectionInstance::operator=(const ReflectionInstance& dest)
        {
            if (this == &dest)
            {
                return *this;
            }
            m_instance = dest.m_instance;
            m_meta     = dest.m_meta;

            return *this;
        }

        ReflectionInstance& ReflectionInstance::operator=(ReflectionInstance&& dest) noexcept
        {
            if (this == &dest)
            {
                return *this;
            }
            m_instance = dest.m_instance;
            m_meta     = dest.m_meta;

            return *this;
        }
    } // namespace Reflection
} // namespace LunarYue