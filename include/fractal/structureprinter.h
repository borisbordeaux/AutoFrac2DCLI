#ifndef AUTOFRAC_STRUCTUREPRINTER_H
#define AUTOFRAC_STRUCTUREPRINTER_H

#include <vector>
#include <string>
#include "utils/fileprinter.h"

namespace frac {

class Edge;

class Face;

class Point2D;

class Structure;

class StructurePrinter {
public:
    explicit StructurePrinter(frac::Structure const& structure, bool planarControlPoints, std::string filename, std::vector<std::vector<Point2D>> const& coords = {});
    void exportStruct();
private:
    void print_header();
    void print_vertex_state();
    void print_decl_of_edge(frac::Edge const& edge);
    void print_delay_cantor_decl(unsigned int n, unsigned int delay_count);
    void print_cantor_n_state_decl(unsigned int n);
    void print_delay_bezier_decl(unsigned int n, unsigned int delay_count);
    void print_bezier_state_decl(unsigned int n);
    void print_impl_of_edge(frac::Edge const& edge);
    void print_delay_cantor_impl(unsigned int n, unsigned int delay_count);
    void print_cantor_n_state_impl(unsigned int n);
    void print_delay_bezier_impl(unsigned int n, unsigned int delay_count);
    void print_bezier_state_impl(unsigned int n);
    void print_init_subds();
    void print_edges_of_cell(frac::Face const& cell);
    void print_subd_of_cell(frac::Face const& cell);
    void print_space_of_cell(frac::Face const& cell);
    void print_prim_of_cell(frac::Face const& cell);
    void print_edge_adjacencies_of_cell(frac::Face const& cell);
    void print_plan_control_points();
    void print_plan_coords_control_points();
    void print_footer();

private:
    frac::Structure const& m_structure;
    bool m_planarControlPoints;
    std::string const m_filename;
    std::vector<std::vector<Point2D>> const& m_coords;
    frac::FilePrinter m_filePrinter;
};
}
#endif //AUTOFRAC_STRUCTUREPRINTER_H
