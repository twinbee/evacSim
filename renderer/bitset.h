/**********************************************************
** bitset.h
** Primary Author: Mike J
** Descripotion: This is our bitset class for storing which face has already been drawn.
** The bitset functionality isn't really taken advantage of in this version
** since we aren't rendering by leafs and nodes.
** Used in sim:
*********************************************************/

#ifndef BITSET_H
#define BITSET_H

class CBitset
{

public:

    // Initialize all the data members
    CBitset() : m_bits(0), m_size(0) {}

    // This is our deconstructor
    ~CBitset()
    {
        // If we have valid memory, get rid of it
        if (m_bits)
        {
            delete m_bits;
            m_bits = 0;
        }
    }

    // This resizes our bitset to a size so each face has a bit associated with it
    void Resize(int count)
    {
        // Get the size of integers we need
        m_size = count/32 + 1;

        // Make sure we haven't already allocated memory for the bits
        if (m_bits)
        {
            delete m_bits;
            m_bits = 0;
        }

        // Allocate the bits and initialize them
        m_bits = new unsigned int[m_size];
        ClearAll();
    }

    // This does the binary math to set the desired bit
    void Set(int i)
    {
        m_bits[i >> 5] |= (1 << (i & 31));
    }

    // This returns if the desired bit slot is a 1 or a 0
    int On(int i)
    {
        return m_bits[i >> 5] & (1 << (i & 31 ));
    }

    // This clears a bit to 0
    void Clear(int i)
    {
        m_bits[i >> 5] &= ~(1 << (i & 31));
    }

    // This initializes the bits to 0
    void ClearAll()
    {
        memset(m_bits, 0, sizeof(unsigned int) * m_size);
    }

private:

    // Our private bit data that holds the bits and size
    unsigned int *m_bits;
    int m_size;
};

#endif
