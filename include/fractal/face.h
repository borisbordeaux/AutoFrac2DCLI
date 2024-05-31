#ifndef AUTOFRAC_FACE_H
#define AUTOFRAC_FACE_H

#include <map>
#include <optional>
#include <vector>
#include <unordered_map>

#include "fractal/edge.h"
#include "utils/set.h"

namespace frac {

enum class AlgorithmSubdivision {
    LinksSurroundDelay,
    LinksSurroundDelayAndBezier,
    LinksOnCorners
};

class Face {
public:
    Face() : Face(std::vector<frac::Edge> {}) {}

    explicit Face(std::vector<frac::Edge> edges, unsigned int delay = 0, const frac::Edge& adjEdge = { frac::EdgeType::CANTOR, 2 }, const frac::Edge& gapEdge = { frac::EdgeType::BEZIER, 2 }, const frac::Edge& reqEdge = { frac::EdgeType::BEZIER, 2 }, AlgorithmSubdivision algo = AlgorithmSubdivision::LinksSurroundDelayAndBezier);
    static Face fromStr(std::string const& name);

    [[nodiscard]] std::vector<frac::Edge> const& constData() const;
    [[nodiscard]] std::vector<frac::Edge>& data();
    [[nodiscard]] int firstInterior() const;
    [[nodiscard]] int lastInterior() const;
    [[nodiscard]] std::size_t len() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] std::size_t offset() const;
    [[nodiscard]] frac::Edge adjEdge() const;
    [[nodiscard]] frac::Edge gapEdge() const;
    [[nodiscard]] frac::Edge reqEdge() const;
    [[nodiscard]] unsigned int delay() const;
    [[nodiscard]] AlgorithmSubdivision algo() const;

    std::optional<frac::Edge> edgeIfRequired(frac::Edge const& edge) const;

    void setAdjEdge(frac::Edge const& edge);
    void setGapEdge(frac::Edge const& edge);
    void setReqEdge(frac::Edge const& edge);
    void setDelay(unsigned int delay);
    void setFirstInterior(int index);
    void setAlgo(AlgorithmSubdivision algo);
    [[nodiscard]] std::vector<frac::Face> subdivisions() const;
    [[nodiscard]] Set<frac::Face> allSubdivisions() const;

    frac::Edge const& operator[](std::size_t index) const;
    bool operator==(frac::Face const& other) const;
    friend std::ostream& operator<<(std::ostream& os, const frac::Face& face);

    [[nodiscard]] std::string toString() const;

    static std::map<std::string, std::string> s_incidenceConstraints;
    static std::map<std::string, std::string> s_adjacencyConstraints;
    static void reset();

    static void addAdjacencyConstraint(frac::Face const& face, frac::Face const& faceSub1, frac::Face const& faceSub2, unsigned int indexSubFace1, unsigned int indexBordFace1, unsigned int indexSubFace2, unsigned int indexBordFace2);
    static void addIncidenceConstraint(frac::Face const& face, frac::Face const& faceSub, unsigned int indexParentEdge, unsigned int indexSubEdge, unsigned int indexSubFaceEdge, unsigned int indexSubFace);

    // key is name of the cell (since it is unique)
    static std::unordered_map<std::string, std::vector<frac::Face>> s_subdivisions;

    std::size_t nbControlPoints(bool bezierCubic) const;

private:
    std::vector<frac::Edge> m_data;
    unsigned int m_delay;
    frac::Edge m_adjEdge;
    frac::Edge m_gapEdge;
    frac::Edge m_reqEdge;
    std::string m_name;
    std::size_t m_offset;
    int m_firstInterior;
    AlgorithmSubdivision m_algo;

    static frac::Set<frac::Face> s_existingFaces;


    static std::size_t computeOffset(frac::Face const& face, frac::Face const& other);
};

} // frac

#endif //AUTOFRAC_FACE_H
