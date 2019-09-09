#ifndef ENGINE_RESIZABLE_ARRAY_INCLUDED

#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_RESIZABLE_ARRAY_INCLUDED
#endif

#include "../../Engine.dec.h"

#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_RESIZABLE_ARRAY_CLASS_NAME ResizableArray<ItemsType, true>
#else
    #define ENGINE_RESIZABLE_ARRAY_CLASS_NAME ResizableArray<ItemsType, false>
#endif

namespace Engine
{
    namespace Utilities
    {
        namespace Collections
        {
            template <typename ItemsType>
            class ENGINE_RESIZABLE_ARRAY_CLASS_NAME final
            {
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                friend ResizableArray<ItemsType, false>;
#else
                friend ResizableArray<ItemsType, true>;
#endif
            public:
                ResizableArray(int Length = 0);
                ~ResizableArray();

                ResizableArray(ResizableArray<ItemsType, true>&) noexcept;
                ResizableArray(ResizableArray<ItemsType, true>&&) noexcept;
                ResizableArray(ResizableArray<ItemsType, false>&) noexcept;
                ResizableArray(ResizableArray<ItemsType, false>&&) noexcept;

                ResizableArray& operator=(ResizableArray<ItemsType, true>) noexcept;
                ResizableArray& operator=(ResizableArray<ItemsType, false>) noexcept;

                /// @brief Gets an item at a specified index.
                ItemsType GetItem(int Index);
                /// @brief Sets an item at a specified index.
                void SetItem(int Index, ItemsType Value);
                /// @brief Gets the current length of the array.
                int GetLength();
                /// @brief Sets the new length of the array.
                ///
                /// Some array items will be removed if NewLength < current length.
                /// This function reallocates the whole array over.
                void Resize(int NewLength);
            private:
#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
                std::shared_mutex Mutex;
#endif
                ItemsType * Array;
                int Length;
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_COLLECTION_WRITE_ACCESS std::lock_guard<std::shared_mutex> guard(Mutex);
    #define ENGINE_COLLECTION_READ_ACCESS std::shared_lock<std::shared_mutex> guard(Mutex);
#else
    #define ENGINE_COLLECTION_WRITE_ACCESS ;
    #define ENGINE_COLLECTION_READ_ACCESS ;
#endif

namespace Engine
{
    namespace Utilities
    {
        namespace Collections
        {
            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(int Length)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Length > 0)
                {
                    Array = new ItemsType[Length];
                    this->Length = Length;
                }
                else if (Length == 0)
                {
                    Array = nullptr;
                    this->Length = 0;
                }
                else throw std::domain_error("Length is less than zero.");
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::~ResizableArray()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Array != nullptr)
                    delete[] Array;
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(ResizableArray<ItemsType, true>& Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                std::shared_lock<std::shared_mutex> op_guard(Op.Mutex);
                Length = Op.Length;
                if (Op.Length == 0) Array = nullptr;
                else
                {
                    Array = new ItemsType[Op.Length];
                    std::copy(Op.Array, Op.Array + Op.Length, Array);
                }
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(ResizableArray<ItemsType, true>&& Op) noexcept : ResizableArray()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                std::shared_lock<std::shared_mutex> op_guard(Op.Mutex);
                std::swap(Length, Op.Length);
                std::swap(Array, Op.Array);
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(ResizableArray<ItemsType, false>& Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                Length = Op.Length;
                if (Op.Length == 0) Array = nullptr;
                else
                {
                    Array = new ItemsType[Op.Length];
                    std::copy(Op.Array, Op.Array + Op.Length, Array);
                }
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME::ResizableArray(ResizableArray<ItemsType, false>&& Op) noexcept : ResizableArray()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                std::swap(Length, Op.Length);
                std::swap(Array, Op.Array);
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME& ENGINE_RESIZABLE_ARRAY_CLASS_NAME::operator=(ResizableArray<ItemsType, true> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                std::shared_lock<std::shared_mutex> op_guard(Op.Mutex);
                std::swap(Length, Op.Length);
                std::swap(Array, Op.Array);
                return *this;
            }

            template <typename ItemsType>
            ENGINE_RESIZABLE_ARRAY_CLASS_NAME& ENGINE_RESIZABLE_ARRAY_CLASS_NAME::operator=(ResizableArray<ItemsType, false> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                std::swap(Length, Op.Length);
                std::swap(Array, Op.Array);
                return *this;
            }

            template <typename ItemsType>
            ItemsType ENGINE_RESIZABLE_ARRAY_CLASS_NAME::GetItem(int Index)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Index >= 0 && Index < Length)
                    return Array[Index];
                throw std::out_of_range("Index is out of range.");
            }

            template <typename ItemsType>
            void ENGINE_RESIZABLE_ARRAY_CLASS_NAME::SetItem(int Index, ItemsType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Index >= 0 && Index < Length)
                    Array[Index] = Value;
                else
                    throw std::out_of_range("Index is out of range.");
            }

            template <typename ItemsType>
            int ENGINE_RESIZABLE_ARRAY_CLASS_NAME::GetLength()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Length;
            }
            template <typename ItemsType>
            void ENGINE_RESIZABLE_ARRAY_CLASS_NAME::Resize(int NewLength)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (NewLength > 0)
                {
                    ItemsType * NewArray = new ItemsType[NewLength];
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

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_RESIZABLE_ARRAY_CLASS_NAME

#ifndef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #define ENGINE_RESIZABLE_ARRAY_USE_MUTEX
    #include "ResizableArray.h"
    #undef ENGINE_RESIZABLE_ARRAY_USE_MUTEX
#endif

#endif // Include Guard
