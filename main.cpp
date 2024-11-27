#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <cmath>
#include <eigen3/Eigen/Dense>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
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

void showLine(sf::RenderWindow *window, sf::Vector2f &point1,
              sf::Vector2f &point2, double width, sf::Color color1,
              sf::Color color2, sf::Color colorm) {
  sf::VertexArray background(sf::TriangleStrip, 6);
  float angle = std::atan2(point2.y - point1.y, point2.x - point1.x);
  // std::cout << "angle:" << angle << std::endl;
  background[0].position =
      sf::Vector2f(point1.x + (width / 2.0) * std::sin(angle),
                   point1.y - (width / 2.0) * std::cos(angle));
  background[1].position =
      sf::Vector2f(point1.x - (width / 2.0) * std::sin(angle),
                   point1.y + (width / 2.0) * std::cos(angle));
  background[2].position = sf::Vector2f(
      (point1.x + point2.x) / 2.0 + (width / 2.0) * std::sin(angle),
      (point1.y + point2.y) / 2.0 - (width / 2.0) * std::cos(angle));
  background[3].position = sf::Vector2f(
      (point1.x + point2.x) / 2.0 - (width / 2.0) * std::sin(angle),
      (point1.y + point2.y) / 2.0 + (width / 2.0) * std::cos(angle));
  background[4].position =
      sf::Vector2f(point2.x + (width / 2.0) * std::sin(angle),
                   point2.y - (width / 2.0) * std::cos(angle));
  background[5].position =
      sf::Vector2f(point2.x - (width / 2.0) * std::sin(angle),
                   point2.y + (width / 2.0) * std::cos(angle));

  background[0].color = color1;
  background[1].color = color1;
  background[2].color = colorm;
  background[3].color = colorm;
  background[4].color = color2;
  background[5].color = color2;

  window->draw(background);
}

void voltToColor(double voltage, sf::Color &color) {
  if (voltage >= 0) {
    if (voltage >= 1)
      voltage = 1;
    color.a = 255;
    color.r = 30;
    color.g = 30 + (255 - 30) * voltage;
  } else {
    if (voltage <= -1)
      voltage = -1;
    color.a = 255;
    color.r = 30 + (255 - 30) * voltage;
    color.g = 30;
    color.b = 30;
  }
}

void midColor(sf::Color &outputColor, sf::Color &color1, sf::Color &color2) {
  int color1Value = (color1.r - 30) - (color1.g - 30);
  int color2Value = (color2.r - 30) - (color2.g - 30);
  int midValue = (color1Value + color2Value) / 2;
  outputColor.a = 255;
  outputColor.r = 30;
  outputColor.g = 30;
  outputColor.b = 30;
  if (midValue >= 0) {
    outputColor.r += midValue;
  } else {
    outputColor.g += -midValue;
  }
}

void showNMOS(sf::RenderWindow *window, double vg, double vd, double vs,
              sf::Vector2f &loc) {
  // pin_d 100 100
  // pin_g 0 150
  // pin_s 100 200
  // loc 100 150
  double width = 5;
  sf::Vector2f pointD1(loc.x, loc.y - 50);
  sf::Vector2f pointD2(loc.x, loc.y - 25);
  sf::Vector2f pointB1(loc.x - 50 + 2.5, loc.y - 25);
  sf::Vector2f pointB2(loc.x - 50 + 2.5, loc.y + 25);
  sf::Vector2f pointBG1(loc.x - 50 - 7.5, loc.y - 25);
  sf::Vector2f pointBG2(loc.x - 50 - 7.5, loc.y + 25);
  sf::Vector2f pointG1(loc.x - 50 - 7.5, loc.y);
  sf::Vector2f pointG2(loc.x - 100, loc.y);
  sf::Vector2f pointS1(loc.x, loc.y + 25);
  sf::Vector2f pointS11(loc.x - 10, loc.y + 25 + 10);
  sf::Vector2f pointS12(loc.x - 10, loc.y + 25 - 10);
  sf::Vector2f pointS2(loc.x, loc.y + 50);

  sf::Color gColor;
  voltToColor(vg, gColor);
  sf::Color dColor;
  voltToColor(vd, dColor);
  sf::Color sColor;
  voltToColor(vs, sColor);
  sf::Color bColor;
  midColor(bColor, dColor, sColor);

  showLine(window, pointD1, pointD2, 5, dColor, dColor, dColor);
  showLine(window, pointB1, pointB2, width, dColor, sColor, bColor);
  showLine(window, pointBG1, pointBG2, width, gColor, gColor, gColor);
  showLine(window, pointG1, pointG2, 5, gColor, gColor, gColor);
  showLine(window, pointS1, pointS2, 5, sColor, sColor, sColor);
  showLine(window, pointD2, pointB1, 5, dColor, dColor, dColor);
  showLine(window, pointB2, pointS1, 5, sColor, sColor, sColor);
  showLine(window, pointS1, pointS11, width, sColor, sColor, sColor);
  showLine(window, pointS1, pointS12, width, sColor, sColor, sColor);
}

