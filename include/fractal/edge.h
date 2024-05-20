#ifndef EDGE_H
#define EDGE_H

#include <ostream>
#include <string>
#include <vector>

namespace frac {

enum class EdgeType {
    CANTOR, BEZIER
};

class Edge {
public:
    Edge(frac::Edge const& other) = default;
    Edge(frac::EdgeType edgeType, unsigned int nbSubdivisions, unsigned int delay = 0);
    Edge& operator=(const frac::Edge& other) = default;
    static Edge fromStr(std::string const& name);

    void decreaseDelay();
    [[nodiscard]] frac::EdgeType edgeType() const;
    [[nodiscard]] unsigned int nbSubdivisions() const;
    [[nodiscard]] unsigned int nbActualSubdivisions() const;
    [[nodiscard]] unsigned int delay() const;
    [[nodiscard]] std::vector<frac::Edge> subdivisions(frac::Edge const& reqEdge) const;
    [[nodiscard]] bool isDelay() const;
    [[nodiscard]] std::string name() const;
    void setEdgeType(frac::EdgeType edgeType);
    void setNbSubdivisions(unsigned int nbSubdivisions);
    void setDelay(unsigned int delay);

    friend std::ostream& operator<<(std::ostream& os, frac::Edge const& edge);
    bool operator==(frac::Edge const& other) const;
    bool operator!=(frac::Edge const& other) const;

    [[nodiscard]] std::string toString() const;

private:
    frac::EdgeType m_edgeType;
    unsigned int m_nbSubdivisions;
    unsigned int m_delay;
};

} // frac

#endif // EDGE_H


