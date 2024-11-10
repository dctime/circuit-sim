#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cmath>
#include <eigen3/Eigen/Dense>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

class CoordinateGraph : public sf::Drawable {
private:
  sf::VertexArray gridLines;
  sf::VertexArray axisLines;
  sf::Vector2f origin;
  float width, height;
  float gridSize;
  float scale;

  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(gridLines, states);
    target.draw(axisLines, states);
  }

public:
  CoordinateGraph(float windowWidth, float windowHeight,
                  float gridSpacing = 50.0f, float scaleFactor = 1.0f)
      : gridLines(sf::Lines), axisLines(sf::Lines), width(windowWidth),
        height(windowHeight), gridSize(gridSpacing), scale(scaleFactor) {

    origin = sf::Vector2f(width / 2, height / 2);
    createGrid();
    createAxes();
  }

  void createGrid() {
    // Vertical lines
    for (float x = origin.x; x <= width; x += gridSize) {
      gridLines.append(
          sf::Vertex(sf::Vector2f(x, 0), sf::Color(100, 100, 100)));
      gridLines.append(
          sf::Vertex(sf::Vector2f(x, height), sf::Color(100, 100, 100)));
    }
    for (float x = origin.x; x >= 0; x -= gridSize) {
      gridLines.append(
          sf::Vertex(sf::Vector2f(x, 0), sf::Color(100, 100, 100)));
      gridLines.append(
          sf::Vertex(sf::Vector2f(x, height), sf::Color(100, 100, 100)));
    }

    // Horizontal lines
    for (float y = origin.y; y <= height; y += gridSize) {
      gridLines.append(
          sf::Vertex(sf::Vector2f(0, y), sf::Color(100, 100, 100)));
      gridLines.append(
          sf::Vertex(sf::Vector2f(width, y), sf::Color(100, 100, 100)));
    }
    for (float y = origin.y; y >= 0; y -= gridSize) {
      gridLines.append(
          sf::Vertex(sf::Vector2f(0, y), sf::Color(100, 100, 100)));
      gridLines.append(
          sf::Vertex(sf::Vector2f(width, y), sf::Color(100, 100, 100)));
    }
  }

  void createAxes() {
    // X-axis
    axisLines.append(sf::Vertex(sf::Vector2f(0, origin.y), sf::Color::White));
    axisLines.append(
        sf::Vertex(sf::Vector2f(width, origin.y), sf::Color::White));

    // Y-axis
    axisLines.append(sf::Vertex(sf::Vector2f(origin.x, 0), sf::Color::White));
    axisLines.append(
        sf::Vertex(sf::Vector2f(origin.x, height), sf::Color::White));
  }

  // Convert graph coordinates to screen coordinates
  sf::Vector2f graphToScreen(float x, float y) {
    return sf::Vector2f(origin.x + x * gridSize * scale,
                        origin.y - y * gridSize * scale // Inverted y-axis
    );
  }

  // Plot a function
  sf::VertexArray plotFunction(std::function<float(float)> func, float xStart,
                               float xEnd, sf::Color color = sf::Color::Red) {
    sf::VertexArray points(sf::LineStrip);

    for (float x = xStart; x <= xEnd; x += 0.1f) {
      float y = func(x);
      sf::Vector2f screenPos = graphToScreen(x, y);
      points.append(sf::Vertex(screenPos, color));
    }

    return points;
  }

  void applyVectorToPlot(std::vector<double> &values, sf::VertexArray &plot,
                         float deltaX, sf::Color color = sf::Color::Red) {
    for (int i = 0; i < values.size(); i++) {
      double x = i * deltaX;
      double y = values.at(i);
      sf::Vector2f screenPos = graphToScreen(x, y);
      plot.append(sf::Vertex(screenPos, color));
    }
  }
};

// change return by vgs and vds
// only for saturation
double calg0(double k, double vgs, double vds, double vt, double va) {
  if (vgs - vt < 0)
    return 0;
  if (vds <= vgs - vt)
    return k * ((vgs - vt) - vds);
  return ((k / 2) * pow(vgs - vt, 2)) / va;
}

double calgm(double k, double vgs, double vds, double vt, double va) {
  if (vgs - vt < 0)
    return 0;
  if (vds <= vgs - vt)
    return k * vds;
  return k * (vgs - vt) * (1 + (vds - (vgs - vt)) / va);
}

