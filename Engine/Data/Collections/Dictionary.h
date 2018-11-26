#ifndef ENGINE_DICTIONARY_INCLUDED

#ifdef ENGINE_DICTIONARY_USE_MUTEX
#define ENGINE_DICTIONARY_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_DICTIONARY_USE_MUTEX
#include "../HandledMutex.h"
#define ENGINE_DICTIONARY_CLASS_NAME Dictionary<KeyType, ValueType, true>
#else
#define ENGINE_DICTIONARY_CLASS_NAME Dictionary<KeyType, ValueType, false>
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename KeyType, typename ValueType>
            class ENGINE_DICTIONARY_CLASS_NAME
            {
            public:
                typedef std::function<void(KeyType Key)> ForEachBody;
                typedef std::function<void(KeyType Key, bool& BreakLoop)> ForEachBodyWithBreakBool;
                typedef std::function<void(KeyType Key, std::function<void()> Break)> ForEachBodyWithBreakFunction;
                typedef std::function<void(KeyType Key, ValueType Value)> ForEachBodyWithValue;
                typedef std::function<void(KeyType Key, ValueType Value, bool& BreakLoop)> ForEachBodyWithValueWithBreakBool;
                typedef std::function<void(KeyType Key, ValueType Value, std::function<void()> Break)> ForEachBodyWithValueWithBreakFunction;

                Dictionary();
                ~Dictionary();

                Dictionary(const Dictionary<KeyType, ValueType, true>&);
                Dictionary& operator=(const Dictionary<KeyType, ValueType, true>&);
                Dictionary(const Dictionary<KeyType, ValueType, false>&);
                Dictionary& operator=(const Dictionary<KeyType, ValueType, false>&);

                void SetValue(KeyType Key, ValueType Value);
                void Remove(KeyType Key);
                void Clear();

                ValueType GetValue(KeyType Key);
                bool IsEmpty();

                void ForEach(ForEachBody Body);
                void ForEach(ForEachBodyWithBreakBool Body);
                void ForEach(ForEachBodyWithBreakFunction Body);
                void ForEach(ForEachBodyWithValue Body);
                void ForEach(ForEachBodyWithValueWithBreakBool Body);
                void ForEach(ForEachBodyWithValueWithBreakFunction Body);
            private:
                struct Pair
                {
                    Pair() {}
                    Pair(KeyType Key, ValueType Value) : Key(Key), Value(Value) {}
                    KeyType Key;
                    ValueType Value;
                };

                class LoopBreaker {};

#ifdef ENGINE_DICTIONARY_USE_MUTEX
                HandledMutex Mutex;
#endif
                int Count;
                ResizableArray<Pair, false> * Pairs;
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_DICTIONARY_USE_MUTEX
#define ENGINE_COLLECTION_WRITE_ACCESS auto guard = Mutex.GetLock();
#define ENGINE_COLLECTION_READ_ACCESS auto guard = Mutex.GetSharedLock();
#else
#define ENGINE_COLLECTION_WRITE_ACCESS ;
#define ENGINE_COLLECTION_READ_ACCESS ;
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Pairs = new ResizableArray<Pair, false>();
                Count = 0;
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::~Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete Pairs;
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary(const Dictionary<KeyType, ValueType, true>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *Pairs = *(Op.Pairs);
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME& ENGINE_DICTIONARY_CLASS_NAME::operator=(const Dictionary<KeyType, ValueType, true>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *Pairs = *(Op.Pairs);

                return *this;
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary(const Dictionary<KeyType, ValueType, false>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *Pairs = *(Op.Pairs);
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME& ENGINE_DICTIONARY_CLASS_NAME::operator=(const Dictionary<KeyType, ValueType, false>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *Pairs = *(Op.Pairs);

                return *this;
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::SetValue(KeyType Key, ValueType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= Pairs->GetLength())
                    if (Pairs->GetLength() > 0)
                        Pairs->Resize(Pairs->GetLength() * 2);
                    else
                        Pairs->Resize(1);

                int s = 0;
                int e = Count;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (Key < Pairs->GetItem(c).Key) e = c;
                    else s = c + 1;
                }

                for (int i = Count; i > s; i--)
                    Pairs->SetItem(i, Pairs->GetItem(i - 1));
                Pairs->SetItem(s, Pair(Key, Value));
                Count++;
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::Remove(KeyType Key)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                int s = 0;
                int e = Count - 1;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (Key == Pairs->GetItem(c).Key) { s = c; break; }
                    else if (Key < Pairs->GetItem(c).Key) e = c - 1;
                    else s = c + 1;
                }

                if (Pairs->GetItem(s).Key != Key)
                    throw std::domain_error("Key not found.");
                
                for (int i = s + 1; i < Count; i++)
                    Pairs->SetItem(i - 1, Pairs->GetItem(i));
                Count--;

                if (Count < Pairs->GetLength() / 2)
                    Pairs->Resize(Pairs->GetLength() / 2);
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = 0;
                Pairs->Resize(0);
            }

            template <typename KeyType, typename ValueType>
            ValueType ENGINE_DICTIONARY_CLASS_NAME::GetValue(KeyType Key)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                int s = 0;
                int e = Count - 1;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (Key == Pairs->GetItem(c).Key) { s = c; break; }
                    else if (Key < Pairs->GetItem(c).Key) e = c - 1;
                    else s = c + 1;
                }

                if (Pairs->GetItem(s).Key != Key)
                    throw std::domain_error("Key not found.");

                return Pairs->GetItem(s).Value;
            }

            template <typename KeyType, typename ValueType>
            bool ENGINE_DICTIONARY_CLASS_NAME::IsEmpty()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count == 0;
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBody Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < Count; i++)
                    Body(Pairs->GetItem(i).Key);
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithBreakBool Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                bool ShouldBreak = false;
                for (int i = 0; i < Count; i++)
                {
                    Body(Pairs->GetItem(i).Key, ShouldBreak);
                    if (ShouldBreak) break;
                }
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithBreakFunction Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                std::function<void()> BreakFunction = []() { throw LoopBreaker() };
                for (int i = 0; i < Count; i++) try
                {
                    Body(Pairs->GetItem(i).Key, BreakFunction);
                }
                catch (LoopBreaker&) { break; }
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithValue Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < Count; i++)
                    Body(Pairs->GetItem(i).Key, Pairs->GetItem(i).Value);
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithValueWithBreakBool Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                bool ShouldBreak = false;
                for (int i = 0; i < Count; i++)
                {
                    Body(Pairs->GetItem(i).Key, Pairs->GetItem(i).Value, ShouldBreak);
                    if (ShouldBreak) break;
                }
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithValueWithBreakFunction Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                std::function<void()> BreakFunction = []() { throw LoopBreaker() };
                for (int i = 0; i < Count; i++) try
                {
                    Body(Pairs->GetItem(i).Key, Pairs->GetItem(i).Value, BreakFunction);
                }
                catch (LoopBreaker&) { break; }
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_DICTIONARY_CLASS_NAME

#ifndef ENGINE_DICTIONARY_USE_MUTEX
#define ENGINE_DICTIONARY_USE_MUTEX
#include "Dictionary.h"
#undef ENGINE_DICTIONARY_USE_MUTEX
#endif

#endif // Include Guard