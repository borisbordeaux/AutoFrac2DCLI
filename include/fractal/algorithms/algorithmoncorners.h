#ifndef AUTOFRAC_ALGORITHMONCORNERS_H
#define AUTOFRAC_ALGORITHMONCORNERS_H

#include <vector>
#include "fractal/face.h"

namespace frac::LinksOnCorners {

std::vector<frac::Face> subdivide(frac::Face const& face);

}

#endif //AUTOFRAC_ALGORITHMONCORNERS_H
