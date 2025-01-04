#include <AdjustableVoltageSource.h>
#include <AdjustableVoltageSourceElement.h>
#include <Circuit.h>
#include <NMOSElement.h>
#include <ResistorElement.h>
#include <SFML/Graphics.hpp>
#include <VoltageSource.h>
#include <VoltageSourceElement.h>
#include <cmath>
#include <functional>
#include <iostream>
#include <showNMOS.h>
#include <showResistor.h>
#include <vector>
#include <wire.h>

void showGrid(sf::RenderWindow& window, sf::Color& color) {
  for (int x = 0; x <= window.getSize().x; x += 50) {
      sf::Vertex line[] = {sf::Vertex(sf::Vector2f(x, 0), color),
                           sf::Vertex(sf::Vector2f(x, window.getSize().y), color)};

      window.draw(line, 2, sf::Lines);
    }

    for (int y = 0; y <= window.getSize().y; y += 50) {
      sf::Vertex line[] = {sf::Vertex(sf::Vector2f(0, y), color),
                           sf::Vertex(sf::Vector2f(window.getSize().x, y), color)};

      window.draw(line, 2, sf::Lines);
    }
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

    sf::Color gridColor = sf::Color(30, 30, 30);
    showGrid(window, gridColor);
    sf::Vector2f voltGateLoc(100, 350);
    double currentScale = 5000;
    showNMOS(&window, circuit->getVoltage(0), circuit->getVoltage(1), 0,
             nmos->getId(circuit->getVoltageMatrix()), 4, 6, currentScale);
    showResistor(&window, circuit->getVoltage(2), circuit->getVoltage(1),
                 4, 4, r0, currentScale);
    sourceD.showVoltageSource(&window, circuit->getVoltage(2), 0,
                              circuit->getVoltage(4), 10, 5, currentScale);
    sourceG.showAdjustableVoltageSource(&window, circuit->getVoltage(0), 0,
                                        circuit->getVoltage(3), 2, 7,
                                        currentScale);
    showGround(&window, 2, 8);
    showGround(&window, 10, 6);
    showGround(&window, 4, 7);
    wire.showWire(&window, 4, 3, 10, 4, circuit->getVoltage(2),
                  circuit->getVoltage(4), currentScale);
    window.draw(text);

    

    window.display();
  }

  // delete bgD;

  return 0;
}
