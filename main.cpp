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
#include <eigen3/Eigen/src/Core/Matrix.h>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
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
    color.r = 30 + (255 - 30) * voltage * (-1);
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

struct Wire {
  double lastoffsetVolt = 0;
  void showWire(sf::RenderWindow *window, sf::Vector2f &loc1,
                sf::Vector2f &loc2, double v, double i, double currentScale) {
    double width = 5;
    sf::Color wireColor;
    voltToColor(v, wireColor);
    showLine(window, loc1, loc2, width, wireColor, wireColor, wireColor);

    double current = i;
    lastoffsetVolt += current * currentScale;
    lastoffsetVolt = std::remainder(lastoffsetVolt, 20);
    lastoffsetVolt =
        lastoffsetVolt < 0 ? lastoffsetVolt + 20.0 : lastoffsetVolt;
    double length =
        std::sqrt(std::pow(loc1.x - loc2.x, 2) + std::pow(loc1.y - loc2.y, 2));
    double unitX = (loc2.x - loc1.x) / length;
    double unitY = (loc2.y - loc1.y) / length;
    int max = (int)(std::sqrt(std::pow(loc1.x - loc2.x, 2) +
                              std::pow(loc1.y - loc2.y, 2)) /
                    20);
    for (int i = 0; i < max; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(loc1.x + 20 * i * unitX - circle.getRadius() +
                             lastoffsetVolt * unitX,
                         loc1.y + 20 * i * unitY - circle.getRadius() +
                             lastoffsetVolt * unitY);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }
};

void showGround(sf::RenderWindow *window, sf::Vector2f &loc) {
  double width = 5;
  sf::Vector2f pointG1(loc.x - 25, loc.y);
  sf::Vector2f pointG2(loc.x + 25, loc.y);
  sf::Vector2f pointG3(loc.x - 10, loc.y + 10);
  sf::Vector2f pointG4(loc.x + 10, loc.y + 10);

  showLine(window, pointG1, pointG2, width, sf::Color(30, 30, 30),
           sf::Color(30, 30, 30), sf::Color(30, 30, 30));
  showLine(window, pointG3, pointG4, width, sf::Color(30, 30, 30),
           sf::Color(30, 30, 30), sf::Color(30, 30, 30));
}

struct AdjustableVoltageSource {
  double lastoffsetVolt = 0;
  void showAdjustableVoltageSource(sf::RenderWindow *window, double vp,
                                   double vm, double i, sf::Vector2f &loc,
                                   double currentScale) {
    double width = 5;

    sf::CircleShape circle(25);
    circle.setFillColor(sf::Color(0, 0, 0));
    circle.setOutlineThickness(width);
    circle.setOutlineColor(sf::Color(30, 30, 30));

    circle.setPosition(loc.x - circle.getRadius(), loc.y - circle.getRadius());

    sf::Vector2f pointP1(loc.x, loc.y - 50);
    sf::Vector2f pointP2(loc.x, loc.y - 25);
    sf::Vector2f pointM1(loc.x, loc.y + 50);
    sf::Vector2f pointM2(loc.x, loc.y + 25);
    sf::Vector2f pointADD1(loc.x - 10, loc.y - 10);
    sf::Vector2f pointADD2(loc.x + 10, loc.y - 10);
    sf::Vector2f pointADD3(loc.x, loc.y - 10 + 10);
    sf::Vector2f pointADD4(loc.x, loc.y - 10 - 10);
    sf::Vector2f pointMINUS1(loc.x - 10, loc.y + 12.5);
    sf::Vector2f pointMINUS2(loc.x + 10, loc.y + 12.5);

    sf::Color colorP;
    voltToColor(vp, colorP);
    sf::Color colorM;
    voltToColor(vm, colorM);

    showLine(window, pointP1, pointP2, width, colorP, colorP, colorP);
    showLine(window, pointM1, pointM2, width, colorM, colorM, colorM);

    window->draw(circle);

    showLine(window, pointADD1, pointADD2, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    showLine(window, pointADD3, pointADD4, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    showLine(window, pointMINUS1, pointMINUS2, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));

    double current = i;
    lastoffsetVolt += current * currentScale;
    lastoffsetVolt = std::remainder(lastoffsetVolt, 20);
    lastoffsetVolt =
        lastoffsetVolt < 0 ? lastoffsetVolt + 20.0 : lastoffsetVolt;
    for (int i = 0; i < 5; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(pointP1.x - circle.getRadius(),
                         pointP1.y + 20 * i - circle.getRadius() +
                             lastoffsetVolt);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }
};

struct VoltageSource {
  double lastoffsetVolt = 0;
  void showVoltageSource(sf::RenderWindow *window, double vp, double vm,
                         double i, sf::Vector2f &loc, double currentScale) {
    double width = 5;

    sf::Vector2f pointP1(loc.x, loc.y - 50);
    sf::Vector2f pointP2(loc.x, loc.y - 10);
    sf::Vector2f pointP3(loc.x - 30, loc.y - 10);
    sf::Vector2f pointP4(loc.x + 30, loc.y - 10);
    sf::Vector2f pointM1(loc.x, loc.y + 50);
    sf::Vector2f pointM2(loc.x, loc.y + 10);
    sf::Vector2f pointM3(loc.x - 20, loc.y + 10);
    sf::Vector2f pointM4(loc.x + 20, loc.y + 10);

    sf::Color colorP;
    voltToColor(vp, colorP);
    sf::Color colorM;
    voltToColor(vm, colorM);

    showLine(window, pointP1, pointP2, width, colorP, colorP, colorP);
    showLine(window, pointP3, pointP4, width, colorP, colorP, colorP);
    showLine(window, pointM1, pointM2, width, colorM, colorM, colorM);
    showLine(window, pointM3, pointM4, width, colorM, colorM, colorM);

    double current = i;
    lastoffsetVolt += current * currentScale;
    lastoffsetVolt = std::remainder(lastoffsetVolt, 20);
    lastoffsetVolt =
        lastoffsetVolt < 0 ? lastoffsetVolt + 20.0 : lastoffsetVolt;
    for (int i = 0; i < 5; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(pointP1.x - circle.getRadius(),
                         pointP1.y + 20 * i - circle.getRadius() +
                             lastoffsetVolt);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }
};

double lastoffsetNMOS = 0;
void showNMOS(sf::RenderWindow *window, double vg, double vd, double vs,
              double id, sf::Vector2f &loc, double currentScale) {
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

  double &current = id;
  // std::cout << "id:" << id << std::endl;
  lastoffsetNMOS += current * currentScale;
  lastoffsetNMOS = std::remainder(lastoffsetNMOS, 20);
  lastoffsetNMOS = lastoffsetNMOS < 0 ? lastoffsetNMOS + 20.0 : lastoffsetNMOS;

  sf::CircleShape circle(width / 2);
  for (int i = 0; i < 2; ++i) {
    circle.setPosition(pointG1.x - circle.getRadius() - 20 * i,
                       pointG1.y - circle.getRadius());
    circle.setFillColor(sf::Color(200, 200, 0));
    window->draw(circle);
  }

  for (int i = 0; i < 20; ++i) {
    if (i < 1)
      circle.setPosition(pointD1.x - circle.getRadius(),
                         pointD1.y - circle.getRadius() + lastoffsetNMOS);
    else if (i < 2) {
      if (lastoffsetNMOS < 5)
        circle.setPosition(pointD1.x - circle.getRadius(),
                           pointD1.y + 20 - circle.getRadius() +
                               lastoffsetNMOS);
      else
        circle.setPosition(pointD2.x - circle.getRadius() - lastoffsetNMOS + 5,
                           pointD2.y - circle.getRadius());
    } else if (i < 3) {
      circle.setPosition(pointD2.x - circle.getRadius() - lastoffsetNMOS - 15,
                         pointD2.y - circle.getRadius());
    } else if (i < 4) {
      if (lastoffsetNMOS < 15)
        circle.setPosition(pointD2.x - circle.getRadius() - lastoffsetNMOS - 35,
                           pointD2.y - circle.getRadius());
      else
        circle.setPosition(pointB1.x - circle.getRadius(),
                           pointB1.y - circle.getRadius() + lastoffsetNMOS -
                               15);
    } else if (i < 6) {
      circle.setPosition(pointB1.x - circle.getRadius(),
                         pointB1.y - circle.getRadius() + lastoffsetNMOS + 5 +
                             (i - 4) * 20);
    } else if (i < 7) {
      if (lastoffsetNMOS < 5)
        circle.setPosition(pointB1.x - circle.getRadius(),
                           pointB1.y - circle.getRadius() + lastoffsetNMOS +
                               45);
      else
        circle.setPosition(pointB2.x - circle.getRadius() + lastoffsetNMOS - 5,
                           pointB2.y - circle.getRadius());
    } else if (i < 8) {
      circle.setPosition(pointB2.x - circle.getRadius() + lastoffsetNMOS + 15,
                         pointB2.y - circle.getRadius());
    } else if (i < 9) {
      if (lastoffsetNMOS < 15)
        circle.setPosition(pointB2.x - circle.getRadius() + lastoffsetNMOS + 35,
                           pointB2.y - circle.getRadius());
      else
        circle.setPosition(pointS1.x - circle.getRadius(),
                           pointS1.y - circle.getRadius() - 15 +
                               lastoffsetNMOS);
    } else {
      circle.setPosition(pointS1.x - circle.getRadius(),
                         pointS1.y - circle.getRadius() + lastoffsetNMOS + 5);
    }
    circle.setFillColor(sf::Color(200, 200, 0));
    window->draw(circle);
  }
}

double lastoffsetresistor = 0;
void showResistor(sf::RenderWindow *window, double v1, double v2,
                  sf::Vector2f &loc, double R, double currentScale) {
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

  lastoffsetresistor += current * currentScale;
  lastoffsetresistor = std::remainder(lastoffsetresistor, 20);
  lastoffsetresistor =
      lastoffsetresistor < 0 ? lastoffsetresistor + 20.0 : lastoffsetresistor;
  for (int i = 0; i < 5; ++i) {
    sf::CircleShape circle(5.0 / 2);
    circle.setPosition(loc.x - circle.getRadius(), loc.y - 50 + 20 * i -
                                                       circle.getRadius() +
                                                       lastoffsetresistor);
    circle.setFillColor(sf::Color(200, 200, 0));
    window->draw(circle);
  }
}

class CircuitElement {
public:
  CircuitElement() {}
  virtual ~CircuitElement() {}
  virtual int getVoltageSourceCount() = 0;
  virtual void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v,
                             int MAX_NODE_ID, double t) = 0;
  virtual void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v,
                             int MAX_NODE_ID, double t) = 0;
};

class AdjustableVoltageSourceElement : public CircuitElement {
public:
  AdjustableVoltageSourceElement() : CircuitElement() {};
  ~AdjustableVoltageSourceElement() {};
  static std::unique_ptr<AdjustableVoltageSourceElement>
  create(std::function<double(double)> v, int pin1, int pin2,
         int voltageSourceID) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<AdjustableVoltageSourceElement> vsrc =
        std::make_unique<AdjustableVoltageSourceElement>();
    vsrc->v = v;
    vsrc->pin1 = pin1;
    vsrc->pin2 = pin2;
    vsrc->voltageSourceID = voltageSourceID;
    return std::move(vsrc);
  }