double lastoffset = 0;
void showResistor(sf::RenderWindow *window, double v1, double v2,
                  sf::Vector2f &loc, double R, double t) {
  // pin1 302.5 300
  // pin2 302.5 400
  // loc 302.5 350
  sf::VertexArray bg(sf::TriangleStrip, 12);
  bg[0].position = sf::Vector2f(loc.x - 2.5, loc.y - 50);
  bg[1].position = sf::Vector2f(loc.x + 2.5, loc.y - 50);
  bg[2].position = sf::Vector2f(loc.x - 2.5, loc.y - 10);
  bg[3].position = sf::Vector2f(loc.x + 2.5, loc.y - 10);
  bg[4].position = sf::Vector2f(loc.x - 7.5, loc.y - 5);
  bg[5].position = sf::Vector2f(loc.x - 2.5, loc.y - 5);
  bg[6].position = sf::Vector2f(loc.x + 2.5, loc.y + 5);
  bg[7].position = sf::Vector2f(loc.x + 7.5, loc.y + 5);
  bg[8].position = sf::Vector2f(loc.x - 2.5, loc.y + 10);
  bg[9].position = sf::Vector2f(loc.x + 2.5, loc.y + 10);
  bg[10].position = sf::Vector2f(loc.x - 2.5, loc.y + 50);
  bg[11].position = sf::Vector2f(loc.x + 2.5, loc.y + 50);

  sf::Color color1;
  voltToColor(v1, color1);
  sf::Color color2;
  voltToColor(v2, color2);
  sf::Color mColor;
  sf::Color m1Color;
  sf::Color m2Color;
  midColor(mColor, color1, color2);
  midColor(m1Color, color1, mColor);
  midColor(m2Color, mColor, color2);

  bg[0].color = color1;
  bg[1].color = color1;
  bg[2].color = color1;
  bg[3].color = color1;
  bg[4].color = m1Color;
  bg[5].color = m1Color;
  bg[6].color = m2Color;
  bg[7].color = m2Color;
  bg[8].color = color2;
  bg[9].color = color2;
  bg[10].color = color2;
  bg[11].color = color2;

  window->draw(bg);
  double current = (v1 - v2) / R;
  double currentScale = 10000;
  lastoffset += current*currentScale;
  lastoffset = std::remainder(lastoffset, 20);
  lastoffset = lastoffset < 0 ? lastoffset+20.0 : lastoffset;
  for (int i = 0; i < 5; ++i) {
    sf::CircleShape circle(5.0 / 2);
    circle.setPosition(loc.x - circle.getRadius(),
                       loc.y - 50 + 20 * i - circle.getRadius() +
                           lastoffset);
    window->draw(circle);
  }
}

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
  sf::Font font;
  if (!font.loadFromFile("../arial.ttf")) {
    std::cout << "Font Load Failed!" << std::endl;
  }
  sf::RenderWindow window(sf::VideoMode(800, 600), "X-Y Coordinate Graph");
  window.setFramerateLimit(60);

  // CoordinateGraph graph(800.0f, 600.0f, 50.0f, 1, 0.2);

  // sf::VertexArray plotV1(sf::LineStrip);
  // graph.applyVectorToPlot(valuesV1, plotV1, 0.01, sf::Color::Green);
  // sf::VertexArray plotV0(sf::LineStrip);
  // graph.applyVectorToPlot(valuesV0, plotV0, 0.01, sf::Color::Red);
  sf::Text text;
  text.setCharacterSize(20);
  text.setStyle(sf::Text::Bold);
  text.setFillColor(sf::Color::White);
  text.setPosition(0, 0);
  text.setFont(font);

  double t = 0;
  while (window.isOpen()) {

    t += 0.01;
    std::function<double(double)> vgsOfT = [](double t) {
      return 13.3 * pow(10, -3) * sin(5 * t) + 0.6;
    };
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    double vgs = vgsOfT(t);
    // nmos stats
    double k = 4 * pow(10, -3);
    double vt = 0.4;
    // double va = std::numeric_limits<double>::infinity();
    double va = INFINITY;
    double r0 = 17.5 * pow(10, 3);

    // G V and I matrix
    Eigen::MatrixXd v(5, 1);
    // initial guess
    v << 0, 0, 0, 0, 0;

    for (int iteration = 0; iteration < 10; iteration++) {
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

      g << g00, g01, g02, 1, 0, g10, g11, g12, 0, 0, g20, g21, g22, 0, 1, 1, 0,
          0, 0, 0, 0, 0, 1, 0, 0;

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
    text.setString("vg: " + std::to_string(v(0)) + " | vd: " +
                   std::to_string(v(1)) + " | t: " + std::to_string(t));
    window.clear(sf::Color::Black);
    sf::Vector2f nmosLoc(200, 300);
    sf::Vector2f resisLoc(200, 200);
    showNMOS(&window, v(0), v(1), 0, nmosLoc);
    showResistor(&window, v(2), v(1), resisLoc, r0, t);
    window.draw(text);
    window.display();
  }

  // delete bgD;

  return 0;
}
