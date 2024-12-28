#pragma once
#include <cstdio>   // I need it for size_t

#include <iostream>

namespace Galvanizer
{
class GalvanizerObject;
using GObjHNDL = GalvanizerObject*;
class WeakRef;

class OwningRef
{
public:
    OwningRef() = default;
    explicit OwningRef(size_t index);
    OwningRef(OwningRef const& right);
    OwningRef(OwningRef&& right) noexcept;
    OwningRef& operator=(OwningRef const& right);
    OwningRef& operator=(OwningRef&& right) noexcept;

    ~OwningRef();

    void DropOwnership(bool deleteObj = true);

    [[nodiscard]] GObjHNDL get() const;
    [[nodiscard]] GObjHNDL operator->() const; // Same as get()
    [[nodiscard]] operator GObjHNDL() const;

    operator WeakRef() const;

private:
    size_t m_DBIndex = -1;
    GObjHNDL m_hndl = nullptr;

    friend OwningRef CreateOwningRef(GObjHNDL hndl);
};

OwningRef CreateOwningRef(GObjHNDL hndl);


class WeakRef
{
public:
    WeakRef() = default;
    explicit WeakRef(size_t index);
    WeakRef(WeakRef const& right);
    WeakRef(WeakRef&& right) noexcept;
    WeakRef& operator=(WeakRef const& right);
    WeakRef& operator=(WeakRef&& right) noexcept;

    ~WeakRef();

    [[nodiscard]] OwningRef lock() const;
    void DropOwnership();

private:
    size_t m_DBIndex = -1;
    GObjHNDL m_hndl = nullptr;
};
}
