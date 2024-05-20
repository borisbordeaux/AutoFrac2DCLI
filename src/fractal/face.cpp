#include "fractal/face.h"
#include "utils/utils.h"
#include "fractal/algorithms/algorithmsurrounddelayandbezier.h"
#include "fractal/algorithms/algorithmsurrounddelay.h"
#include "fractal/algorithms/algorithmoncorners.h"

#include <iostream>

frac::Set<frac::Face> frac::Face::s_existingFaces;
std::map<std::string, std::string> frac::Face::s_incidenceConstraints;
std::map<std::string, std::string> frac::Face::s_adjacencyConstraints;
std::unordered_map<std::string, std::vector<frac::Face>> frac::Face::s_subdivisions;

frac::Face::Face(std::vector<Edge> edges, unsigned int delay, frac::Edge const& adjEdge, frac::Edge const& gapEdge, frac::Edge const& reqEdge, AlgorithmSubdivision algo) :
        m_data(std::move(edges)), m_delay(delay), m_adjEdge(adjEdge), m_gapEdge(gapEdge), m_reqEdge(reqEdge), m_offset(0), m_firstInterior(-1), m_algo(algo) {
    for (Face const& f: s_existingFaces.data()) {
        if (*this == f) {
            m_name = f.m_name;
            m_offset = computeOffset(f, *this);
        }
    }
    //if face doesn't exist
    if (m_name.empty()) {
        m_name = "Cell_" + std::to_string(s_existingFaces.size());
        if (m_delay != 0) {
            // one face if delayed, with subdivided boundaries
            // add delay info to name
            m_name += "_" + std::to_string(delay);
        }
        s_existingFaces.add(*this);
    }
}

std::vector<frac::Edge> const& frac::Face::constData() const {
    return m_data;
}

std::vector<frac::Edge>& frac::Face::data() {
    return m_data;
}


int frac::Face::firstInterior() const {
    return m_firstInterior;
}

int frac::Face::lastInterior() const {
    return m_firstInterior + 2;
}

std::size_t frac::Face::len() const {
    return m_data.size();
}

std::string frac::Face::name() const {
    return m_name;
}

int frac::Face::offset() const {
    return m_offset;
}

void frac::Face::setFirstInterior(int index) {
    m_firstInterior = index;
}

void frac::Face::setAlgo(frac::AlgorithmSubdivision algo) {
    m_algo = algo;
}

std::vector<frac::Face> frac::Face::subdivisions() const {
    if (s_subdivisions.find(m_name) != s_subdivisions.end()) {
        return s_subdivisions[m_name];
    }
    switch (m_algo) {
        case AlgorithmSubdivision::LinksSurroundDelay:
            return frac::LinksSurroundDelay::subdivide(*this);
        case AlgorithmSubdivision::LinksSurroundDelayAndBezier:
            return frac::LinksSurroundDelayAndBezier::subdivide(*this);
        case AlgorithmSubdivision::LinksOnCorners:
            return frac::LinksOnCorners::subdivide(*this);
    }
    return {};
}

const frac::Edge& frac::Face::operator[](std::size_t index) const {
    return m_data.at(index);
}

bool frac::Face::operator==(frac::Face const& other) const {
    if (m_delay != other.m_delay || this->len() != other.len() || m_adjEdge != other.m_adjEdge || m_gapEdge != other.m_gapEdge || m_reqEdge != other.m_reqEdge || m_algo != other.m_algo) {
        return false;
    }

    std::vector<frac::Edge> shifted { other.m_data };

    for (std::size_t i = 0; i < this->len(); ++i) {
        if (m_data == shifted) {
            return true;
        }
        std::vector<frac::Edge> new_shifted { frac::utils::shiftVector(shifted) };
        shifted.clear();
        for (Edge const& e: new_shifted) {
            shifted.emplace_back(e);
        }
    }
    return false;
}

namespace frac {
std::ostream& operator<<(std::ostream& os, frac::Face const& face) {
    return os << face.toString();
}
}

void frac::Face::addAdjacencyConstraint(frac::Face const& face, frac::Face const& faceSub1, frac::Face const& faceSub2, unsigned int indexSubFace1, unsigned int indexBordFace1, unsigned int indexSubFace2, unsigned int indexBordFace2) {
    if (s_adjacencyConstraints.find(face.name()) == std::end(Face::s_adjacencyConstraints)) {
        s_adjacencyConstraints[face.name()] = "";
    }
    int s1 = static_cast<int>(indexSubFace1);
    int b1 = frac::utils::mod(static_cast<int>(indexBordFace1) - faceSub1.offset(), static_cast<int>(faceSub1.len()));
    int s2 = static_cast<int>(indexSubFace2);
    int b2 = frac::utils::mod(static_cast<int>(indexBordFace2) - faceSub2.offset(), static_cast<int>(faceSub2.len()));
    s_adjacencyConstraints[face.name()] += "    " + face.name() + "(Sub('" + std::to_string(s1) + "') + Bord('" + std::to_string(b1) + "') + Permut('0'), Sub('" + std::to_string(s2) + "') + Bord('" + std::to_string(b2) + "'))\n";
}

