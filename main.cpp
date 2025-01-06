#include "UIElement.h"
#include <AdjustableVoltageSourceElement.h>
#include <AdjustableVoltageSourceUIElement.h>
#include <Circuit.h>
#include <GroundUIElement.h>
#include <NMOSElement.h>
#include <NMOSUIElement.h>
#include <ResistorElement.h>
#include <ResistorUIElement.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <VoltageSourceElement.h>
#include <VoltageSourceUIElement.h>
#include <WireUIElement.h>
#include <button.h>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

double TEST_DOUBLE = 0;

void leftMouseButtonPressed(int xGrid, int yGrid,
                            std::vector<std::unique_ptr<UIElement>> &uiElements,
                            Circuit *circuit, double *currentScale);

void showGrid(sf::RenderWindow &window, sf::Color &color) {
  for (int x = 0; x <= window.getSize().x; x += 50) {
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(x, 0), color),
        sf::Vertex(sf::Vector2f(x, window.getSize().y), color)};

    window.draw(line, 2, sf::Lines);
  }

  for (int y = 0; y <= window.getSize().y; y += 50) {
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(0, y), color),
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
  std::vector<Button> buttons;
  for (int i = 0; i < 5; i++) {
    buttons.push_back(Button(50 * i + 5 * i, 550, 50, 50, &font, "Button",
                             sf::Color::Red, sf::Color::Green,
                             sf::Color::Blue));
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

  // UI only
  double currentScale = 5000;
  double GROUND = 0;

  std::unique_ptr<UIElement> gnd1 = std::make_unique<GroundUIElement>(2, 8);
  std::unique_ptr<UIElement> gnd2 = std::make_unique<GroundUIElement>(10, 6);
  std::unique_ptr<UIElement> gnd3 = std::make_unique<GroundUIElement>(4, 7);

  std::unique_ptr<UIElement> wire = std::make_unique<WireUIElement>(
      circuit->getVoltagePointer(2), circuit->getVoltagePointer(4),
      &currentScale, 4, 3, 10, 4);
  std::unique_ptr<UIElement> sourceG =
      std::make_unique<AdjustableVoltageSourceUIElement>(
          circuit->getVoltagePointer(0), &GROUND, circuit->getVoltagePointer(3),
          &currentScale, 2, 7);

  std::unique_ptr<UIElement> sourceD = std::make_unique<VoltageSourceUIElement>(
      circuit->getVoltagePointer(2), &GROUND, circuit->getVoltagePointer(4),
      &currentScale, 10, 5);

  std::unique_ptr<UIElement> resistorDrain =
      std::make_unique<ResistorUIElement>(circuit->getVoltagePointer(2),
                                          circuit->getVoltagePointer(1), r0,
                                          &currentScale, 4, 4);

  std::unique_ptr<UIElement> nmosUI = std::make_unique<NMOSUIElement>(
      circuit->getVoltagePointer(0), circuit->getVoltagePointer(1), &GROUND,
      nmos.get(), circuit.get(), &currentScale, 4, 6);

  std::vector<std::unique_ptr<UIElement>> uiElements;

  uiElements.push_back(std::move(wire));
  uiElements.push_back(std::move(sourceG));
  uiElements.push_back(std::move(sourceD));
  uiElements.push_back(std::move(resistorDrain));
  uiElements.push_back(std::move(nmosUI));
  uiElements.push_back(std::move(gnd1));
  uiElements.push_back(std::move(gnd2));
  uiElements.push_back(std::move(gnd3));

  sf::Vector2i mouseGridPos;
  sf::Vector2i mousePos;
  bool mousePressed = false;

  while (window.isOpen()) {

    circuit->incTimerByDeltaT();

    // mousePos, mouseGridPos update
    mousePos = sf::Mouse::getPosition(window);

    if (mousePos.x < 0 || mousePos.x >= window.getSize().x) {
      mousePos.x = -1;
      mouseGridPos.x = -1;
    } else {
      if (mousePos.x % 50 >= 25) {
        mouseGridPos.x = mousePos.x / 50 + 1;
      } else {
        mouseGridPos.x = mousePos.x / 50;
      }
    }

    if (mousePos.y < 0 || mousePos.y >= window.getSize().y) {
      mousePos.y = -1;
      mouseGridPos.y = -1;
    } else {
      if (mousePos.y % 50 >= 25) {
        mouseGridPos.y = mousePos.y / 50 + 1;
      } else {
        mouseGridPos.y = mousePos.y / 50;
      }
    }

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      mousePressed = false;
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressed &&
               mouseGridPos.x != -1 && mouseGridPos.y != -1) {
      leftMouseButtonPressed(mouseGridPos.x, mouseGridPos.y, uiElements,
                             circuit.get(), &currentScale);
      mousePressed = true;
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

    // TODO: ground and wire doesnt fit into UI Elements and Circuit Elements
    // need ground and wire UIElement classes
    // TODO: need UICircuit class to decide the pin numbers when circuit updates
    // and passes to the elements. circuit needs to be stopped and restarted.
    //
    // resistor follows the cursor
    ResistorUIElement::showGhostElement(&window, mouseGridPos.x,
                                        mouseGridPos.y);

    // TODO: UI Elements should use Circuit Elements PIN num to get data from
    // circuit
    // TODO: UI Elements should store the representing circuit element and put
    // circuit elements into main circuits when build
    // TODO: Wires should get data from connected UIElements
    // TODO: Ground only need to be rendered
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      uiElement->showElement(&window);
    }

    for (size_t i = 0; i < buttons.size(); i++) {
      buttons[i].render(&window);
      buttons[i].update(sf::Vector2f(sf::Mouse::getPosition(window)));
    }
    window.draw(text);

    window.display();
  }

  // delete bgD;

  return 0;
}

void leftMouseButtonPressed(int xGrid, int yGrid,
                            std::vector<std::unique_ptr<UIElement>> &uiElements,
                            Circuit *circuit, double *currentScale) {
  std::unique_ptr<UIElement> resistor = std::make_unique<ResistorUIElement>(
      &TEST_DOUBLE, &TEST_DOUBLE, 1000, currentScale, xGrid, yGrid);
  uiElements.push_back(std::move(resistor));
}
