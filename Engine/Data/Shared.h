#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        class Shared
        {
        public:
            Shared(const Shared&);
            Shared(const Type&);
            Type& operator=(const Shared&);
            Type& operator=(const Type&);
            operator Type();

            Type Set(const Type&);
            Type Get();
        private:
            Type Value;
            std::shared_mutex Mutex;
        };
    }
}

// DEFINITION ----------------------------------------------------------------

namespace Engine
{
    namespace Data
    {
        template <typename Type>
        Shared<Type>::Shared(const Shared& Operand)
        {
            std::shared_lock<std::shared_mutex> guard(Operand.Mutex);
            Type Value = Operand.Value;
            guard.unlock();
            std::lock_guard<std::shared_mutex> guard(Mutex);
            this->Value = Value;
        }

        template <typename Type>
        Shared<Type>::Shared(const Type& Value)
        {
            std::lock_guard<std::shared_mutex> Guard(Mutex);
            this->Value = Value;
        }

        template <typename Type>
        Type& Shared<Type>::operator=(const Shared& Operand)
        {
            std::shared_lock<std::shared_mutex> guard(Operand.Mutex);
            Type Value = Operand.Value;
            guard.unlock();
            std::lock_guard<std::shared_mutex> guard(Mutex);
            this->Value = Value;
            return Value;
        }

        template <typename Type>
        Type& Shared<Type>::operator=(const Type& Value)
        {
            std::lock_guard<std::shared_mutex> guard(Mutex);
            this->Value = Value;
            return Value;
        }

        template <typename Type>
        Shared<Type>::operator Type()
        {
            std::shared_lock<std::shared_mutex> guard(Mutex);
            return Value;
        }

        template <typename Type>
        Type Shared<Type>::Set(const Type& Value)
        {
            std::lock_guard<std::shared_mutex> guard(Mutex);
            this->Value = Value;
            return Value;
        }

        template <typename Type>
        Type Shared<Type>::Get()
        {
            std::shared_lock<std::shared_mutex> guard(Mutex);
            return Value;
        }
    }
}