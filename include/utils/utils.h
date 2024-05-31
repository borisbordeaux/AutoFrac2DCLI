#ifndef AUTOFRAC_UTILS_H
#define AUTOFRAC_UTILS_H

#include <algorithm>
#include <sstream>
#include <vector>
#include <iomanip>
#include "point2d.h"

namespace frac::utils {

inline std::string to_string(float value) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << value;
    std::string res = stream.str();
    std::replace(res.begin(), res.end(), ',', '.');
    return res;
}

template<typename T>
inline T mod(T a, T b) {
    return (a % b + b) % b;
}

template<typename T>
inline T pow(T base, T up) {
    if (up == static_cast<T>(0)) {
        return static_cast<T>(1);
    } else {
        return base * pow(base, up - static_cast<T>(1));
    }
}

template<typename T>
std::vector<T> shiftVector(std::vector<T> const& vec) {
    std::vector<T> res;
    res.reserve(vec.size());
    for (std::size_t i = 1; i < vec.size(); ++i) {
        res.emplace_back(vec[i]);
    }
    res.emplace_back(vec[0]);
    return res;
}

inline std::vector<std::string> split(std::string const& str, char const& delimiter) {
    std::vector<std::string> strings;
    std::istringstream f(str);
    std::string s;
    while (std::getline(f, s, delimiter)) {
        strings.push_back(s);
    }
    return strings;
}

inline std::vector<std::string> split(std::string const& str, std::string const& delimiter) {
    std::vector<std::string> strings;
    std::size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        strings.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    strings.push_back(str.substr(start));
    return strings;
}

inline std::vector<float> get_bezier_transformation(unsigned int i, unsigned int n) {
    float denominator = static_cast<float>(n * n);
    return { static_cast<float>((i - n) * (i - n)) / denominator,
             static_cast<float>((i - n) * (1 + i - n)) / denominator,
             static_cast<float>((1 + i - n) * (1 + i - n)) / denominator,

             static_cast<float>(2 * i * (n - i)) / denominator,
             static_cast<float>(n + 2 * i * n - 2 * i * (i + 1)) / denominator,
             static_cast<float>(-2 * (1 + i - n) * (1 + i)) / denominator,

             static_cast<float>(i * i) / denominator,
             static_cast<float>(i * (1 + i)) / denominator,
             static_cast<float>((i + 1) * (i + 1)) / denominator
    };
}

inline std::vector<float> get_bezier_cubic_transformation(unsigned int i, unsigned int n) {
    float denominator = static_cast<float>(n * n * n);
    return { static_cast<float>(-(i - n) * (i - n) * (i - n)) / denominator,
             static_cast<float>(-(i - n) * (i - n) * (i - n + 1)) / denominator,
             static_cast<float>(-(i - n) * (i - n + 1) * (i - n + 1)) / denominator,
             static_cast<float>(-(i - n + 1) * (i - n + 1) * (i - n + 1)) / denominator,

             static_cast<float>(3 * i * (i - n) * (i - n)) / denominator,
             static_cast<float>((i - n) * (3 * i * (i - n + 1) - n)) / denominator,
             static_cast<float>((i - n + 1) * (3 * i * (i - n + 1) - 2 * n)) / denominator,
             static_cast<float>(3 * (1 + i) * (i - n + 1) * (i - n + 1)) / denominator,

             static_cast<float>(3 * i * i * (n - i)) / denominator,
             static_cast<float>(i * (n * (3 * i + 2) - 3 * i * (i + 1))) / denominator,
             static_cast<float>((i + 1) * (3 * i * n + n - 3 * i * (i + 1))) / denominator,
             static_cast<float>(-3 * (i + 1) * (i + 1) * (i - n + 1)) / denominator,

             static_cast<float>(i * i * i) / denominator,
             static_cast<float>(i * i * (i + 1)) / denominator,
             static_cast<float>(i * (i + 1) * (i + 1)) / denominator,
             static_cast<float>((i + 1) * (i + 1) * (i + 1)) / denominator,
    };
}

inline Point2D coordOfPointOnLineAt(float t, Point2D p0, Point2D p1) {
    return { p0 * (1 - t) + p1 * t };
}

inline Point2D coordOfPointOnQuadCurveAt(float t, Point2D p0, Point2D p1, Point2D p2) {
    return { p0 * (1.0f - t) * (1.0f - t) + p1 * 2.0f * t * (1.0f - t) + p2 * t * t };
}

inline Point2D coordOfPointOnCubicCurveAt(float t, Point2D p0, Point2D p1, Point2D p2, Point2D p3) {
    return { p0 * (1.0f - t) * (1.0f - t) * (1.0f - t) + p1 * 3.0f * t * (1.0f - t) * (1.0f - t) + p2 * 3.0f * t * t * (1.0f - t) + p3 * t * t * t };
}

}

#endif //AUTOFRAC_UTILS_H
