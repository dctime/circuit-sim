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
  double scaleX, scaleY;

  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(gridLines, states);
    target.draw(axisLines, states);
  }

public:
  CoordinateGraph(float windowWidth, float windowHeight,
                  float gridSpacing = 50.0f, double xGridUnit = 1.0,
                  double yGridUnit = 1.0)
      : gridLines(sf::Lines), axisLines(sf::Lines), width(windowWidth),
        height(windowHeight), gridSize(gridSpacing), scaleX(1.0 / xGridUnit),
        scaleY(1.0 / yGridUnit) {

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
    return sf::Vector2f(origin.x + x * gridSize * scaleX,
                        origin.y - y * gridSize * scaleY // Inverted y-axis
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
  sf::RenderWindow window(sf::VideoMode(800, 600), "X-Y Coordinate Graph");
  window.setFramerateLimit(60);

  CoordinateGraph graph(800.0f, 600.0f, 50.0f, 1, 0.2);

  std::vector<double> valuesV1;
  std::vector<double> valuesV0;

  std::function<double(double)> vgsOfT = [](double t) { return 13.3*pow(10, -3) * sin(t) + 0.6; };
  for (double t = 0; t <= 5; t += 0.01) {
    double vgs = vgsOfT(t);
    // nmos stats
    double k = 4 * pow(10, -3);
    double vt = 0.4;
    // double va = std::numeric_limits<double>::infinity();
    double va = INFINITY;

    // G V and I matrix
    Eigen::MatrixXd v(5, 1);
    // initial guess
    v << 0, 0, 0, 0, 0;

    for (int iteration = 0; iteration < 10; iteration++) {
      double r0 = 17.5 * pow(10, 3);
      Eigen::MatrixXd g(5, 5);
      double g0;
      g0 = calg0(k, v(0), v(1), vt, va);
      double g00 = 0;
      double g01 = 0;
      double g10 = 0;
      double g02 = 0;
      double g20 = 0;
      double g11 = g0 + 1.0 / r0;
      double g12 = -1.0 / r0;
      double g21 = -1.0 / r0;
      double g22 = 1.0 / r0;

      g << g00, g01, g02, 1, 0,
        g10, g11, g12, 0, 0,
        g20, g21, g22, 0, 1,
        1, 0, 0, 0, 0,
        0, 0, 1, 0, 0;

      Eigen::MatrixXd i(5, 1);
      double issd = calgm(k, v(0), v(1), vt, va) * (v(0));
      // std::cout << "issd: " << issd << std::endl;
      double ideq = calideq(k, v(0), v(1), vt, va);
      // std::cout << "ideq: " << ideq << std::endl;
      i << 0, -issd - ideq, 0, vgs, 1.8;

      // std::cout << "g v i success" << std::endl;
      // F matrix
      Eigen::MatrixXd f(5, 1);
      f = g * v - i;

      // std::cout << "f matrix success" << std::endl;
      // J matrix
      double delta = 0.00000001;
      Eigen::MatrixXd dupV = v.replicate(1, 5);
      Eigen::MatrixXd vWithDelta = Eigen::MatrixXd::Identity(5, 5);
      vWithDelta = vWithDelta * delta + dupV;
      // std::cout << "vWithData value: " << vWithDelta << std::endl;
      Eigen::MatrixXd j = (g * vWithDelta - g * dupV) / delta;
      // std::cout << "j" << std::endl;
      // std::cout << j << std::endl;

      // std::cout << "j matrix success" << std::endl;

      // calculate deltaV
      Eigen::MatrixXd deltaV = -1 * ((j.inverse()) * f);
      // std::cout << "deltaV value:" << std::endl;
      // std::cout << deltaV << std::endl;
      // calculate new v
      v += deltaV;
      // std::cout << "cal v success" << std::endl;

      // print v
      // std::cout << "iteration: " << iteration << std::endl;
      // std::cout << "V0\nV1\nIx\nIY:" << std::endl;
      // std::cout << v << std::endl;
      // std::cout << "===============" << std::endl;
      // std::cout << "evaluation:" << std::endl;
      // std::cout << g * v - i << std::endl;
      // std::cout << "===============" << std::endl;
    }
    std::cout << "v0: " << v(0) << " v1: " << v(1) << std::endl;
    valuesV0.push_back(v(0));
    valuesV1.push_back(v(1));
  }

  sf::VertexArray plotV1(sf::LineStrip);
  graph.applyVectorToPlot(valuesV1, plotV1, 0.01, sf::Color::Green);
  sf::VertexArray plotV0(sf::LineStrip);
  graph.applyVectorToPlot(valuesV0, plotV0, 0.01, sf::Color::Red);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);
    window.draw(graph);
    window.draw(plotV0);
    window.draw(plotV1);
    window.display();
  }

  return 0;
}