  double getV(double t) { return v(t); }
  int getVoltageSourceCount() override { return 1; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    if (pin1 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin1) = 1;
      g(pin1, MAX_NODE_ID + 1 + voltageSourceID) = 1;
    }

    if (pin2 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin2) = -1;
      g(pin2, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }

    // std::cout << "adjVoltageSourceGSuccess" << std::endl;
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    i(MAX_NODE_ID + 1 + voltageSourceID) = this->v(t);
    // std::cout << "adjVoltageSourceISuccess" << std::endl;
  }

private:
  std::function<double(double)> v;
  int pin1;
  int pin2;
  int voltageSourceID;
};

class VoltageSourceElement : public CircuitElement {
public:
  VoltageSourceElement() : CircuitElement() {}
  ~VoltageSourceElement() {}
  static std::unique_ptr<VoltageSourceElement>
  create(double v, int pin1, int pin2, int voltageSourceID) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<VoltageSourceElement> vsrc =
        std::make_unique<VoltageSourceElement>();
    vsrc->v = v;
    vsrc->pin1 = pin1;
    vsrc->pin2 = pin2;
    vsrc->voltageSourceID = voltageSourceID;
    return std::move(vsrc);
  }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    // std::cout << "modifying" << std::endl;
    // std::cout << "pin1" << pin1 << std::endl;
    // std::cout << "pin2" << pin2 << std::endl;
    // std::cout << "MAX_NODE_ID" << MAX_NODE_ID << std::endl;
    // std::cout << "voltageSourceID" << voltageSourceID << std::endl;
    if (pin1 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin1) = 1;
      // std::cout << "success1" << std::endl;
      g(pin1, MAX_NODE_ID + 1 + voltageSourceID) = 1;
      // std::cout << "success2" << std::endl;
    }

    if (pin2 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin2) = -1;
      g(pin2, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }

    // std::cout << "voltageSourceGSuccess" << std::endl;
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    i(MAX_NODE_ID + 1 + voltageSourceID) = this->v;

    // std::cout << "voltageSourceISuccess" << std::endl;
  }

  int getVoltageSourceCount() override { return 1; }

