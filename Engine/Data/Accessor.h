#ifndef ENGINE_ACCESSOR_INCLUDED

#ifndef ENGINE_ACCESSOR_NO_SETTER
    #ifdef ENGINE_ACCESSOR_NO_GETTER
        #define ENGINE_ACCESSOR_INCLUDED
    #endif
#endif

#include "../Engine.dec.h"

#if ENGINE_ACCESSOR_NO_SETTER
    #define ENGINE_ACCESSOR_CLASS_NAME Accessor<Type, false, true>
#elif ENGINE_ACCESSOR_NO_GETTER
    #define ENGINE_ACCESSOR_CLASS_NAME Accessor<Type, true, false>
#else
    #define ENGINE_ACCESSOR_CLASS_NAME Accessor<Type, true, true>
#endif

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        class ENGINE_ACCESSOR_CLASS_NAME final
        {

#if ENGINE_ACCESSOR_NO_GETTER || ENGINE_ACCESSOR_NO_SETTER

            friend Accessor<Type, true, true>;

#endif
#if !defined(ENGINE_ACCESSOR_NO_GETTER) || ENGINE_ACCESSOR_NO_SETTER

            friend Accessor<Type, true, false>;

#endif

        public:

#if ENGINE_ACCESSOR_NO_GETTER

            /// @param OnSet Called on set, can be a lambda
            Accessor(std::function<void(Type)> OnSet);

#elif ENGINE_ACCESSOR_NO_SETTER

            /// @param OnGet Called on get, can be a lambda
            Accessor(std::function<Type()> OnGet);

#else

            /// @param OnSet Called on set, can be a lambda
            /// @param OnGet Called on get, can be a lambda
            Accessor(std::function<void(Type)> OnSet, std::function<Type()> OnGet);

#endif

            Accessor(const Accessor&) = delete;

#ifndef ENGINE_ACCESSOR_NO_SETTER

            ENGINE_ACCESSOR_CLASS_NAME& operator=(const Accessor<Type, true, true>&);
            ENGINE_ACCESSOR_CLASS_NAME& operator=(const Accessor<Type, false, true>&);
            ENGINE_ACCESSOR_CLASS_NAME& operator=(const Accessor<Type, true, false>&) = delete;

            ENGINE_ACCESSOR_CLASS_NAME& operator=(const Type&);
            /// @brief Sets a value. Can be done by an assignment.
            void Set(const Type&);

#else

            Accessor& operator=(const Accessor&) = delete;

#endif

#ifndef ENGINE_ACCESSOR_NO_GETTER

            operator Type();
            /// @brief Gets the value.
            ///        Can be done by using this object in an expression.
            Type Get();

#endif

        private:

#ifndef ENGINE_ACCESSOR_NO_SETTER

            std::function<void(Type)> OnSet;

#endif

#ifndef ENGINE_ACCESSOR_NO_GETTER

            std::function<Type()> OnGet;

#endif

        };
    }
}

// DEFINITION ----------------------------------------------------------------

namespace Engine
{
    namespace Data
    {
#if ENGINE_ACCESSOR_NO_GETTER

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME::Accessor(std::function<void(Type)> OnSet)
        {
            this->OnSet = OnSet;
        }

#elif ENGINE_ACCESSOR_NO_SETTER

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME::Accessor(std::function<Type()> OnGet)
        {
            this->OnGet = OnGet;
        }

#else

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME::Accessor(std::function<void(Type)> OnSet, std::function<Type()> OnGet)
        {
            this->OnSet = OnSet;
            this->OnGet = OnGet;
        }

#endif

#ifndef ENGINE_ACCESSOR_NO_SETTER

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME& ENGINE_ACCESSOR_CLASS_NAME::operator=(const Accessor<Type, true, true>& Operand)
        {
            Type Value = Operand.OnGet();
            OnSet(Value);
            return *this;
        }

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME& ENGINE_ACCESSOR_CLASS_NAME::operator=(const Accessor<Type, false, true>& Operand)
        {
            Type Value = Operand.OnGet();
            OnSet(Value);
            return *this;
        }

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME& ENGINE_ACCESSOR_CLASS_NAME::operator=(const Type& Value)
        {
            OnSet(Value);
            return *this;
        }

        template <typename Type>
        void ENGINE_ACCESSOR_CLASS_NAME::Set(const Type& Value)
        {
            OnSet(Value);
        }

#endif

#ifndef ENGINE_ACCESSOR_NO_GETTER

        template <typename Type>
        ENGINE_ACCESSOR_CLASS_NAME::operator Type()
        {
            return OnGet();
        }

        template <typename Type>
        Type ENGINE_ACCESSOR_CLASS_NAME::Get()
        {
            return OnGet();
        }

#endif

    }
}

#undef ENGINE_ACCESSOR_CLASS_NAME

#ifndef ENGINE_ACCESSOR_FIRST_INCLUDE

    #define ENGINE_ACCESSOR_FIRST_INCLUDE

    #define ENGINE_ACCESSOR_NO_SETTER 1
    #include "Accessor.h"
    #undef ENGINE_ACCESSOR_NO_SETTER

    #define ENGINE_ACCESSOR_NO_GETTER 1
    #include "Accessor.h"
    #undef ENGINE_ACCESSOR_NO_GETTER

#endif

#endif // Include Guard
