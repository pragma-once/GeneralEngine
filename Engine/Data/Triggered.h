#ifndef ENGINE_TRIGGERED_INCLUDED

#ifndef ENGINE_TRIGGERED_NO_SETTER
    #ifdef ENGINE_TRIGGERED_NO_GETTER
        #define ENGINE_TRIGGERED_INCLUDED
    #endif
#endif

#include "../Engine.dec.h"

#if ENGINE_TRIGGERED_NO_SETTER
    #define ENGINE_TRIGGERED_CLASS_NAME Triggered<Type, false, true>
#elif ENGINE_TRIGGERED_NO_GETTER
    #define ENGINE_TRIGGERED_CLASS_NAME Triggered<Type, true, false>
#else
    #define ENGINE_TRIGGERED_CLASS_NAME Triggered<Type, true, true>
#endif

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        class ENGINE_TRIGGERED_CLASS_NAME final
        {

#if ENGINE_TRIGGERED_NO_GETTER || ENGINE_TRIGGERED_NO_SETTER

            friend Triggered<Type, true, true>;

#endif
#if !defined(ENGINE_TRIGGERED_NO_GETTER) || ENGINE_TRIGGERED_NO_SETTER

            friend Triggered<Type, true, false>;

#endif

        public:

#if ENGINE_TRIGGERED_NO_GETTER

            /// @param OnSet Called on set, can be a lambda
            Triggered(std::function<void(Type)> OnSet);

#elif ENGINE_TRIGGERED_NO_SETTER

            /// @param OnGet Called on get, can be a lambda
            Triggered(std::function<Type()> OnGet);

#else

            /// @param OnSet Called on set, can be a lambda
            /// @param OnGet Called on get, can be a lambda
            Triggered(std::function<void(Type)> OnSet, std::function<Type()> OnGet);

#endif

            Triggered(const Triggered&) = delete;

#ifndef ENGINE_TRIGGERED_NO_SETTER

            ENGINE_TRIGGERED_CLASS_NAME& operator=(const Triggered<Type, true, true>&);
            ENGINE_TRIGGERED_CLASS_NAME& operator=(const Triggered<Type, false, true>&);
            ENGINE_TRIGGERED_CLASS_NAME& operator=(const Triggered<Type, true, false>&) = delete;

            ENGINE_TRIGGERED_CLASS_NAME& operator=(const Type&);
            /// @brief Sets a value. Can be done by an assignment.
            void Set(const Type&);

#else

            Triggered& operator=(const Triggered&) = delete;

#endif

#ifndef ENGINE_TRIGGERED_NO_GETTER

            operator Type();
            /// @brief Gets the value.
            ///        Can be done by using this object in an expression.
            Type Get();

#endif

        private:

#ifndef ENGINE_TRIGGERED_NO_SETTER

            std::function<void(Type)> OnSet;

#endif

#ifndef ENGINE_TRIGGERED_NO_GETTER

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
#if ENGINE_TRIGGERED_NO_GETTER

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME::Triggered(std::function<void(Type)> OnSet)
        {
            this->OnSet = OnSet;
        }

#elif ENGINE_TRIGGERED_NO_SETTER

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME::Triggered(std::function<Type()> OnGet)
        {
            this->OnGet = OnGet;
        }

#else

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME::Triggered(std::function<void(Type)> OnSet, std::function<Type()> OnGet)
        {
            this->OnSet = OnSet;
            this->OnGet = OnGet;
        }

#endif

#ifndef ENGINE_TRIGGERED_NO_SETTER

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME& ENGINE_TRIGGERED_CLASS_NAME::operator=(const Triggered<Type, true, true>& Operand)
        {
            Type Value = Operand.OnGet();
            OnSet(Value);
            return *this;
        }

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME& ENGINE_TRIGGERED_CLASS_NAME::operator=(const Triggered<Type, false, true>& Operand)
        {
            Type Value = Operand.OnGet();
            OnSet(Value);
            return *this;
        }

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME& ENGINE_TRIGGERED_CLASS_NAME::operator=(const Type& Value)
        {
            OnSet(Value);
            return *this;
        }

        template <typename Type>
        void ENGINE_TRIGGERED_CLASS_NAME::Set(const Type& Value)
        {
            OnSet(Value);
        }

#endif

#ifndef ENGINE_TRIGGERED_NO_GETTER

        template <typename Type>
        ENGINE_TRIGGERED_CLASS_NAME::operator Type()
        {
            return OnGet();
        }

        template <typename Type>
        Type ENGINE_TRIGGERED_CLASS_NAME::Get()
        {
            return OnGet();
        }

#endif

    }
}

#undef ENGINE_TRIGGERED_CLASS_NAME

#ifndef ENGINE_TRIGGERED_FIRST_INCLUDE

    #define ENGINE_TRIGGERED_FIRST_INCLUDE

    #define ENGINE_TRIGGERED_NO_SETTER 1
    #include "Triggered.h"
    #undef ENGINE_TRIGGERED_NO_SETTER

    #define ENGINE_TRIGGERED_NO_GETTER 1
    #include "Triggered.h"
    #undef ENGINE_TRIGGERED_NO_GETTER

#endif

#endif // Include Guard