private:
  double v;
  int pin1;
  int pin2;
  int voltageSourceID;
};

class ResistorElement : public CircuitElement {
public:
  ResistorElement() : CircuitElement() {}
  ~ResistorElement() {}
  static std::unique_ptr<ResistorElement> create(double r, int pin1, int pin2) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<ResistorElement> rEle = std::make_unique<ResistorElement>();
    rEle->R = r;
    rEle->PIN1 = pin1;
    rEle->PIN2 = pin2;
    return std::move(rEle);
  }

  int getVoltageSourceCount() override { return 0; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    double g0 = 1.0 / R;
    if (PIN1 != -1)
      g(PIN1, PIN1) += g0;
    if (PIN2 != -1)
      g(PIN2, PIN2) += g0;
    if (PIN1 != -1 && PIN2 != -1) {
      g(PIN1, PIN2) += -g0;
      g(PIN2, PIN1) += -g0;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {}

private:
  double R;
  int PIN1;
  int PIN2;
};

class NMOSElement : public CircuitElement {
public:
  NMOSElement() : CircuitElement() {}
  ~NMOSElement() {}
  static std::unique_ptr<NMOSElement> create(double K, double VA, double VT,
                                             int PIN_G, int PIN_D, int PIN_S) {
    std::unique_ptr<NMOSElement> nmosEle = std::make_unique<NMOSElement>();
    nmosEle->PIN_G = PIN_G;
    nmosEle->PIN_D = PIN_D;
    nmosEle->PIN_S = PIN_S;
    nmosEle->K = K;
    nmosEle->VA = VA;
    nmosEle->VT = VT;
    return std::move(nmosEle);
  }

  int getVoltageSourceCount() override { return 0; }

  double getId(Eigen::MatrixXd &vResult) {
    double vg, vd, vs;
    if (PIN_G == -1) {
      vg = 0;
    } else {
      vg = vResult(PIN_G);
    }

    if (PIN_D == -1) {
      vd = 0;
    } else {
      vd = vResult(PIN_D);
    }

    if (PIN_S == -1) {
      vs = 0;
    } else {
      vs = vResult(PIN_S);
    }
    return calid(K, vg - vs, vd - vs, VT, VA);
  }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    // g0
    double vg, vd, vs;
    if (PIN_G == -1) {
      vg = 0;
    } else {
      vg = v(PIN_G);
    }

    if (PIN_D == -1) {
      vd = 0;
    } else {
      vd = v(PIN_D);
    }

    if (PIN_S == -1) {
      vs = 0;
    } else {
      vs = v(PIN_S);
    }

    double g0 = calg0(K, vg - vs, vd - vs, VT, VA);

    if (PIN_D != -1)
      g(PIN_D, PIN_D) += g0;

    if (PIN_S != -1)
      g(PIN_S, PIN_S) += g0;

    if (PIN_D != -1 && PIN_S != -1) {
      g(PIN_D, PIN_S) += -g0;
      g(PIN_S, PIN_D) += -g0;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    double vg, vd, vs;
    if (PIN_G == -1) {
      vg = 0;
    } else {
      vg = v(PIN_G);
    }

    if (PIN_D == -1) {
      vd = 0;
    } else {
      vd = v(PIN_D);
    }

    if (PIN_S == -1) {
      vs = 0;
    } else {
      vs = v(PIN_S);
    }

    double ideq = calideq(K, vg - vs, vd - vs, VT, VA);
    double issd = calissd(K, vg - vs, vd - vs, VT, VA);

    if (PIN_D != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_D) += -ideq;
      i(PIN_D) += -issd;
    }

    if (PIN_S != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_S) += ideq;
      i(PIN_S) += issd;
    }
  }

