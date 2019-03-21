#ifndef ENGINE_SHARED_INCLUDED

#ifdef ENGINE_SHARED_MANUAL
    #define ENGINE_SHARED_INCLUDED
#endif

#include "../Engine.dec.h"

#ifdef ENGINE_SHARED_MANUAL
    #include "HandledMutex.h"
    #define ENGINE_SHARED_CLASS_NAME Shared<Type, true>
#else
    #define ENGINE_SHARED_CLASS_NAME Shared<Type, false>
#endif

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        class ENGINE_SHARED_CLASS_NAME final
        {
#ifdef ENGINE_SHARED_MANUAL
                friend Shared<Type, false>;
#else
                friend Shared<Type, true>;
#endif
        public:

#ifdef ENGINE_SHARED_MANUAL
            /// @brief The mutex used by this shared variable.
            ///
            /// Can be used to perform locks in larger/other program sections.
            HandledMutex Mutex;
#endif

            Shared(Shared<Type, false>&);
            Shared(Shared<Type, true>&);
            Shared(const Type&);
            ENGINE_SHARED_CLASS_NAME& operator=(Shared<Type, false>&);
            ENGINE_SHARED_CLASS_NAME& operator=(Shared<Type, true>&);
            ENGINE_SHARED_CLASS_NAME& operator=(const Type&);
            operator Type();

            /// @brief Sets the shared variable. Can be done by an assignment.
            void Set(const Type&);
            /// @brief Gets the shared variable.
            ///        Can be done by using this variable in an expression.
            Type Get();
        private:
            Type Value;

#ifndef ENGINE_SHARED_MANUAL
            std::shared_mutex Mutex;
#endif
        };
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_SHARED_MANUAL
    #define ENGINE_SHARED_WRITE_ACCESS auto guard = Mutex.GetLock();
    #define ENGINE_SHARED_READ_ACCESS auto guard = Mutex.GetSharedLock();
#else
    #define ENGINE_SHARED_WRITE_ACCESS std::lock_guard<std::shared_mutex> guard(Mutex);
    #define ENGINE_SHARED_READ_ACCESS std::shared_lock<std::shared_mutex> guard(Mutex);
#endif

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        ENGINE_SHARED_CLASS_NAME::Shared(Shared<Type, false>& Operand)
        {
            std::shared_lock<std::shared_mutex> guard_op(Operand.Mutex);
            Type Value = Operand.Value;
            guard_op.unlock();
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
        }

        template <typename Type>
        ENGINE_SHARED_CLASS_NAME::Shared(Shared<Type, true>& Operand)
        {
            auto guard_op = Operand.Mutex.GetSharedLock();
            Type Value = Operand.Value;
            guard_op.Unlock();
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
        }

        template <typename Type>
        ENGINE_SHARED_CLASS_NAME::Shared(const Type& Value)
        {
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
        }

        template <typename Type>
        ENGINE_SHARED_CLASS_NAME& ENGINE_SHARED_CLASS_NAME::operator=(Shared<Type, false>& Operand)
        {
            std::shared_lock<std::shared_mutex> guard_op(Operand.Mutex);
            Type Value = Operand.Value;
            guard_op.unlock();
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
            return *this;
        }

        template <typename Type>
        ENGINE_SHARED_CLASS_NAME& ENGINE_SHARED_CLASS_NAME::operator=(Shared<Type, true>& Operand)
        {
            auto guard_op = Operand.Mutex.GetSharedLock();
            Type Value = Operand.Value;
            guard_op.Unlock();
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
            return *this;
        }

        template <typename Type>
        ENGINE_SHARED_CLASS_NAME& ENGINE_SHARED_CLASS_NAME::operator=(const Type& Value)
        {
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
            return *this;
        }

        template <typename Type>
        ENGINE_SHARED_CLASS_NAME::operator Type()
        {
            ENGINE_SHARED_READ_ACCESS;
            return Value;
        }

        template <typename Type>
        void ENGINE_SHARED_CLASS_NAME::Set(const Type& Value)
        {
            ENGINE_SHARED_WRITE_ACCESS;
            this->Value = Value;
        }

        template <typename Type>
        Type ENGINE_SHARED_CLASS_NAME::Get()
        {
            ENGINE_SHARED_READ_ACCESS;
            return Value;
        }
    }
}

#undef ENGINE_SHARED_WRITE_ACCESS
#undef ENGINE_SHARED_READ_ACCESS

#undef ENGINE_SHARED_CLASS_NAME

#ifndef ENGINE_SHARED_FIRST_INCLUDEENGINE_PROPERTY_FIRST_INCLUDE

    #define ENGINE_SHARED_FIRST_INCLUDE

    #define ENGINE_SHARED_MANUAL
    #include "Shared.h"
    #undef ENGINE_SHARED_MANUAL

#endif

#endif // Include Guard
