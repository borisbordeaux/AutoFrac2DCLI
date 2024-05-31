#include "fractal/structure.h"
#include "utils/utils.h"
#include <iostream>

frac::Structure::Structure(std::vector<Face> const& faces, bool bezierCubic) : m_faces(faces), m_bezierCubic(bezierCubic) {}

void frac::Structure::addAdjacency(Adjacency const& adj) {
    if (m_faces[adj.Face1][adj.Edge1] == m_faces[adj.Face2][adj.Edge2]) {
        std::size_t offset1 = m_faces[adj.Face1].offset();
        std::size_t offset2 = m_faces[adj.Face2].offset();
        std::size_t edge1 = static_cast<std::size_t>(frac::utils::mod(static_cast<int>(adj.Edge1) - static_cast<int>(offset1), static_cast<int>(m_faces[adj.Face1].len())));
        std::size_t edge2 = static_cast<std::size_t>(frac::utils::mod(static_cast<int>(adj.Edge2) - static_cast<int>(offset2), static_cast<int>(m_faces[adj.Face2].len())));
        m_strAdjacency += "    init(Sub('" + std::to_string(adj.Face1) + "') + Bord('" + std::to_string(edge1) + "') + Permut('0'), Sub('" + std::to_string(adj.Face2) + "') + Bord('" + std::to_string(edge2) + "'))\n";
        m_adjacencies.push_back(adj);
    }
}

frac::Set<frac::Edge> frac::Structure::allEdges() const {
    frac::Set<frac::Edge> res;
    frac::Set<frac::Face> faces = this->allFaces();
    for (frac::Face const& f: faces.data()) {
        for (frac::Edge const& e: f.constData()) {
            res.add(e);
        }
    }
    return res;
}

frac::Set<frac::Face> frac::Structure::allFaces() const {
    frac::Set<frac::Face> res;
    for (frac::Face const& f: this->m_faces) {
        res.add(f);
    }
    for (frac::Face const& f: this->m_faces) {
        frac::Set<frac::Face> subdivisions = f.allSubdivisions();
        for (frac::Face const& sub: subdivisions.data()) {
            res.add(sub);
        }
    }
    return res;
}

std::size_t frac::Structure::nbControlPointsOfFace(std::size_t indexFace) const {
    return m_faces[indexFace].nbControlPoints(m_bezierCubic);
}

namespace frac {
std::ostream& operator<<(std::ostream& os, frac::Structure const& structure) {
    for (frac::Face const& f: structure.m_faces) {
        os << f << std::endl;
    }
    os << structure.m_strAdjacency;
    return os;
}
}

frac::Adjacency frac::Adjacency::fromStr(std::string const& strConstraint) {
    std::string sepFaces = " / ";
    std::string sepFaceInfo = ".";

    std::vector<std::string> splitFaces = frac::utils::split(strConstraint, sepFaces);
    std::string face1Info = splitFaces[0];
    std::string face2Info = splitFaces[1];

    std::vector<std::string> splitFace1 = frac::utils::split(face1Info, sepFaceInfo);
    std::vector<std::string> splitFace2 = frac::utils::split(face2Info, sepFaceInfo);

    std::size_t face1 = std::stoul(splitFace1[0]);
    std::size_t edge1 = std::stoul(splitFace1[1]);
    std::size_t face2 = std::stoul(splitFace2[0]);
    std::size_t edge2 = std::stoul(splitFace2[1]);
    return { face1, edge1, face2, edge2 };
}

frac::Face const& frac::Structure::operator[](std::size_t index) const {
    return m_faces[index];
}

std::string frac::Structure::strAdjacencies() const {
    return this->m_strAdjacency;
}

const std::vector<frac::Face>& frac::Structure::faces() const {
    return m_faces;
}

std::vector<frac::Adjacency> const& frac::Structure::adjacencies() const {
    return m_adjacencies;
}

std::vector<std::size_t> frac::Structure::controlPointIndices(std::size_t indexEdge, std::size_t indexFace, bool reverse) const {
    std::vector<std::size_t> res = {};
    std::size_t current = 0;
    for (std::size_t i = 0; i < indexEdge; i++) {
        current += m_faces[indexFace][i].edgeType() == EdgeType::BEZIER ? (m_bezierCubic ? 3 : 2) : 1;
    }
    res.emplace_back(current);
    res.emplace_back((current + 1) % this->nbControlPointsOfFace(indexFace));

    if (m_faces[indexFace][indexEdge].edgeType() == EdgeType::BEZIER) {
        res.emplace_back((current + 2) % this->nbControlPointsOfFace(indexFace));
        if (m_bezierCubic) {
            res.emplace_back((current + 3) % this->nbControlPointsOfFace(indexFace));
        }
    }

    if (reverse) {
        //reverse extremities
        std::size_t temp = res[0];
        res[0] = res[res.size() - 1];
        res[res.size() - 1] = temp;
        if (m_bezierCubic && m_faces[indexFace][indexEdge].edgeType() == EdgeType::BEZIER) {
            //reverse the 2 points in the middle
            temp = res[1];
            res[1] = res[2];
            res[2] = temp;
        }
    }

    return res;
}

bool frac::Structure::isInternControlPoint(std::size_t indexControlPoint, std::size_t indexFace) const {
    bool res = indexControlPoint != 0;
    std::size_t current = 0;
    for (std::size_t i = 0; i < m_faces[indexFace].constData().size(); i++) {
        current += m_faces[indexFace][i].edgeType() == EdgeType::BEZIER ? (m_bezierCubic ? 3 : 2) : 1;
        if (current == indexControlPoint) {
            res = false;
        }
    }
    return res;
}

bool frac::Structure::isControlPointBelongEdge(std::size_t indexControlPoint, std::size_t indexFace, std::size_t indexEdge) const {
    bool res = false;
    std::size_t current = 0;
    for (std::size_t i = 0; i < m_faces[indexFace].constData().size(); i++) {
        if (i == indexEdge) {
            if (current == indexControlPoint || current + 1 == indexControlPoint) {
                res = true;
            }
            if (m_faces[indexFace][i].edgeType() == EdgeType::BEZIER) {
                if (m_bezierCubic) {
                    if (current + 2 == indexControlPoint || current + 3 == indexControlPoint) {
                        res = true;
                    }
                } else {
                    if (current + 2 == indexControlPoint) {
                        res = true;
                    }
                }
            }
        }
        current += m_faces[indexFace][i].edgeType() == EdgeType::BEZIER ? (m_bezierCubic ? 3 : 2) : 1;
    }
    return res;
}

bool frac::Structure::isBezierCubic() const {
    return m_bezierCubic;
}
