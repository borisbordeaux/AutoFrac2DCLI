#include "fractal/structure.h"
#include "utils/utils.h"
#include <iostream>

frac::Structure::Structure(std::vector<Face> const& faces, BezierType bezierType, CantorType cantorType) : m_faces(faces), m_bezierType(bezierType), m_cantorType(cantorType) {}

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
    return m_faces[indexFace].nbControlPoints(m_bezierType, m_cantorType);
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
    std::string const& face1Info = splitFaces[0];
    std::string const& face2Info = splitFaces[1];

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
        //add to current the number of control points on the edge minus one
        current += m_faces[indexFace][i].nbControlPoints(m_bezierType, m_cantorType) - 1;
    }
    res.emplace_back(current); //first control point index

    std::size_t nbInternCtrlPts = m_faces[indexFace][indexEdge].nbInternControlPoints(m_bezierType, m_cantorType);
    for (std::size_t i = 1; i <= nbInternCtrlPts; i++) {
        res.emplace_back((current + i) % this->nbControlPointsOfFace(indexFace)); //intern control points indices
    }

    res.emplace_back((current + nbInternCtrlPts + 1) % this->nbControlPointsOfFace(indexFace)); //last control point index

    if (reverse) {
        std::reverse(res.begin(), res.end());
    }

    return res;
}

bool frac::Structure::isInternControlPoint(std::size_t indexControlPoint, std::size_t indexFace) const {
    bool res = indexControlPoint != 0;
    std::size_t current = 0;
    for (std::size_t i = 0; i < m_faces[indexFace].constData().size(); i++) {
        current += m_faces[indexFace][i].nbControlPoints(m_bezierType, m_cantorType) - 1;
        if (current == indexControlPoint) {
            res = false;
        }
    }
    return res;
}

bool frac::Structure::isControlPointBelongEdge(std::size_t indexControlPoint, std::size_t indexFace, std::size_t indexEdge) const {
    std::vector<std::size_t> indices = this->controlPointIndices(indexEdge, indexFace);
    return std::find(indices.begin(), indices.end(), indexControlPoint) != indices.end();
}

frac::BezierType frac::Structure::bezierType() const {
    return m_bezierType;
}

frac::CantorType frac::Structure::cantorType() const {
    return m_cantorType;
}
