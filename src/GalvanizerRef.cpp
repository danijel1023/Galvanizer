#include "GalvanizerRef.h"
#include "GalvanizerObject.h"

#include <vector>
#include <mutex>
#include <stack>

using namespace Galvanizer;

struct RDB_Elm
{
    explicit RDB_Elm(GObjHNDL hndl) : hndl(hndl) {}

    int strongCount = 0;
    int weakCount = 0;
    GObjHNDL hndl = nullptr;
};

class RefDatabase
{
public:
    size_t CreateStrongRef(GObjHNDL hndl)
    {
        std::lock_guard lck(m_DBAccess);
        size_t index;

        if (m_freeElm.empty())
        {
            m_DB.emplace_back(hndl);
            index = m_DB.size() - 1;
        }
        else
        {
            index = m_freeElm.top();
            m_freeElm.pop();

            m_DB.at(index) = RDB_Elm(hndl);
        }

        m_DB.at(index).strongCount++;
        return index;
    }

    void addStrongRef(size_t index)
    {
        if (index == static_cast<size_t>(-1))
            return;

        std::lock_guard lck(m_DBAccess);
        m_DB.at(index).strongCount++;
    }

    void addWeakRef(size_t index)
    {
        if (index == static_cast<size_t>(-1))
            return;

        std::lock_guard lck(m_DBAccess);
        m_DB.at(index).weakCount++;
    }

    void removeStrongRef(size_t* index, bool deleteObj)
    {
        if (*index == static_cast<size_t>(-1))
            return;

        std::lock_guard lck(m_DBAccess);
        RDB_Elm& elm = m_DB.at(*index);

        if (elm.strongCount == 1)
        {
            if (deleteObj)
                delete elm.hndl;

            elm.hndl = nullptr;
        }

        // Decrement-after-deletion - if the object has a weak ref to itself, and we delete it, it will
        // result in 2 calls to freeElmIfDead
        elm.strongCount--;
        freeElmIfDead(*index);

        *index = static_cast<size_t>(-1);
    }

    void removeWeakRef(size_t* index)
    {
        if (*index == static_cast<size_t>(-1))
            return;

        std::lock_guard lck(m_DBAccess);

        RDB_Elm& elm = m_DB.at(*index);

        elm.weakCount--;
        freeElmIfDead(*index);

        *index = static_cast<size_t>(-1);
    }

    GObjHNDL get(size_t index)
    {
        if (index == static_cast<size_t>(-1))
            return nullptr;

        std::lock_guard lck(m_DBAccess);
        return m_DB[index].hndl;
    }

private:
    std::recursive_mutex m_DBAccess;
    std::vector<RDB_Elm> m_DB;
    std::stack<size_t> m_freeElm;

    void freeElmIfDead(size_t index)
    {
        if (m_DB.at(index).strongCount == 0 && m_DB.at(index).weakCount == 0)
        {
            m_freeElm.push(index);
        }
    }
};

static RefDatabase g_RefDatabase;

OwningRef Galvanizer::CreateOwningRef(GObjHNDL hndl)
{
    OwningRef obj;
    obj.m_DBIndex = g_RefDatabase.CreateStrongRef(hndl);
    std::cout << "Created owningRef: " << obj.m_DBIndex << " for " << hndl << std::endl;

    return obj;
}


OwningRef::OwningRef(size_t index)
{
    m_DBIndex = index;

    g_RefDatabase.addStrongRef(m_DBIndex);
    m_hndl = g_RefDatabase.get(m_DBIndex);
}

OwningRef::OwningRef(OwningRef const& right)
{
    m_DBIndex = right.m_DBIndex;

    g_RefDatabase.addStrongRef(right.m_DBIndex);
    m_hndl = g_RefDatabase.get(m_DBIndex);
}

OwningRef::OwningRef(OwningRef&& right) noexcept
{
    m_DBIndex = right.m_DBIndex;
    right.m_DBIndex = static_cast<size_t>(-1);

    m_hndl = g_RefDatabase.get(m_DBIndex);
}

OwningRef& OwningRef::operator=(OwningRef const& right)
{
    m_DBIndex = right.m_DBIndex;

    g_RefDatabase.addStrongRef(right.m_DBIndex);
    m_hndl = g_RefDatabase.get(m_DBIndex);

    return *this;
}

OwningRef& OwningRef::operator=(OwningRef&& right) noexcept
{
    m_DBIndex = right.m_DBIndex;
    right.m_DBIndex = static_cast<size_t>(-1);

    m_hndl = g_RefDatabase.get(m_DBIndex);

    return *this;
}


OwningRef::~OwningRef()
{
    g_RefDatabase.removeStrongRef(&m_DBIndex, true);
}

GObjHNDL OwningRef::get() const
{
    return g_RefDatabase.get(m_DBIndex);
}


void OwningRef::DropOwnership(bool deleteObj)
{
    g_RefDatabase.removeStrongRef(&m_DBIndex, deleteObj);

    m_DBIndex = static_cast<size_t>(-1);
    m_hndl = nullptr;
}


GObjHNDL OwningRef::operator->() const { return get(); }
OwningRef::operator GObjHNDL() const { return get(); }
OwningRef::operator WeakRef() const { return WeakRef(m_DBIndex); }



WeakRef::WeakRef(size_t index)
{
    m_DBIndex = index;

    g_RefDatabase.addWeakRef(m_DBIndex);
    m_hndl = g_RefDatabase.get(m_DBIndex);
}

WeakRef::WeakRef(WeakRef const& right)
{
    m_DBIndex = right.m_DBIndex;

    g_RefDatabase.addWeakRef(m_DBIndex);
    m_hndl = g_RefDatabase.get(m_DBIndex);
}

WeakRef::WeakRef(WeakRef&& right) noexcept
{
    m_DBIndex = right.m_DBIndex;
    right.m_DBIndex = static_cast<size_t>(-1);

    m_hndl = g_RefDatabase.get(m_DBIndex);
}

WeakRef& WeakRef::operator=(WeakRef const& right)
{
    m_DBIndex = right.m_DBIndex;

    g_RefDatabase.addWeakRef(m_DBIndex);
    m_hndl = g_RefDatabase.get(m_DBIndex);

    return *this;
}

WeakRef& WeakRef::operator=(WeakRef&& right) noexcept
{
    m_DBIndex = right.m_DBIndex;
    right.m_DBIndex = static_cast<size_t>(-1);

    m_hndl = g_RefDatabase.get(m_DBIndex);

    return *this;
}

WeakRef::~WeakRef()
{
    g_RefDatabase.removeWeakRef(&m_DBIndex);
}

OwningRef WeakRef::lock() const
{
    return OwningRef(m_DBIndex);
}

void WeakRef::DropOwnership()
{
    g_RefDatabase.removeWeakRef(&m_DBIndex);

    m_DBIndex = static_cast<size_t>(-1);
    m_hndl = nullptr;
}
