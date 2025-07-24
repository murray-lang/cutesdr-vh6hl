#ifndef __VECTORASSEMBLER_H__
#define __VECTORASSEMBLER_H__

#include <stdint.h>
#include "../../SampleTypes.h"

template<typename SampleType>
class VectorAssembler {
public:
    using Vector = std::vector<SampleType>;
    using VectorVector = std::vector<Vector>;

    VectorAssembler(uint32_t requiredLength) :
        m_requiredLength(requiredLength),
        m_pCurrentVector(nullptr)
    {

    }
    virtual ~VectorAssembler() {
        clear();
    }

    VectorAssembler& clear() {
        delete m_pCurrentVector;
        m_pCurrentVector = nullptr;
        for (auto p : m_availableVectors)
        {
            delete p;
        }
        m_availableVectors.clear();
    }

    virtual VectorAssembler& add(SampleType sample)
    {
        if(m_pCurrentVector == nullptr) {
            m_pCurrentVector = new Vector();
        }
        m_pCurrentVector->push_back(sample);
        if (m_pCurrentVector->size() == m_requiredLength) {
            m_availableVectors.push_back(m_pCurrentVector);
            m_pCurrentVector = nullptr;
        }
        return *this;
    }

    virtual VectorAssembler& add(const Vector& samples)
    {
        vector::iterator iter = samples.begin();
        for(iter; iter < samples.end(); iter++) {
            add(*iter);
        }
        return *this;
    }

    //const Vector* getCurrentVector() const { return m_pCurrentVector; }
    VectorVector& getAvailableVectors() { return m_availableVectors; }
    bool areVectorsAvailable() const { return m_availableVectors.size() > 0; }

protected:
    uint32_t m_requiredLength;
    Vector* m_pCurrentVector;
    VectorVector m_availableVectors;

}

#endif //__VECTORASSEMBLER_H__
