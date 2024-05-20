#ifndef AUTOFRAC_ALGORITHMSURROUNDDELAYANDBEZIER_H
#define AUTOFRAC_ALGORITHMSURROUNDDELAYANDBEZIER_H

#include <vector>
#include "fractal/face.h"

namespace frac::LinksSurroundDelayAndBezier {

std::vector<frac::Face> subdivide(frac::Face const& face);

}

#endif //AUTOFRAC_ALGORITHMSURROUNDDELAYANDBEZIER_H
