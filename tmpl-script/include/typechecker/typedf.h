#ifndef TYPECHECKER_TYPEDFN_H
#define TYPECHECKER_TYPEDFN_H

#include "include/interpreter/environment.h"
#include<string>

namespace Runtime
{
    class TypeDfCast
    {
    public:
        using PFn = std::shared_ptr<Fn>;
    private:
        std::string m_origin; // cast from
        std::string m_target; // cast to
        PFn m_caster;

    public:
        TypeDfCast(std::string origin, std::string target, PFn caster)
            : m_origin(origin), m_target(target), m_caster(caster) { }

    public:
        inline std::string GetOrigin() const { return m_origin; }
        inline std::string GetTarget() const { return m_target; }
        inline PFn GetCaster() const { return m_caster; }
    };

    class TypeDf
    {
    public:
        using Casts = Environment<TypeDfCast>;
        using PCasts = std::shared_ptr<Casts>;
        using PFn = std::shared_ptr<Fn>;
    private:
        std::string m_typename;
        PValType m_basename;
        PCasts m_casts;
        PFn m_constructor;

    public:
        TypeDf(std::string tName, PValType basename)
            : m_typename(tName), m_basename(basename),
              m_casts(std::make_shared<Casts>()),
              m_constructor(nullptr) { }
    
    public:
        inline std::string GetTypeName() const { return m_typename; }
        inline PValType GetBaseType() const { return m_basename; }
        inline PCasts GetCastsEnv() const { return m_casts; }

        inline bool HasConstructor() const { return m_constructor != nullptr; }
        inline PFn GetConstructor() const { return m_constructor; }

    public:
        void SetConstructor(PFn constructor) { m_constructor = constructor; }
    };
}

#endif // TYPECHECKER_TYPEDFN_H
