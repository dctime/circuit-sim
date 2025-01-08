#include <UICircuit.h>
#include <UIElement.h>

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

  
  // UI only
  UICircuit uiCircuit;

  std::unique_ptr<UIElement> gnd1 =
      std::make_unique<GroundUIElement>(&uiCircuit, 4, 5);
  std::unique_ptr<UIElement> gnd2 =
      std::make_unique<GroundUIElement>(&uiCircuit, 5, 9);
  std::unique_ptr<UIElement> gnd3 =
      std::make_unique<GroundUIElement>(&uiCircuit, 7, 9);

  std::unique_ptr<UIElement> wire1 =
      std::make_unique<WireUIElement>(&uiCircuit, 4, 3, 5, 3);
  std::unique_ptr<UIElement> wire2 =
      std::make_unique<WireUIElement>(&uiCircuit, 5, 3, 7, 3);
  std::unique_ptr<UIElement> wire3 =
      std::make_unique<WireUIElement>(&uiCircuit, 5, 5, 5, 6);
  std::unique_ptr<UIElement> wire4 =
      std::make_unique<WireUIElement>(&uiCircuit, 5, 6, 5, 7);

  std::function<double(double)> vsrc = [](double t) { return 10*sin(t); };
  std::unique_ptr<UIElement> source =
      std::make_unique<AdjustableVoltageSourceUIElement>(&uiCircuit, 4, 4, vsrc);
  double r1 = 10000000;
  std::unique_ptr<UIElement> resistor1 =
      std::make_unique<ResistorUIElement>(&uiCircuit, 5, 4, r1);
  std::unique_ptr<UIElement> resistor2 =
      std::make_unique<ResistorUIElement>(&uiCircuit, 5, 8, r1);
  double r2 = 6000;
  std::unique_ptr<UIElement> resistor3 =
      std::make_unique<ResistorUIElement>(&uiCircuit, 7, 4, r2);
  std::unique_ptr<UIElement> resistor4 =
      std::make_unique<ResistorUIElement>(&uiCircuit, 7, 8, r2);
  double K = 1 * pow(10, -3);
  double VT = 1;
  // double va = std::numeric_limits<double>::infinity();
  double VA = INFINITY;
  std::unique_ptr<UIElement> nmosUI =
      std::make_unique<NMOSUIElement>(&uiCircuit, 7, 6, K, VT, VA);

  uiCircuit.addElement(gnd1);
  uiCircuit.addElement(gnd2);
  uiCircuit.addElement(gnd3);
  uiCircuit.addElement(wire1);
  uiCircuit.addElement(wire2);
  uiCircuit.addElement(wire3);
  uiCircuit.addElement(wire4);
  uiCircuit.addElement(source);
  uiCircuit.addElement(resistor1);
  uiCircuit.addElement(resistor2);
  uiCircuit.addElement(resistor3);
  uiCircuit.addElement(resistor4);
  uiCircuit.addElement(nmosUI);

  sf::Vector2i mouseGridPos;
  sf::Vector2i mousePos;
  bool mousePressed = false;

  while (window.isOpen()) {
    uiCircuit.runCircuit();

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
      std::cout << "Left Mouse Button Pressed" << std::endl;
      // leftMouseButtonPressed(mouseGridPos.x, mouseGridPos.y, uiElements,
      //                        circuit.get(), &currentScale);
      mousePressed = true;
    }

    // std::cout << "*******************************" << std::endl;
    // std::cout << "Time:" << circuit->getTime() << std::endl;

    // if (circuit->getTime() >= 0.03)
    //   window.close();

    window.clear(sf::Color::Black);

    sf::Color gridColor = sf::Color(30, 30, 30);
    showGrid(window, gridColor);

    // TODO: need UICircuit class to decide the pin numbers when circuit updates
    // and passes to the elements. circuit needs to be stopped and restarted.
    //
    // resistor follows the cursor
    ResistorUIElement::showGhostElement(&window, mouseGridPos.x,
                                        mouseGridPos.y);

    uiCircuit.showCircuit(&window);

    for (size_t i = 0; i < buttons.size(); i++) {
      buttons[i].render(&window);
      buttons[i].update(sf::Vector2f(sf::Mouse::getPosition(window)));
    }

    NMOSUIElement *nmosUIElement = (NMOSUIElement *)uiCircuit.getUIElement(12);
    text.setString(
        "vg: " + std::to_string(nmosUIElement->getShownPinGVolt()) +
        " | vd: " + std::to_string(nmosUIElement->getShownPinDVolt()) +
        " | vs: " + std::to_string(nmosUIElement->getShownPinSVolt()) +
        " | i: " + std::to_string(nmosUIElement->getShownId()) +
        " | t: " + std::to_string(uiCircuit.getTime()));
    window.draw(text);

    window.display();
  }

  // delete bgD;

  return 0;
}

void leftMouseButtonPressed(int xGrid, int yGrid,
                            std::vector<std::unique_ptr<UIElement>> &uiElements,
                            Circuit *circuit, double *currentScale) {
  // std::unique_ptr<UIElement> resistor = std::make_unique<ResistorUIElement>(
  //     &TEST_DOUBLE, &TEST_DOUBLE, 1000, currentScale, xGrid, yGrid);
  // uiElements.push_back(std::move(resistor));
}
