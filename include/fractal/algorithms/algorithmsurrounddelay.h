#ifndef AUTOFRAC_ALGORITHM_H
#define AUTOFRAC_ALGORITHM_H

#include <vector>
#include "fractal/face.h"

namespace frac::LinksSurroundDelay {

std::vector<frac::Face> subdivide(frac::Face const& face);

}

#endif //AUTOFRAC_ALGORITHM_H
