#ifndef ENGINE_PROPERTY_INCLUDED

#ifndef ENGINE_PROPERTY_NO_SETTER
    #ifdef ENGINE_PROPERTY_NO_GETTER
        #define ENGINE_PROPERTY_INCLUDED
    #endif
#endif

#include "../Engine.dec.h"

#if ENGINE_PROPERTY_NO_SETTER
    #define ENGINE_PROPERTY_CLASS_NAME Property<Type, false, true>
#elif ENGINE_PROPERTY_NO_GETTER
    #define ENGINE_PROPERTY_CLASS_NAME Property<Type, true, false>
#else
    #define ENGINE_PROPERTY_CLASS_NAME Property<Type, true, true>
#endif

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        class ENGINE_PROPERTY_CLASS_NAME final
        {

#if ENGINE_PROPERTY_NO_GETTER || ENGINE_PROPERTY_NO_SETTER

            friend Property<Type, true, true>;

#endif
#if !defined(ENGINE_PROPERTY_NO_GETTER) || ENGINE_PROPERTY_NO_SETTER

            friend Property<Type, true, false>;

#endif

        public:

#if ENGINE_PROPERTY_NO_GETTER

            /// @param Setter The setter function, can be a lambda
            Property(std::function<void(Type)> Setter);

#elif ENGINE_PROPERTY_NO_SETTER

            /// @param Getter The getter function, can be a lambda
            Property(std::function<Type()> Getter);

#else

            /// @param Setter The setter function, can be a lambda
            /// @param Getter The getter function, can be a lambda
            Property(std::function<void(Type)> Setter, std::function<Type()> Getter);

#endif

            Property(const Property&) = delete;

#ifndef ENGINE_PROPERTY_NO_SETTER

            ENGINE_PROPERTY_CLASS_NAME& operator=(const Property<Type, true, true>&);
            ENGINE_PROPERTY_CLASS_NAME& operator=(const Property<Type, false, true>&);
            ENGINE_PROPERTY_CLASS_NAME& operator=(const Property<Type, true, false>&) = delete;

            ENGINE_PROPERTY_CLASS_NAME& operator=(const Type&);
            /// @brief Sets the property. Can be done by an assignment.
            void Set(const Type&);

#else

            Property& operator=(const Property&) = delete;

#endif

#ifndef ENGINE_PROPERTY_NO_GETTER

            operator Type();
            /// @brief Gets the property.
            ///        Can be done by using this property in an expression.
            Type Get();

#endif

        private:

#ifndef ENGINE_PROPERTY_NO_SETTER

            std::function<void(Type)> Setter;

#endif

#ifndef ENGINE_PROPERTY_NO_GETTER

            std::function<Type()> Getter;

#endif

        };
    }
}

// DEFINITION ----------------------------------------------------------------

namespace Engine
{
    namespace Data
    {
#if ENGINE_PROPERTY_NO_GETTER

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME::Property(std::function<void(Type)> Setter)
        {
            this->Setter = Setter;
        }

#elif ENGINE_PROPERTY_NO_SETTER

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME::Property(std::function<Type()> Getter)
        {
            this->Getter = Getter;
        }

#else

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME::Property(std::function<void(Type)> Setter, std::function<Type()> Getter)
        {
            this->Setter = Setter;
            this->Getter = Getter;
        }

#endif

#ifndef ENGINE_PROPERTY_NO_SETTER

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME& ENGINE_PROPERTY_CLASS_NAME::operator=(const Property<Type, true, true>& Operand)
        {
            Type Value = Operand.Getter();
            Setter(Value);
            return *this;
        }

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME& ENGINE_PROPERTY_CLASS_NAME::operator=(const Property<Type, false, true>& Operand)
        {
            Type Value = Operand.Getter();
            Setter(Value);
            return *this;
        }

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME& ENGINE_PROPERTY_CLASS_NAME::operator=(const Type& Value)
        {
            Setter(Value);
            return *this;
        }

        template <typename Type>
        void ENGINE_PROPERTY_CLASS_NAME::Set(const Type& Value)
        {
            Setter(Value);
        }

#endif

#ifndef ENGINE_PROPERTY_NO_GETTER

        template <typename Type>
        ENGINE_PROPERTY_CLASS_NAME::operator Type()
        {
            return Getter();
        }

        template <typename Type>
        Type ENGINE_PROPERTY_CLASS_NAME::Get()
        {
            return Getter();
        }

#endif

    }
}

#undef ENGINE_PROPERTY_CLASS_NAME

#ifndef ENGINE_PROPERTY_FIRST_INCLUDE

    #define ENGINE_PROPERTY_FIRST_INCLUDE

    #define ENGINE_PROPERTY_NO_SETTER 1
    #include "Property.h"
    #undef ENGINE_PROPERTY_NO_SETTER

    #define ENGINE_PROPERTY_NO_GETTER 1
    #include "Property.h"
    #undef ENGINE_PROPERTY_NO_GETTER

#endif

#endif // Include Guard
