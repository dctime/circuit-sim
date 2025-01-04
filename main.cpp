#include <SFML/Graphics.hpp> 
#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include <Circuit.h>
#include <AdjustableVoltageSource.h>
#include <VoltageSource.h>
#include <NMOSElement.h>
#include <ResistorElement.h>
#include <showNMOS.h>
#include <wire.h>
#include <AdjustableVoltageSource.h>
#include <AdjustableVoltageSourceElement.h>
#include <VoltageSourceElement.h>
#include <showResistor.h>
#include <button.h>
int main() {
  sf::Font font;
  if (!font.loadFromFile("../arial.ttf")) {
    std::cout << "Font Load Failed!" << std::endl;
  }
  sf::RenderWindow window(sf::VideoMode(800, 600), "X-Y Coordinate Graph");
  window.setFramerateLimit(60);
  std::vector<Button> buttons;
  for(int i=0;i<5;i++)
  {
    buttons.push_back(Button(100*i+10*i, 550, 100, 50, &font, "Button", sf::Color::Red, sf::Color::Green, sf::Color::Blue));
  }
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
    for(size_t i=0;i<buttons.size();i++)
    {
      buttons[i].render(&window);
      buttons[i].update(sf::Vector2f(sf::Mouse::getPosition(window)));
    }
    window.draw(text);
    window.display();
  }
  

  // delete bgD;

  return 0;
}
