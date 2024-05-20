#include "fractal/edge.h"
#include "utils/utils.h"

frac::Edge::Edge(EdgeType edgeType, unsigned int nbSubdivisions, unsigned int delay) : m_edgeType(edgeType), m_nbSubdivisions(nbSubdivisions), m_delay(delay) {}

frac::Edge frac::Edge::fromStr(const std::string& name) {
    std::vector<std::string> splitEdgeName = frac::utils::split(name, '_');

    frac::EdgeType type = splitEdgeName[0] == "C" ? frac::EdgeType::CANTOR : frac::EdgeType::BEZIER;
    unsigned int nbSubs = std::stoul(splitEdgeName[1]);
    unsigned int delayEdge = std::stoul(splitEdgeName[2]);

    return {type, nbSubs, delayEdge};
}

void frac::Edge::decreaseDelay() {
    if (this->isDelay()) {
        this->m_delay--;
    }
}

frac::EdgeType frac::Edge::edgeType() const {
    return this->m_edgeType;
}

unsigned int frac::Edge::nbActualSubdivisions() const {
    return this->isDelay() ? 1 : this->m_nbSubdivisions;
}

unsigned int frac::Edge::nbSubdivisions() const {
    return this->m_nbSubdivisions;
}

unsigned int frac::Edge::delay() const {
    return this->m_delay;
}

std::vector<frac::Edge> frac::Edge::subdivisions(Edge const& reqEdge) const {
    std::vector<Edge> result;
    if (this->isDelay()) {
        result.emplace_back(this->edgeType(), this->nbSubdivisions(), this->delay() - 1);
    } else {
        if (this->edgeType() == EdgeType::CANTOR) {
            for (unsigned int i = 0; i < this->nbSubdivisions() - 1; ++i) {
                result.emplace_back(EdgeType::CANTOR, this->nbSubdivisions());
                result.emplace_back(reqEdge);
                result.emplace_back(reqEdge);
            }
            result.emplace_back(EdgeType::CANTOR, this->nbSubdivisions());
        }
        if (this->edgeType() == EdgeType::BEZIER) {
            for (unsigned int i = 0; i < this->nbSubdivisions(); ++i) {
                result.emplace_back(EdgeType::BEZIER, this->nbSubdivisions());
            }
        }
    }
    return result;
}

bool frac::Edge::isDelay() const {
    return this->m_delay > 0;
}

namespace frac {
std::ostream& operator<<(std::ostream& os, frac::Edge const& edge) {
    std::string edgeType = edge.edgeType() == frac::EdgeType::BEZIER ? "B" : "C";
    std::string nbSub = std::to_string(edge.nbSubdivisions());
    std::string delay = edge.isDelay() ? "_" + std::to_string(edge.delay()) : "";

    return os << edgeType << nbSub << delay;
}
}

bool frac::Edge::operator==(Edge const& other) const {
    return this->m_edgeType == other.m_edgeType && this->m_delay == other.m_delay && this->m_nbSubdivisions == other.m_nbSubdivisions;
}

bool frac::Edge::operator!=(Edge const& other) const {
    return !(*this == other);
}

std::string frac::Edge::toString() const {
    std::string res;

    std::string edgeType = this->edgeType() == frac::EdgeType::BEZIER ? "B_" : "C_";
    std::string nbSub = std::to_string(this->nbSubdivisions());
    std::string delay = "_" + std::to_string(this->delay());

    return edgeType + nbSub + delay;
}

std::string frac::Edge::name() const {
    std::string res;

    std::string edgeType = this->edgeType() == frac::EdgeType::BEZIER ? "B" : "C";
    std::string nbSub = std::to_string(this->nbSubdivisions());
    std::string delay = this->isDelay() ? "_" + std::to_string(this->delay()) : "";

    return edgeType + nbSub + delay;
}

void frac::Edge::setEdgeType(frac::EdgeType edgeType) {
    this->m_edgeType = edgeType;
}

void frac::Edge::setNbSubdivisions(unsigned int nbSubdivisions) {
    this->m_nbSubdivisions = nbSubdivisions;
}

void frac::Edge::setDelay(unsigned int delay) {
    this->m_delay = delay;
}