private:
  int PIN_G;
  int PIN_D;
  int PIN_S;
  double K;
  double VA;
  double VT;

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

  double calissd(double k, double vgs, double vds, double vt, double va) {
    return calgm(k, vgs, vds, vt, va) * vgs;
  }

  double calideq(double k, double vgs, double vds, double vt, double va) {
    return calid(k, vgs, vds, vt, va) - calissd(k, vgs, vds, vt, va) -
           calg0(k, vgs, vds, vt, va) * vds;
  }
};

class Circuit {
public:
  static std::unique_ptr<Circuit> create(std::vector<CircuitElement *> elements,
                                         double deltaT, int MAX_NODE_ID) {
    std::unique_ptr<Circuit> circuit = std::make_unique<Circuit>();
    circuit->elements = elements;
    circuit->t = 0;
    circuit->deltaT = deltaT;
    circuit->MAX_NODE_ID = MAX_NODE_ID;

    circuit->MAX_MATRIX_SIZE = MAX_NODE_ID + 1; // node starts from 0
    for (CircuitElement *ele : elements) {
      circuit->MAX_MATRIX_SIZE += ele->getVoltageSourceCount();
    }

    circuit->v.resize(circuit->MAX_MATRIX_SIZE, 1);
    circuit->v.setZero();
    circuit->g.resize(circuit->MAX_MATRIX_SIZE, circuit->MAX_MATRIX_SIZE);
    circuit->g.setZero();
    circuit->i.resize(circuit->MAX_MATRIX_SIZE, 1);
    circuit->i.setZero();
    // std::cout << "MAX_MATRIX_SIZE: " << circuit->MAX_MATRIX_SIZE << std::endl;

    for (CircuitElement *ele : elements) {
      // g matrix
      ele->modifyGMatrix(circuit->g, circuit->v, circuit->MAX_NODE_ID, 0);
      // i matrix
      ele->modifyIMatrix(circuit->i, circuit->v, circuit->MAX_NODE_ID, 0);
      // std::cout << "element success" << std::endl;
    }

    return std::move(circuit);
  }

