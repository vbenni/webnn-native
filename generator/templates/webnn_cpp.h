//* Copyright 2017 The Dawn Authors
//* Copyright 2021 The WebNN-native Authors
//*
//* Licensed under the Apache License, Version 2.0 (the "License");
//* you may not use this file except in compliance with the License.
//* You may obtain a copy of the License at
//*
//*     http://www.apache.org/licenses/LICENSE-2.0
//*
//* Unless required by applicable law or agreed to in writing, software
//* distributed under the License is distributed on an "AS IS" BASIS,
//* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//* See the License for the specific language governing permissions and
//* limitations under the License.
#ifndef WEBNN_CPP_H_
#define WEBNN_CPP_H_

#include "webnn/webnn.h"
#include "webnn/EnumClassBitmasks.h"

#include <limits>

namespace ml {

    {% for type in by_category["enum"] %}
        enum class {{as_cppType(type.name)}} : uint32_t {
            {% for value in type.values %}
                {{as_cppEnum(value.name)}} = 0x{{format(value.value, "08X")}},
            {% endfor %}
            //* TODO(dawn:22) remove this once the PSA is sent and the deadline passed.
            {% if type.name.canonical_case() == "texture format" %}
                RG11B10Float = RG11B10Ufloat,
                BC6HRGBSfloat = BC6HRGBFloat,
            {% endif %}
        };

    {% endfor %}

    {% for type in by_category["bitmask"] %}
        enum class {{as_cppType(type.name)}} : uint32_t {
            {% for value in type.values %}
                {{as_cppEnum(value.name)}} = 0x{{format(value.value, "08X")}},
            {% endfor %}
        };

    {% endfor %}

    {% for type in by_category["bitmask"] %}
        template<>
        struct IsDawnBitmask<{{as_cppType(type.name)}}> {
            static constexpr bool enable = true;
        };

    {% endfor %}

    using Proc = WebnnProc;
    {% for type in by_category["callback"] %}
        using {{as_cppType(type.name)}} = {{as_cType(type.name)}};
    {% endfor %}

    {% for type in by_category["object"] %}
        class {{as_cppType(type.name)}};
    {% endfor %}

    {% for type in by_category["structure"] %}
        struct {{as_cppType(type.name)}};
    {% endfor %}

    template<typename Derived, typename CType>
    class ObjectBase {
      public:
        ObjectBase() = default;
        ObjectBase(CType handle): mHandle(handle) {
            if (mHandle) Derived::WebnnReference(mHandle);
        }
        ~ObjectBase() {
            if (mHandle) Derived::WebnnRelease(mHandle);
        }

        ObjectBase(ObjectBase const& other)
            : ObjectBase(other.GetHandle()) {
        }
        Derived& operator=(ObjectBase const& other) {
            if (&other != this) {
                if (mHandle) Derived::WebnnRelease(mHandle);
                mHandle = other.mHandle;
                if (mHandle) Derived::WebnnReference(mHandle);
            }

            return static_cast<Derived&>(*this);
        }

        ObjectBase(ObjectBase&& other) {
            mHandle = other.mHandle;
            other.mHandle = 0;
        }
        Derived& operator=(ObjectBase&& other) {
            if (&other != this) {
                if (mHandle) Derived::WebnnRelease(mHandle);
                mHandle = other.mHandle;
                other.mHandle = 0;
            }

            return static_cast<Derived&>(*this);
        }

        ObjectBase(std::nullptr_t) {}
        Derived& operator=(std::nullptr_t) {
            if (mHandle != nullptr) {
                Derived::WebnnRelease(mHandle);
                mHandle = nullptr;
            }
            return static_cast<Derived&>(*this);
        }

        bool operator==(std::nullptr_t) const {
            return mHandle == nullptr;
        }
        bool operator!=(std::nullptr_t) const {
            return mHandle != nullptr;
        }

        explicit operator bool() const {
            return mHandle != nullptr;
        }
        CType GetHandle() const {
            return mHandle;
        }
        CType Release() {
            CType result = mHandle;
            mHandle = 0;
            return result;
        }
        static Derived Acquire(CType handle) {
            Derived result;
            result.mHandle = handle;
            return result;
        }

      protected:
        CType mHandle = nullptr;
    };

{% macro render_cpp_default_value(member) -%}
    {%- if member.annotation in ["*", "const*", "const*const*"] and member.optional -%}
        {{" "}}= nullptr
    {%- elif member.type.category in ["enum", "bitmask"] and member.default_value != None -%}
        {{" "}}= {{as_cppType(member.type.name)}}::{{as_cppEnum(Name(member.default_value))}}
    {%- elif member.type.category == "native" and member.default_value != None -%}
        {{" "}}= {{member.default_value}}
    {%- else -%}
        {{assert(member.default_value == None)}}
    {%- endif -%}
{%- endmacro %}

{% macro render_cpp_method_declaration(type, method) %}
    {% set CppType = as_cppType(type.name) %}
    {{as_cppType(method.return_type.name)}} {{method.name.CamelCase()}}(
        {%- for arg in method.arguments -%}
            {%- if not loop.first %}, {% endif -%}
            {%- if arg.type.category == "object" and arg.annotation == "value" -%}
                {{as_cppType(arg.type.name)}} const& {{as_varName(arg.name)}}
            {%- else -%}
                {{as_annotated_cppType(arg)}}
            {%- endif -%}
            {{render_cpp_default_value(arg)}}
        {%- endfor -%}
    ) const
{%- endmacro %}

    {% for type in by_category["object"] %}
        {% set CppType = as_cppType(type.name) %}
        {% set CType = as_cType(type.name) %}
        class {{CppType}} : public ObjectBase<{{CppType}}, {{CType}}> {
          public:
            using ObjectBase::ObjectBase;
            using ObjectBase::operator=;

            {% for method in type.methods %}
                {{render_cpp_method_declaration(type, method)}};
            {% endfor %}

          private:
            friend ObjectBase<{{CppType}}, {{CType}}>;
            static void WebnnReference({{CType}} handle);
            static void WebnnRelease({{CType}} handle);
        };

    {% endfor %}

    struct ChainedStruct {
        ChainedStruct const * nextInChain = nullptr;
        // SType sType = SType::Invalid;
    };

    {% for type in by_category["structure"] %}
        {% if type.chained %}
            struct {{as_cppType(type.name)}} : ChainedStruct {
                {{as_cppType(type.name)}}() {
                    sType = SType::{{type.name.CamelCase()}};
                }
        {% else %}
            struct {{as_cppType(type.name)}} {
        {% endif %}
            {% if type.extensible %}
                ChainedStruct const * nextInChain = nullptr;
            {% endif %}
            {% for member in type.members %}
                {% set member_declaration = as_annotated_cppType(member) + render_cpp_default_value(member) %}
                {% if type.chained and loop.first %}
                    //* Align the first member to ChainedStruct to match the C struct layout.
                    alignas(ChainedStruct) {{member_declaration}};
                {% else %}
                    {{member_declaration}};
                {% endif %}
            {% endfor %}
        };

    {% endfor %}

    GraphBuilder CreateGraphBuilder(Context context);
    NamedInputs CreateNamedInputs();
    NamedOperands CreateNamedOperands();
    NamedOutputs CreateNamedOutputs();
    OperatorArray CreateOperatorArray();

}  // namespace webnn

#endif // WEBNN_CPP_H_
