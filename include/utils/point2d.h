#ifndef AUTOFRACCLI_POINT2D_H
#define AUTOFRACCLI_POINT2D_H

namespace frac {

class Point2D {
public:
    Point2D() = default;

    Point2D(float x, float y) : m_x(x), m_y(y) {};

    void setX(float x) { m_x = x; };

    void setY(float y) { m_y = y; };

    float x() const { return m_x; }

    float y() const { return m_y; }

    Point2D operator+(Point2D const& other) const {
        return { m_x + other.m_x, m_y + other.m_y };
    }

    Point2D operator/(float value) const {
        return { m_x / value, m_y / value };
    }

    Point2D operator*(float value) const {
        return { m_x * value, m_y * value };
    }

private:
    float m_x;
    float m_y;
};

} // frac

#endif //AUTOFRACCLI_POINT2D_H