  void incTimerByDeltaT() {
    t += deltaT;
  }
  void iterate() {
    g.resize(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    g.setZero();
    i.resize(MAX_MATRIX_SIZE, 1);
    i.setZero();
    // std::cout << "MAX_MATRIX_SIZE: " << MAX_MATRIX_SIZE << std::endl;

    for (CircuitElement *ele : elements) {
      // g matrix
      ele->modifyGMatrix(g, v, MAX_NODE_ID, t);
      // i matrix
      ele->modifyIMatrix(i, v, MAX_NODE_ID, t);
      // std::cout << "element success" << std::endl;
    }


    // std::cout << "g: " << std::endl;
    // std::cout << g << std::endl;
    // std::cout << "v: " << std::endl;
    // std::cout << v << std::endl;
    // std::cout << "i: " << std::endl;
    // std::cout << i << std::endl;
    // std::cout << "Start ITERATE: " << std::endl;
    // F matrix
    Eigen::MatrixXd f(MAX_MATRIX_SIZE, 1);
    f = g * v - i;

    // std::cout << "f matrix success" << std::endl;
    // J matrix
    double delta = 0.00000001;
    Eigen::MatrixXd dupV = v.replicate(1, MAX_MATRIX_SIZE);
    Eigen::MatrixXd vWithDelta =
        Eigen::MatrixXd::Identity(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
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
    // std::cout << "iteration: " << "idk" << std::endl;
    // std::cout << "V0\nV1\nIx\nIY:" << std::endl;
    // std::cout << v << std::endl;
    // std::cout << "===============" << std::endl;
    // std::cout << "evaluation:" << std::endl;
    // std::cout << g * v - i << std::endl;
    // std::cout << "===============" << std::endl;
  }

  double getVoltage(int PIN_ID) { return v(PIN_ID); }
  Eigen::MatrixXd &getVoltageMatrix() { return v; }

  double getTime() { return t; }

private:
  std::vector<CircuitElement *> elements;
  Eigen::MatrixXd v;
  Eigen::MatrixXd g;
  Eigen::MatrixXd i;
  double t;
  double deltaT;
  int MAX_NODE_ID;
  int MAX_MATRIX_SIZE;
};

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

  // vg
  std::function<double(double)> vgsOfT = [](double t) {
    // return 13.3 * pow(10, -3) * sin(5 * t) + 0.6;
    return sin(t) + 0.6;
  };

  std::unique_ptr<AdjustableVoltageSourceElement> adjustVsrc =
      AdjustableVoltageSourceElement::create(vgsOfT, 0, -1, 0);
  // voltage source from vd
  std::unique_ptr<VoltageSourceElement> vsrc =
      VoltageSourceElement::create(1.8, 2, -1, 1);

  // nmos stats
  double K = 4 * pow(10, -3);
  double VT = 0.4;
  // double va = std::numeric_limits<double>::infinity();
  double VA = INFINITY;
  std::unique_ptr<NMOSElement> nmos = NMOSElement::create(K, VA, VT, 0, 1, -1);

  // resistor format
  double r0 = 17.5 * pow(10, 3);
  std::unique_ptr<ResistorElement> resistor = ResistorElement::create(r0, 2, 1);

  std::vector<CircuitElement *> elements;
  elements.push_back(vsrc.get());
  elements.push_back(adjustVsrc.get());
  elements.push_back(nmos.get());
  elements.push_back(resistor.get());

  std::unique_ptr<Circuit> circuit = Circuit::create(elements, 0.01, 2);
  Wire wire;

  AdjustableVoltageSource sourceG;
  VoltageSource sourceD;

  while (window.isOpen()) {

    circuit->incTimerByDeltaT();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // std::cout << "*******************************" << std::endl;
    // std::cout << "Time:" << circuit->getTime() << std::endl;

    // if (circuit->getTime() >= 0.03)
    //   window.close();

    for (int iteration = 0; iteration < 10; iteration++) {
      // std::cout << "========================" << std::endl;
      // std::cout << "iteration: " << iteration << std::endl;

      circuit->iterate();
    }

    text.setString("vg: " + std::to_string(circuit->getVoltage(0)) + " | vd: " +
                   std::to_string(circuit->getVoltage(1)) + " | i: " +
                   std::to_string(nmos->getId(circuit->getVoltageMatrix())) +
                   " | t: " + std::to_string(circuit->getTime()));
    window.clear(sf::Color::Black);
    sf::Vector2f nmosLoc(200, 300);
    sf::Vector2f nmosGroundLoc(200, 350);
    sf::Vector2f voltGateLoc(100, 350);
    sf::Vector2f voltGateGroundLoc(100, 400);
    sf::Vector2f resisLoc(200, 200);
    sf::Vector2f voltLoc(500, 250);
    sf::Vector2f voltGroundLoc(500, 300);
    sf::Vector2f wire1Loc(200, 150);
    sf::Vector2f wire2Loc(500, 200);
    double currentScale = 5000;
    showNMOS(&window, circuit->getVoltage(0), circuit->getVoltage(1), 0,
             nmos->getId(circuit->getVoltageMatrix()), nmosLoc, currentScale);
    showResistor(&window, circuit->getVoltage(2), circuit->getVoltage(1),
                 resisLoc, r0, currentScale);
    sourceD.showVoltageSource(&window, circuit->getVoltage(2), 0,
                              circuit->getVoltage(4), voltLoc, currentScale);
    sourceG.showAdjustableVoltageSource(&window, circuit->getVoltage(0), 0,
                                        circuit->getVoltage(3), voltGateLoc,
                                        currentScale);
    showGround(&window, voltGateGroundLoc);
    showGround(&window, voltGroundLoc);
    showGround(&window, nmosGroundLoc);
    wire.showWire(&window, wire1Loc, wire2Loc, circuit->getVoltage(2),
                  circuit->getVoltage(4), currentScale);
    window.draw(text);
    window.display();
  }

  // delete bgD;

  return 0;
}
