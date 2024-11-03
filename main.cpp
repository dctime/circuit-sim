#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cmath>
#include <eigen3/Eigen/Dense>
#include <functional>
#include <iostream>
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

double diodeCurrent(double voltage, double Is, double vt) {
  return Is * (exp(voltage / vt) - 1);
}

double diodeGeq(double voltageDiff, double vt, double Is) {
  return (diodeCurrent(voltageDiff, Is, vt) + Is) / vt;
}

double diodeIeq(double voltageDifference, double Is, double vt) {
  double current = diodeCurrent(voltageDifference, Is, vt);
  std::cout << "diodeIeq (current): " << current << std::endl;
  double g = diodeGeq(voltageDifference, vt, Is);
  std::cout << "diodeIeq (g): " << g << std::endl;
  return current - g * voltageDifference;
}

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "X-Y Coordinate Graph");
  window.setFramerateLimit(60);

  // === circuit calculation ===
  double vt = 0.805;
  double Is = 171 * pow(10, -9);

  double R = 1000;

  // init V vector
  Eigen::MatrixXd v(4, 1);
  v << 1, 1, 1, 1;
  std::cout << "v: " << v.rows() << " * " << v.cols() << std::endl;

  for (int index = 0; index < 10; index++) {
    // init G Matrix
    Eigen::MatrixXd g(4, 4);
    double geq1 = diodeGeq(v(0) - v(1), vt, Is);
    std::cout << "geq1: " << geq1 << std::endl;
    double geq2 = diodeGeq(v(1) - v(2), vt, Is);
    std::cout << "geq2: " << geq2 << std::endl;
    g << geq1, -geq1, 0, 1,
      -geq1, geq1 + geq2, -geq2, 0,
      0, -geq2, geq2 + 1/R, 0,
        1, 0, 0, 0;
    std::cout << "g: " << g.rows() << " * " << g.cols() << std::endl;
    std::cout << "g value: " << g << std::endl;
    // init I Matrix
    double Ieq1 = diodeIeq(v(0) - v(1), Is, vt);
    std::cout << "Ieq1 value: " << Ieq1 << std::endl;
    double Ieq2 = diodeIeq(v(1) - v(2), Is, vt);
    std::cout << "Ieq2 value: " << Ieq2 << std::endl;
    Eigen::MatrixXd i(4, 1);
    i << -Ieq1, Ieq1 - Ieq2, Ieq2, 5;
    std::cout << "i: " << i.rows() << " * " << i.cols() << std::endl;
    // Calculate Jacobian Matrix
    double delta = 0.000000000001;
    Eigen::MatrixXd identityDelta = Eigen::MatrixXd::Identity(4, 4) * delta;
    std::cout << "identityData: "  << identityDelta.rows() << " * "
              << identityDelta.cols() << std::endl;
    std::cout << "identityData Values:\n" << identityDelta << std::endl;
    Eigen::MatrixXd colCopiedV = v.replicate(1, 4);
    Eigen::MatrixXd vWithDelta = colCopiedV + identityDelta;
    std::cout << "vWithData: " << vWithDelta.rows() << " * "
              << vWithDelta.cols() << std::endl;
    std::cout << "vWithData Values:\n" << vWithDelta << std::endl; 
    Eigen::MatrixXd j = (g * vWithDelta - g * colCopiedV) / delta;
    std::cout << "j: " << j.rows() << " * " << j.cols() << std::endl;
    std::cout << "j values:\n" << j << std::endl;
    // Caculate F(v)
    Eigen::MatrixXd f = g * v - i;
    std::cout << "f: " << f.rows() << " * " << f.cols() << std::endl;
    // Find deltaV
    Eigen::MatrixXd deltaV = -1 * j.inverse() * f;
    std::cout << "deltaV: " << deltaV.rows() << " * " << deltaV.cols()
              << std::endl;
    // check deltaV is small enough
    std::cout << "deltaV Values:\n" << deltaV << std::endl;
    // apply deltaV to V
    v += deltaV;
    std::cout << "Voltage Values\nV1\nV2\nV3\nI:\n" << v << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "evaluate:\n" << g * v - i << std::endl;
    std::cout << "iteration steps: " << index+1 << std::endl;
    // repeat
  }

  // CoordinateGraph graph(800.0f, 600.0f, 10.0f, 1.0f);

  // Example: Plot y = x^2
  // auto quadratic = [](float x) { return x * x; };
  // sf::VertexArray quadraticPlot = graph.plotFunction(quadratic, -4, 4);
  // sf::VertexArray plotV1(sf::LineStrip);
  // graph.applyVectorToPlot(valuesV1, plotV1, 0.01, sf::Color::Green);
  // sf::VertexArray plotI1(sf::LineStrip);
  // graph.applyVectorToPlot(valuesI1, plotI1, 0.01, sf::Color::Yellow);

  // while (window.isOpen()) {
  //   sf::Event event;
  //   while (window.pollEvent(event)) {
  //     if (event.type == sf::Event::Closed)
  //       window.close();
  //   }
  //
  //   window.clear(sf::Color::Black);
  //   window.draw(graph);
  //   window.draw(plotV1);
  //   window.draw(plotI1);
  //   window.display();
  // }

  return 0;
}
