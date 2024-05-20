#include "fractal/algorithms/algorithmsurrounddelay.h"
#include "utils/utils.h"

std::vector<frac::Face> frac::LinksSurroundDelay::subdivide(const frac::Face& face) {
    std::vector<frac::Face> res;
    bool writeConstraints = Face::s_incidenceConstraints.find(face.name()) == Face::s_incidenceConstraints.end();
    if (face.delay() == 0) {
        //if face has no delay
        for (std::size_t i = 0; i < face.len(); ++i) {
            // foreach edge
            frac::Edge current = face[i];
            if (current.isDelay()) {
                // current edge has a delay so creation of one face
                frac::Edge subFirst { current };
                subFirst.decreaseDelay();
                std::vector<frac::Edge> boundaries = { subFirst };
                boundaries.push_back(face.adjEdge());
                boundaries.push_back(face.gapEdge());
                boundaries.push_back(face.adjEdge());
                frac::Face c = Face(boundaries, 0, face.adjEdge(), face.gapEdge(), face.reqEdge(), face.algo());
                c.setFirstInterior(1);
                if (writeConstraints) {
                    Face::addIncidenceConstraint(face, c, i, 0, 0, res.size());
                }
                res.push_back(c);
            } else {
                // current edge has not a delay, so we look at the edge before
                std::size_t idx = static_cast<std::size_t>(utils::mod(static_cast<int>(i) - 1, static_cast<int>(face.len())));
                if (face[idx].isDelay()) {
                    // the edge before has a delay
                    std::vector<frac::Edge> boundaries = { current };
                    std::optional<frac::Edge> requiredEdge = face.edgeIfRequired(current);
                    if (requiredEdge.has_value()) {
                        boundaries.push_back(requiredEdge.value());
                    }
                    boundaries.push_back(face.adjEdge());
                    boundaries.push_back(face.gapEdge());
                    boundaries.push_back(face.adjEdge());
                    frac::Face c = Face(boundaries, 0, face.adjEdge(), face.gapEdge(), face.reqEdge(), face.algo());
                    c.setFirstInterior(static_cast<int>(boundaries.size() - 3));
                    if (writeConstraints) {
                        Face::addIncidenceConstraint(face, c, i, 0, 0, res.size());
                    }
                    res.push_back(c);
                }

                //creation of intermediate states
                int nbIntermediateStates { static_cast<int>(current.nbSubdivisions()) - 2 };
                for (int j = 0; j < nbIntermediateStates; j++) {
                    std::vector<frac::Edge> boundaries = { current };
                    std::optional<frac::Edge> requiredEdge = face.edgeIfRequired(current);
                    if (requiredEdge.has_value()) {
                        boundaries.push_back(requiredEdge.value());
                    }
                    int indexFirstInterior { static_cast<int>(boundaries.size()) };
                    boundaries.push_back(face.adjEdge());
                    boundaries.push_back(face.gapEdge());
                    boundaries.push_back(face.adjEdge());
                    if (requiredEdge.has_value()) {
                        boundaries.push_back(requiredEdge.value());
                    }
                    frac::Face c = Face(boundaries, 0, face.adjEdge(), face.gapEdge(), face.reqEdge(), face.algo());
                    c.setFirstInterior(indexFirstInterior);
                    if (writeConstraints) {
                        Face::addIncidenceConstraint(face, c, i, j + 1, 0, res.size());
                    }
                    res.push_back(c);
                }

                // creation of last state of the edge
                frac::Edge next = face[utils::mod(i + 1, face.len())];
                if (next.isDelay()) {
                    // if next edge has delay
                    std::vector<frac::Edge> boundaries = { current };
                    boundaries.push_back(face.adjEdge());
                    boundaries.push_back(face.gapEdge());
                    boundaries.push_back(face.adjEdge());
                    std::optional<frac::Edge> requiredEdge = face.edgeIfRequired(current);
                    if (requiredEdge.has_value()) {
                        boundaries.push_back(requiredEdge.value());
                    }
                    frac::Face c = Face(boundaries, 0, face.adjEdge(), face.gapEdge(), face.reqEdge(), face.algo());
                    c.setFirstInterior(1);
                    if (writeConstraints) {
                        Face::addIncidenceConstraint(face, c, i, nbIntermediateStates + 1, 0, res.size());
                    }
                    res.push_back(c);
                } else {
                    // if next edge has no delay
                    std::vector<frac::Edge> boundaries = { current, next };
                    std::optional<frac::Edge> requiredEdge = face.edgeIfRequired(next);
                    if (requiredEdge.has_value()) {
                        boundaries.push_back(requiredEdge.value());
                    }
                    int indexFirstInterior { static_cast<int>(boundaries.size()) };
                    boundaries.push_back(face.adjEdge());
                    boundaries.push_back(face.gapEdge());
                    boundaries.push_back(face.adjEdge());
                    std::optional<frac::Edge> secondRequiredEdge = face.edgeIfRequired(current);
                    if (secondRequiredEdge.has_value()) {
                        boundaries.push_back(secondRequiredEdge.value());
                    }
                    frac::Face c = Face(boundaries, 0, face.adjEdge(), face.gapEdge(), face.reqEdge(), face.algo());
                    c.setFirstInterior(indexFirstInterior);
                    if (writeConstraints) {
                        Face::addIncidenceConstraint(face, c, i, nbIntermediateStates + 1, 0, res.size());
                        Face::addIncidenceConstraint(face, c, frac::utils::mod(i + 1, face.len()), 0, 1, res.size());
                    }
                    res.push_back(c);
                }
            }
        }
        for (std::size_t i = 0; i < res.size(); ++i) {
            frac::Face current = res[i];
            frac::Face next = res[frac::utils::mod(i + 1, res.size())];
            if (writeConstraints) {
                Face::addAdjacencyConstraint(face, current, next, i, current.firstInterior(), frac::utils::mod(i + 1, res.size()), next.lastInterior());
            }
        }
    } else {
        // current face has delay
        std::vector<frac::Edge> boundaries = {};
        for (std::size_t i = 0; i < face.len(); ++i) {
            // for each edge, we subdivide it and add it to the result face
            frac::Edge edge = face[i];
            std::vector<Edge> subdivisionsEdge = edge.subdivisions(face.reqEdge());
            for (frac::Edge const& e: subdivisionsEdge) {
                boundaries.push_back(e);
            }
        }
        frac::Face c = Face(boundaries, face.delay() - 1, face.adjEdge(), face.gapEdge(), face.reqEdge(), face.algo());
        // no adjacency constraints
        // write incidence constraints
        if (writeConstraints) {
            int k = 0;
            for (std::size_t i = 0; i < face.len(); ++i) {
                frac::Edge edge { face[i] };
                unsigned int nbSubdivisionsEdge = edge.nbActualSubdivisions();
                for (unsigned int j = 0; j < nbSubdivisionsEdge; ++j) {
                    Face::addIncidenceConstraint(face, c, i, j, k, 0);
                    if (edge.edgeType() == EdgeType::BEZIER) {
                        k++;
                    }
                    if (edge.edgeType() == EdgeType::CANTOR) {
                        if (edge.isDelay()) {
                            k++;
                        } else {
                            k += (j != (nbSubdivisionsEdge - 1)) ? 3 : 1;
                        }
                    }
                }
            }
        }
        res.push_back(c);
    }
    Face::s_subdivisions[face.name()] = res;
    return res;
}