void frac::Face::addIncidenceConstraint(frac::Face const& face, frac::Face const& faceSub, unsigned int indexParentEdge, unsigned int indexSubEdge, unsigned int indexSubFaceEdge, unsigned int indexSubFace) {
    if (s_incidenceConstraints.find(face.name()) == std::end(Face::s_incidenceConstraints)) {
        s_incidenceConstraints[face.name()] = "";
    }
    int b1 = frac::utils::mod(static_cast<int>(indexParentEdge) - face.offset(), static_cast<int>(face.len()));
    int s1 = static_cast<int>(indexSubEdge);
    int s2 = static_cast<int>(indexSubFace);
    int b2 = frac::utils::mod(static_cast<int>(indexSubFaceEdge) - faceSub.offset(), static_cast<int>(faceSub.len()));
    s_incidenceConstraints[face.name()] += "    " + face.name() + "(Bord('" + std::to_string(b1) + "') + Sub('" + std::to_string(s1) + "'), Sub('" + std::to_string(s2) + "') + Bord('" + std::to_string(b2) + "'))\n";
}

int frac::Face::computeOffset(frac::Face const& face, frac::Face const& other) {
    if (face.m_delay != other.m_delay || face.len() != other.len() || face.m_adjEdge != other.m_adjEdge || face.m_gapEdge != other.m_gapEdge || face.m_reqEdge != other.m_reqEdge) {
        return -1;
    }
    std::vector<Edge> shifted { other.m_data };
    for (std::size_t i = 0; i < other.len(); ++i) {
        if (face.m_data == shifted) {
            return static_cast<int>(i);
        }
        std::vector<frac::Edge> new_shifted { frac::utils::shiftVector(shifted) };
        shifted.clear();
        for (Edge const& e: new_shifted) {
            shifted.emplace_back(e);
        }
    }
    return -1;
}

frac::Set<frac::Face> frac::Face::allSubdivisions() const {
    frac::Set<frac::Face> res;
    res.add(*this);
    std::size_t i = 0;
    bool changed = true;
    while (changed) {
        frac::Set<frac::Face> added;
        for (std::size_t j = i; j < res.size(); ++j) {
            std::vector<frac::Face> subs = res[j].subdivisions();
            for (frac::Face const& f: subs) {
                added.add(f);
            }
        }
        std::size_t lastSize = res.size();
        for (frac::Face const& f: added.data()) {
            res.add(f);
        }
        changed = res.size() != lastSize;
        i = lastSize;
    }
    return res;
}

std::string frac::Face::toString() const {
    std::string res = (*this)[0].toString();
    for (std::size_t i = 1; i < this->len(); ++i) {
        res += " - " + (*this)[i].toString();
    }
    res += " / ";

    res += m_adjEdge.toString() + " - ";
    res += m_gapEdge.toString() + " - ";
    res += m_reqEdge.toString() + " / ";
    res += std::to_string(m_delay) + " / ";
    res += std::to_string(static_cast<int>(m_algo));
    return res;
}


frac::Edge frac::Face::adjEdge() const {
    return m_adjEdge;
}

frac::Edge frac::Face::gapEdge() const {
    return m_gapEdge;
}

frac::Edge frac::Face::reqEdge() const {
    return m_reqEdge;
}

void frac::Face::reset() {
    Face::s_incidenceConstraints.clear();
    Face::s_adjacencyConstraints.clear();
    Face::s_existingFaces.clear();
    Face::s_subdivisions.clear();
}

void frac::Face::setAdjEdge(frac::Edge const& edge) {
    m_adjEdge = edge;
}

void frac::Face::setGapEdge(frac::Edge const& edge) {
    m_gapEdge = edge;
}

void frac::Face::setReqEdge(frac::Edge const& edge) {
    m_reqEdge = edge;
}

void frac::Face::setDelay(unsigned int delay) {
    m_delay = delay;
}

unsigned int frac::Face::delay() const {
    return m_delay;
}

frac::AlgorithmSubdivision frac::Face::algo() const {
    return m_algo;
}

std::optional<frac::Edge> frac::Face::edgeIfRequired(const frac::Edge& edge) const {
    if (edge.edgeType() == frac::EdgeType::CANTOR) {
        return m_reqEdge;
    } else {
        return {};
    }
}

std::size_t frac::Face::nbControlPoints(bool bezierCubic) const {
    std::size_t res = 0;
    for (Edge const& e: m_data) {
        res += e.edgeType() == EdgeType::BEZIER ? (bezierCubic ? 4 : 3) : 2;
        res--;
    }
    return res;
}

frac::Face frac::Face::fromStr(std::string const& name) {
    std::string sepCellInfo = " / ";
    std::string sepEdges = " - ";

    std::vector<std::string> splitCellName = frac::utils::split(name, sepCellInfo);
    std::string edgesNames = splitCellName[0];
    std::string paramsNames = splitCellName[1];
    unsigned int delay = std::stoul(splitCellName[2]);

    std::vector<frac::Edge> edges;
    for (std::string const& edgeName: frac::utils::split(edgesNames, sepEdges)) {
        edges.emplace_back(frac::Edge::fromStr(edgeName));
    }
    std::vector<std::string> splitParamsNames = frac::utils::split(paramsNames, sepEdges);

    frac::Edge adjEdge = frac::Edge::fromStr(splitParamsNames[0]);
    frac::Edge gapEdge = frac::Edge::fromStr(splitParamsNames[1]);
    frac::Edge reqEdge = frac::Edge::fromStr(splitParamsNames[2]);

    frac::AlgorithmSubdivision algo = static_cast<frac::AlgorithmSubdivision>(std::stoul(splitCellName[3]));
    return frac::Face(edges, delay, adjEdge, gapEdge, reqEdge, algo);
}
