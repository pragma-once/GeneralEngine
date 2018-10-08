#ifndef ENGINE_PROPERTY_INCLUDED

#ifndef ENGINE_PROPERTY_NO_SETTER
    #ifdef ENGINE_PROPERTY_NO_GETTER
        #define ENGINE_PROPERTY_INCLUDED
    #endif
#endif

#include "../Engine.dec.h"

template <typename Type>
class Property<Type, true, true>
{
public:
    Property(const Property&) = delete;

    Property() {}

    Property(

#ifndef ENGINE_PROPERTY_NO_SETTER
        std::function<void(Type)> Setter
#endif

#ifndef ENGINE_PROPERTY_NO_GETTER
        , std::function<Type()> Getter
#endif

    )
    {

#ifndef ENGINE_PROPERTY_NO_SETTER
        this->Setter = Setter;
#endif
#ifndef ENGINE_PROPERTY_NO_GETTER
        this->Getter = Getter;
#endif

    }

#ifndef ENGINE_PROPERTY_NO_SETTER

    void SetSetter(std::function<void(Type)> Setter)
    {
        if (this->Setter != nullptr)
            throw std::logic_error("Cannot set the Setter more than once.");
        this->Setter = Setter;
    }

#endif

#ifndef ENGINE_PROPERTY_NO_GETTER

    void SetGetter(std::function<void(Type)> Getter)
    {
        if (this->Getter != nullptr)
            throw std::logic_error("Cannot set the Getter more than once.");
        this->Getter = Getter;
    }

#endif

#ifndef defined(ENGINE_PROPERTY_NO_SETTER) && defined(ENGINE_PROPERTY_NO_GETTER)

    int operator=(const Property& Operand)
    {
        Type Value = Operand.Getter();
        Setter(Value);
        return Value;
    }

#else

    int operator=(const Property& Operand) = delete;

#endif

#ifndef ENGINE_PROPERTY_NO_SETTER

    int operator=(const Type& Value)
    {
        Setter(Value);
        return Value;
    }

#endif

#ifndef ENGINE_PROPERTY_NO_GETTER

    operator Type()
    {
        return Getter();
    }

#endif

private:

#ifndef ENGINE_PROPERTY_NO_SETTER

    std::function<void(Type)> Setter = nullptr;

#endif

#ifndef ENGINE_PROPERTY_NO_GETTER

    std::function<Type()> Getter = nullptr;

#endif

};

#define ENGINE_PROPERTY_NO_SETTER
#include "Property.h"
#undef ENGINE_PROPERTY_NO_SETTER

#define ENGINE_PROPERTY_NO_GETTER
#include "Property.h"
#undef ENGINE_PROPERTY_NO_GETTER

#endif // Include Guard