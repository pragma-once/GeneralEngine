#ifndef ENGINE_RESIZABLE_ARRAY_INCLUDED

#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_RESIZABLE_ARRAY_INCLUDED
#endif

#include "../../Engine.dec.h"

#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_RESIZABLE_ARRAY_CLASS_NAME ResizableArray<T, true>
#else
    #define ENGINE_RESIZABLE_ARRAY_CLASS_NAME ResizableArray<T, false>
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename T>
            class ENGINE_RESIZABLE_ARRAY_CLASS_NAME
            {
            public:
                ResizableArray(int Length = 0);
                ~ResizableArray();

                ResizableArray(const ResizableArray<T, true>&);
                ResizableArray& operator=(const ResizableArray<T, true>&);
                ResizableArray(const ResizableArray<T, false>&);
                ResizableArray& operator=(const ResizableArray<T, false>&);

                T GetItem(int Index);
                void SetItem(int Index, T Value);
                int GetLength();
                void Resize(int NewLength);
            private:
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::shared_mutex Mutex;
#endif
                T * Array;
                int Length;
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename T>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(int Length)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif

                if (Length > 0)
                {
                    Array = new T[Length];
                    this->Length = Length;
                }
                else if (Length == 0)
                {
                    Array = nullptr;
                    this->Length = 0;
                }
                else throw std::domain_error("Length is less than zero.");
            }

            template <typename T>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::~ResizableArray()
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif

                if (Array != nullptr)
                    delete[] Array;
            }

            template <typename T>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(const ResizableArray<T, true>& Op)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif
                std::shared_lock<std::shared_mutex> op_guard(Op.Mutex);
                Length = Op.Length;
                if (Op.Length == 0) Array = nullptr;
                else
                {
                    Array = new T[Op.Length];
                    std::copy(Op.Array, Op.Array + Op.Length, Array);
                }
            }

            template <typename T>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME& ENGINE_RESIZABLE_ARRAY_CLASS_NAME::operator=(const ResizableArray<T, true>& Op)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif
                std::shared_lock<std::shared_mutex> op_guard(Op.Mutex);
                if (Op.Length == 0) Array = nullptr;
                else
                {
                    if (Length != Op.Length)
                    {
                        Length = Op.Length;
                        if (Array != nullptr)
                            delete[] Array;
                        Array = new T[Op.Length];
                    }

                    Array = new T[Op.Length];
                    std::copy(Op.Array, Op.Array + Op.Length, Array);
                }
            }

            template <typename T>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(const ResizableArray<T, false>& Op)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif
                Length = Op.Length;
                if (Op.Length == 0) Array = nullptr;
                else
                {
                    Array = new T[Op.Length];
                    std::copy(Op.Array, Op.Array + Op.Length, Array);
                }
            }

            template <typename T>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME& ENGINE_RESIZABLE_ARRAY_CLASS_NAME::operator=(const ResizableArray<T, false>& Op)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif
                if (Op.Length == 0) Array = nullptr;
                else
                {
                    if (Length != Op.Length)
                    {
                        Length = Op.Length;
                        if (Array != nullptr)
                            delete[] Array;
                        Array = new T[Op.Length];
                    }

                    Array = new T[Op.Length];
                    std::copy(Op.Array, Op.Array + Op.Length, Array);
                }
            }

            template <typename T>
            T ENGINE_RESIZABLE_ARRAY_CLASS_NAME::GetItem(int Index)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::shared_lock<std::shared_mutex> guard(Mutex);
#endif

                if (Index >= 0 && Index < Length)
                    return Array[Index];
                throw std::out_of_range("Index is out of range.");
            }

            template <typename T>
            void ENGINE_RESIZABLE_ARRAY_CLASS_NAME::SetItem(int Index, T Value)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif

                if (Index >= 0 && Index < Length)
                    Array[Index] = Value;
                else
                    throw std::out_of_range("Index is out of range.");
            }

            template <typename T>
            int ENGINE_RESIZABLE_ARRAY_CLASS_NAME::GetLength()
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::shared_lock<std::shared_mutex> guard(Mutex);
#endif

                return Length;
            }
            template <typename T>
            void ENGINE_RESIZABLE_ARRAY_CLASS_NAME::Resize(int NewLength)
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::lock_guard<std::shared_mutex> guard(Mutex);
#endif

                if (NewLength > 0)
                {
                    T * NewArray = new T[NewLength];
                    int MinimumLength = NewLength > Length ? Length : NewLength;

                    for (int i = 0; i < MinimumLength; i++)
                        NewArray[i] = Array[i];

                    if (Array != nullptr)
                        delete[] Array;

                    Array = NewArray;
                    Length = NewLength;
                }
                else if (NewLength == 0)
                {
                    if (Array != nullptr)
                        delete[] Array;
                    Array = nullptr;
                    Length = 0;
                }
                else throw std::domain_error("NewLength is less than zero.");
            }
        }
    }
}

#undef ENGINE_RESIZABLE_ARRAY_CLASS_NAME

#ifndef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #include "ResizableArray.h"
    #undef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
#endif

#endif // Include Guard
