#pragma once
#include <vector>
#include "logger.hpp"

// TODO(theblek): Overload [] operator to get data for even slicker syntax
template<typename T, int MAX_SIZE>
class PackedArray {
 public:
    using Index = int;

    std::vector<T> entries;

    PackedArray() : m_EntryCount(0) {
        entries = std::vector<T>(MAX_SIZE);
        m_EntryToIndex = std::vector<Index>(MAX_SIZE);
        m_IndexToEntry = std::vector<Index>(MAX_SIZE);
        for (unsigned int i = 0; i < MAX_SIZE; i++) {
            m_EntryToIndex[i] = i;
            m_IndexToEntry[i] = i;
        }
    }

    bool HasData(Index entry) {
        if (entry < 0 || entry >= MAX_SIZE) {
            Logger::Error("Invalid entry access at pos: %d", entry);
            return false;
        }
        return m_EntryToIndex[entry] < m_EntryCount;
    }

    void RemoveData(Index entry) {
        if (!HasData(entry)) {
            Logger::Error("Entry %d does not have valid data to remove", entry);
            return;
        }

        // If not the last one
        if (m_EntryToIndex[entry] < m_EntryCount - 1) {
            // Move last one to deleted position
            Index removedIndex = m_EntryToIndex[entry];
            Swap(entry, m_EntryCount - 1);
            entries[removedIndex] = entries[m_EntryCount - 1];
        }

        m_EntryCount--;
    }

    T &GetData(Index entry) {
        if (!HasData(entry))
            Logger::Error("Entry %d does not have valid data", entry);

        return entries[m_EntryToIndex[entry]];
    }

    void SetData(Index entry, const T &data) {
        if (entry < 0 || entry >= MAX_SIZE) {
            Logger::Error("Invalid entry access at pos: %d", entry);
            return;
        }
        // If entry is new
        if (m_EntryToIndex[entry] >= m_EntryCount) {
            Swap(entry, m_EntryCount);
            m_EntryCount++;
        }

        entries[m_EntryToIndex[entry]] = data;
    }

    // BEWARE RETURED INDEX IS INTERNAL
    // AND THEREFORE SHOULD ONLY BE USED TO ITERATE OVER INTERNAL ARRAY
    int GetSize() const {
        return m_EntryCount;
    }

    // This should only be used in conjection with iteration over
    // the underlying array `entries`
    Index GetFromInternal(int index) {
        return m_IndexToEntry[index];
    }

    Index GetEmptyEntry() const {
        return m_IndexToEntry[m_EntryCount];
    }

    Index AddData(const T &data) {
        if (m_EntryCount == MAX_SIZE) {
            Logger::Error("Adding entry into full array with size: %d", m_EntryCount);
            return -1;
        }
        Index place = GetEmptyEntry();
        SetData(place, data);
        return place;
    }

    T *begin() {
        return &entries[0];
    }

    T *end() {
        return &entries[m_EntryCount];
    }

    const T *begin() const {
        return &entries[0];
    }

    const T *end() const {
        return &entries[m_EntryCount];
    }


 private:
    // BEWARE: aEntry is a public index, while bIndex in a private one.
    void Swap(Index aEntry, Index bIndex) {
        Index bEntry = m_IndexToEntry[bIndex];
        Index aIndex = m_EntryToIndex[aEntry];
        m_EntryToIndex[aEntry] = bIndex;
        m_EntryToIndex[bEntry] = aIndex;
        m_IndexToEntry[aIndex] = bEntry;
        m_IndexToEntry[bIndex] = aEntry;
    }

    Index m_EntryCount;
    std::vector<Index> m_EntryToIndex;
    std::vector<Index> m_IndexToEntry;
};
