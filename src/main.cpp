#include <iostream>
#include <fstream>
#include "fractal/face.h"
#include "fractal/structure.h"
#include "fractal/structureprinter.h"
#include "utils/point2d.h"
#include "utils/utils.h"

enum Mode {
    FACE,
    CONSTRAINT,
    COORD
};

int main(int argc, char* argv[]) {
    bool ok = true;
    std::string filename;
    bool autoCoord = false;
    bool cubicBezier = false;

    if (argc == 2) {
        std::cout << "Using quadratic bezier curves, intern control points not auto, reading file " << argv[1] << "..." << std::endl;
        filename = argv[1];
    } else if (argc == 3) {
        if (argv[1] == std::string("-a")) {
            std::cout << "Using quadratic bezier curves, intern control points auto, reading file " << argv[2] << "..." << std::endl;
            filename = argv[2];
            autoCoord = true;
        } else if (argv[1] == std::string("-c")) {
            std::cout << "Using cubic bezier curves, intern control points not auto, reading file " << argv[2] << "..." << std::endl;
            filename = argv[2];
            cubicBezier = true;
        }
    } else if (argc == 4 && argv[1] == std::string("-a") && argv[2] == std::string("-c")) {
        std::cout << "Using cubic bezier curves, intern control points auto, reading file " << argv[3] << "..." << std::endl;
        filename = argv[3];
        autoCoord = true;
        cubicBezier = true;
    } else {
        std::cout << "usage: AutoFracCli [-a] [-c] filename" << std::endl;
        std::cout << "\tfilename\t\t path to the input file" << std::endl;
        std::cout << "\t-a      \t\t automatic position of intern control points" << std::endl;
        std::cout << "\t-c      \t\t use cubic bezier curves, default is quadratic" << std::endl;
        ok = false;
    }

    if (!ok) { return 1; }

    std::vector<frac::Face> faces;
    std::vector<frac::Adjacency> constraints;
    std::vector<frac::Point2D> readCoords;
    std::vector<std::vector<frac::Point2D>> coords;

    Mode mode = FACE;

    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.at(0) == 'f') {
            mode = FACE;
            continue;
        }
        if (line.at(0) == 'c') {
            mode = CONSTRAINT;
            continue;
        }
        if (line.at(0) == 'p') {
            mode = COORD;
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }

        switch (mode) {
            case FACE:
                faces.emplace_back(frac::Face::fromStr(line));
                break;
            case CONSTRAINT:
                constraints.emplace_back(frac::Adjacency::fromStr(line));
                break;
            case COORD:
                std::vector<std::string> words = frac::utils::split(line, " ");
                readCoords.emplace_back(std::stof(words[0]), std::stof(words[1]));
                break;
        }
    }

    for (frac::Face const& f: faces) {
        std::cout << f.name() << std::endl;
    }

    frac::Structure structure(faces, cubicBezier);
    for (frac::Adjacency const& adj: constraints) {
        structure.addAdjacency(adj);
    }

    //fill coordinates
    std::size_t currentReadCoord = 0;
    for (std::size_t i = 0; i < faces.size(); i++) {
        coords.emplace_back();
        for (std::size_t j = 0; j < faces[i].constData().size(); j++) {
            coords[i].emplace_back(readCoords[currentReadCoord]);
            currentReadCoord++;
            if (faces[i][j].edgeType() == frac::EdgeType::BEZIER) {
                if (autoCoord) {
                    if (cubicBezier) {
                        coords[i].emplace_back(); // emplace default coord for intern control point, will be better placed after
                    }
                    coords[i].emplace_back(); // emplace default coord for intern control point, will be better placed after
                } else {
                    if (cubicBezier) {
                        coords[i].emplace_back(readCoords[currentReadCoord]);
                        currentReadCoord++;
                    }
                    coords[i].emplace_back(readCoords[currentReadCoord]);
                    currentReadCoord++;
                }
            }
        }
    }

    //fill intern control points coordinates
    if (autoCoord) {
        for (std::size_t i = 0; i < faces.size(); i++) {
            int nbCtrlPts = static_cast<int>(coords[i].size());
            bool firstInternCP = true;
            for (int j = 0; j < static_cast<int>(coords[i].size()); j++) {
                if (structure.isInternControlPoint(j, i)) {
                    if (structure.isBezierCubic()) {
                        if (firstInternCP) {
                            frac::Point2D P0 = coords[i][j - 1];
                            frac::Point2D P1 = coords[i][(j + 2) % nbCtrlPts];
                            frac::Point2D c = frac::utils::coordOfPointOnLineAt(1.f / 3.f, P0, P1);
                            coords[i][j] = c;
                            firstInternCP = false;
                        } else {
                            frac::Point2D P0 = coords[i][j - 2];
                            frac::Point2D P1 = coords[i][(j + 1) % nbCtrlPts];
                            frac::Point2D c = frac::utils::coordOfPointOnLineAt(2.f / 3.f, P0, P1);
                            coords[i][j] = c;
                            firstInternCP = true;
                        }
                    } else {
                        coords[i][j] = (coords[i][j - 1] + coords[i][(j + 1) % nbCtrlPts]) / 2.0f;
                    }
                }
            }
        }
    }

    //shift coordinates of control points for faces with an offset
    for (std::size_t i = 0; i < faces.size(); i++) {
        std::size_t offset = faces[i].offset();
        for (std::size_t j = 0; j < offset; j++) {
            coords[i] = frac::utils::shiftVector(coords[i]);
            if (faces[i][j].edgeType() == frac::EdgeType::BEZIER) {
                coords[i] = frac::utils::shiftVector(coords[i]);
                if (structure.isBezierCubic()) {
                    coords[i] = frac::utils::shiftVector(coords[i]);
                }
            }
        }
    }

    frac::StructurePrinter printer(structure, true, "output.py", coords);
    printer.exportStruct();
    std::cout << "Structure exported to file output.py" << std::endl;
    return 0;
}
