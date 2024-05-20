#ifndef AUTOFRAC_SET_H
#define AUTOFRAC_SET_H

#include <algorithm>
#include <vector>

namespace frac {

template<typename T>
class Set {
public:
    void add(T const& elt) {
        if (std::find(this->m_data.begin(), this->m_data.end(), elt) == this->m_data.end()) {
            this->m_data.emplace_back(elt);
        }
    }

    std::size_t size() const {
        return this->m_data.size();
    }

    T const& operator[](std::size_t index) const {
        return this->m_data[index];
    }

    std::vector<T> const& data() const {
        return this->m_data;
    }

    void clear() {
        this->m_data.clear();
    }

    auto begin() { return m_data.begin(); }

    auto end() { return m_data.end(); }

private:
    std::vector<T> m_data;
};

} // frac

#endif //AUTOFRAC_SET_H