double calid(double k, double vgs, double vds, double vt, double va) {
  if (vgs - vt < 0)
    return 0;
  if (vds <= vgs - vt)
    return k * ((vgs - vt) - vds / 2) * vds;
  return (k / 2) * pow(vgs - vt, 2) * (1 + (vds - (vgs - vt)) / va);
}

double calideq(double k, double vgs, double vds, double vt, double va) {
  return calid(k, vgs, vds, vt, va) - calgm(k, vgs, vds, vt, va) * vgs -
         calg0(k, vgs, vds, vt, va) * vds;
}

int main() {
  // nmos stats
  double k = 1 * pow(10, -3);
  double vt = 1;
  // double va = std::numeric_limits<double>::infinity();
  double va = INFINITY;

  // G V and I matrix
  Eigen::MatrixXd v(7, 1);
  // initial guess
  v << 0, 0, 0, 0, 0, 0, 0;

  for (int iteration = 0; iteration < 100; iteration++) {
    Eigen::MatrixXd g(7, 7);
    double g0;
    g0 = calg0(k, v(1) - v(4), v(3) - v(4), vt, va);
    double g1 = 1.0 / (10 * pow(10, 6));
    double g2 = 1.0 / (10 * pow(10, 6));
    double g3 = 1.0 / 6000.0;
    double g4 = 1.0 / 6000.0;

    double g00 = g1;
    double g01 = -g1;
    double g10 = -g1;
    double g02 = 0;
    double g20 = 0;
    double g03 = 0;
    double g30 = 0;
    double g04 = 0;
    double g40 = 0;
    double g11 = g1 + g2;
    double g12 = 0;
    double g21 = 0;
    double g13 = 0;
    double g31 = 0;
    double g14 = 0;
    double g41 = 0;
    double g22 = g3;
    double g23 = -g3;
    double g32 = -g3;
    double g24 = 0;
    double g42 = 0;
    double g33 = g3 + g0;
    double g34 = -g0;
    double g43 = -g0;
    double g44 = g0 + g4;

    g << g00, g01, g02, g03, g04, 1, 0,
      g10, g11, g12, g13, g14, 0, 0,
      g20, g21, g22, g23, g24, 0, 1,
      g30, g31, g32, g33, g34, 0, 0,
      g40, g41, g42, g43, g44, 0, 0,
      1, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0, 0;

    Eigen::MatrixXd i(7, 1);
    double issd = calgm(k, v(1)-v(4), v(3)-v(4), vt, va) * (v(1)-v(4));
    std::cout << "issd: " << issd << std::endl;
    double ideq = calideq(k, v(1)-v(4), v(3)-v(4), vt, va);
    std::cout << "ideq: " << ideq << std::endl;
    i << 0, 0, 0, -issd - ideq, issd + ideq, 10, 10;

    std::cout << "g v i success" << std::endl;
    // F matrix
    Eigen::MatrixXd f(7, 1);
    f = g * v - i;

    std::cout << "f matrix success" << std::endl;
    // J matrix
    double delta = 0.00000001;
    Eigen::MatrixXd dupV = v.replicate(1, 7);
    Eigen::MatrixXd vWithDelta = Eigen::MatrixXd::Identity(7, 7);
    vWithDelta = vWithDelta * delta + dupV;
    std::cout << "vWithData value: " << vWithDelta << std::endl;
    Eigen::MatrixXd j = (g * vWithDelta - g * dupV) / delta;
    std::cout << "j" << std::endl;
    std::cout << j << std::endl;

    std::cout << "j matrix success" << std::endl;

    // calculate deltaV
    Eigen::MatrixXd deltaV = (-1.0/(iteration+1)) * ((j.inverse()) * f);
    std::cout << "deltaV value:" << std::endl;
    std::cout << deltaV << std::endl;
    // calculate new v
    v += deltaV;
    std::cout << "cal v success" << std::endl;

    // print v
    std::cout << "iteration: " << iteration << std::endl;
    std::cout << "V0\nV1\nV2\nV3\nV4\nIx\nIY:" << std::endl;
    std::cout << v << std::endl;
    std::cout << "===============" << std::endl;
    std::cout << "evaluation:" << std::endl;
    std::cout << g * v - i << std::endl;
    std::cout << "===============" << std::endl;
  }
  return 0;
}
