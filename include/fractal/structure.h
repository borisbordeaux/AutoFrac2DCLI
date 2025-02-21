#ifndef AUTOFRAC_FRAC_STRUCTURE_H
#define AUTOFRAC_FRAC_STRUCTURE_H

#include <ostream>
#include <string>
#include <vector>

#include "fractal/face.h"
#include "utils/set.h"

namespace frac {

struct Adjacency {
    std::size_t Face1;
    std::size_t Edge1;
    std::size_t Face2;
    std::size_t Edge2;

    Adjacency(std::size_t face1, std::size_t edge1, std::size_t face2, std::size_t edge2) :
            Face1(face1), Edge1(edge1), Face2(face2), Edge2(edge2) {}

    bool equals(Adjacency const& other) const {
        return Face1 == other.Face1 && Edge1 == other.Edge1 && Face2 == other.Face2 && Edge2 == other.Edge2;
    }

    static Adjacency fromStr(std::string const& strConstraint);
};

class Structure {
public:
    explicit Structure(std::vector<frac::Face> const& faces, frac::BezierType bezierType, frac::CantorType cantorType);

    void addAdjacency(Adjacency const& adj);
    std::string strAdjacencies() const;
    std::vector<Adjacency> const& adjacencies() const;
    frac::Set<frac::Edge> allEdges() const;
    frac::Set<frac::Face> allFaces() const;

    std::vector<frac::Face> const& faces() const;

    std::size_t nbControlPointsOfFace(std::size_t indexFace) const;
    std::vector<std::size_t> controlPointIndices(std::size_t indexEdge, std::size_t indexFace, bool reverse = false) const;

    bool isInternControlPoint(std::size_t indexControlPoint, std::size_t indexFace) const;
    bool isControlPointBelongEdge(std::size_t indexControlPoint, std::size_t indexFace, std::size_t indexEdge) const;

    friend std::ostream& operator<<(std::ostream& os, const frac::Structure& structure);
    frac::Face const& operator[](std::size_t index) const;

    frac::BezierType bezierType() const;
    frac::CantorType cantorType() const;

private:
    std::vector<frac::Face> m_faces;
    std::string m_strAdjacency;
    std::vector<Adjacency> m_adjacencies;
    frac::BezierType m_bezierType;
    frac::CantorType m_cantorType;
};

} // frac

#endif //AUTOFRAC_FRAC_STRUCTURE_H
