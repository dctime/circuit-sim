#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <UICircuit.h>
#include <UIElement.h>

#include <AdjustableVoltageSourceElement.h>
#include <AdjustableVoltageSourceUIElement.h>
#include <Circuit.h>
int Circuit::circuitCounter = 0;
#include <CapacitorUIElement.h>
#include <CurrentSourceUIElement.h>
#include <DiodeUIElement.h>
#include <GroundUIElement.h>
#include <InductorUIElement.h>
#include <NMOSElement.h>
#include <NMOSUIElement.h>
#include <PMOSUIElement.h>
#include <ResistorElement.h>
#include <ResistorUIElement.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <VoltageSourceElement.h>
#include <VoltageSourceUIElement.h>
#include <WireUIElement.h>
#include <button.h>
#include <chrono>
#include <cmath>
#include <eigen3/Eigen/src/Core/products/Parallelizer.h>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <queue>
double TEST_DOUBLE = 0;

int xloc1 = -1;
int xloc2 = -1;
int yloc1 = -1;
int yloc2 = -1;

double K = 5 * pow(10, -2);
double VT = 0.4;
// double va = std::numeric_limits<double>::infinity();
double VA = INFINITY;

void leftMouseButtonPressedHolding(int xGrid, int yGrid) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    xloc2 = xGrid;
    yloc2 = yGrid;
  }
}

void leftMouseButtonPressedNegativeEdge(int xGrid, int yGrid,
                                        UICircuit *circuit) {
  if (xloc1 != -1 && xloc2 != -1 && yloc1 != -1 && yloc2 != -1 &&
      !(xloc1 == xloc2 && yloc1 == yloc2)) {
    std::unique_ptr<UIElement> wire =
        std::make_unique<WireUIElement>(circuit, xloc1, yloc1, xloc2, yloc2);
    circuit->addElement(wire, xGrid, yGrid);
  }
  xloc1 = -1;
  xloc2 = -1;
  yloc1 = -1;
  yloc2 = -1;
}

void leftMouseButtonPressedEdge(int xGrid, int yGrid, UICircuit *circuit) {
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
    std::unique_ptr<UIElement> resistor =
        std::make_unique<ResistorUIElement>(circuit, xGrid, yGrid, 1000);
    circuit->addElement(resistor, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
    std::unique_ptr<UIElement> gnd =
        std::make_unique<GroundUIElement>(circuit, xGrid, yGrid);
    circuit->addElement(gnd, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    xloc1 = xGrid;
    yloc1 = yGrid;
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    circuit->startCircuit();
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
    std::unique_ptr<UIElement> nmos =
        std::make_unique<NMOSUIElement>(circuit, xGrid, yGrid, K, VT, VA);
    circuit->addElement(nmos, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
    double KP = 5.5555556 * pow(10, -3);
    double VTP = 0.4;
    std::unique_ptr<UIElement> pmos =
        std::make_unique<PMOSUIElement>(circuit, xGrid, yGrid, K, VT, VA);
    circuit->addElement(pmos, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
    double Is = 6.9 * pow(10, -16);
    double VTD = 0.025;
    std::unique_ptr<UIElement> diode =
        std::make_unique<DiodeUIElement>(circuit, xGrid, yGrid, Is, VTD);
    circuit->addElement(diode, xGrid, yGrid);

  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
    std::unique_ptr<UIElement> voltage =
        std::make_unique<VoltageSourceUIElement>(circuit, xGrid, yGrid, 5);
    circuit->addElement(voltage, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    std::function<double(double)> v = [](double t) { 
      if ((int)(t*1000000)%2 == 1) {
        return 10;
      } else {
        return 0;
      }
    };
    std::unique_ptr<UIElement> voltage =
        std::make_unique<AdjustableVoltageSourceUIElement>(circuit, xGrid,
                                                           yGrid, v);
    circuit->addElement(voltage, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) &&
             !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
    double i = 1;
    std::unique_ptr<UIElement> current =
        std::make_unique<CurrentSourceUIElement>(circuit, xGrid, yGrid, i);
    circuit->addElement(current, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
    std::unique_ptr<UIElement> capacitor =
        std::make_unique<CapacitorUIElement>(circuit, xGrid, yGrid, 0.0001);
    circuit->addElement(capacitor, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) &&
             sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
    std::unique_ptr<UIElement> inductor =
        std::make_unique<InductorUIElement>(circuit, xGrid, yGrid, 0.0001);
    circuit->addElement(inductor, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
    std::unique_ptr<UIElement> bigResistor =
        std::make_unique<ResistorUIElement>(circuit, xGrid, yGrid,
                                            1000000000000);
    circuit->addElement(bigResistor, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
    std::unique_ptr<UIElement> r800 =
        std::make_unique<ResistorUIElement>(circuit, xGrid, yGrid, 800);
    circuit->addElement(r800, xGrid, yGrid);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
    std::unique_ptr<UIElement> v18 =
        std::make_unique<VoltageSourceUIElement>(circuit, xGrid, yGrid, 1.8);
    circuit->addElement(v18, xGrid, yGrid);
  }
  circuit->printOutLocToUIElementID();
}

void rightMouseButtonPressedHolding(int xGrid, int yGrid) {}

void rightMouseButtonPressedNegativeEdge(int xGrid, int yGrid,
                                        UICircuit *circuit) {}

void rightMouseButtonPressedEdge(int xGrid, int yGrid, UICircuit *circuit) {
  circuit->deleteElement(circuit->getUiElementIDFromLoc(xGrid, yGrid));
  circuit->printOutLocToUIElementID();
}

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
  // use half the threads
  Eigen::setNbThreads(std::thread::hardware_concurrency());
  std::cout << "Threads: " << Eigen::nbThreads() << std::endl;

  sf::Font font;
  if (!font.loadFromFile("../arial.ttf")) {
    std::cout << "Font Load Failed!" << std::endl;
  }
  sf::RenderWindow window(sf::VideoMode(800, 600), "X-Y Coordinate Graph");
  window.setFramerateLimit(60);
  std::vector<Button> buttons;
  for (int i = 0; i < 5; i++) {
    buttons.push_back(Button(50 * i + 5 * i, window.getSize().y - 50, 50, 50,
                             &font, "Button", sf::Color::Red, sf::Color::Green,
                             sf::Color::Blue));
  }
  Button startButton(window.getSize().x - 55, 5, 50, 50, &font, "Start",
                     sf::Color::Red, sf::Color::Green, sf::Color::Blue);
  Button endButton(window.getSize().x - 55, 55, 50, 50, &font, "End",
                   sf::Color::Red, sf::Color::Green, sf::Color::Blue);

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

  UICircuit uiCircuit;
  // UI only

  std::cout << "UICircuit Size: " << sizeof(uiCircuit) << std::endl;

 
  sf::Vector2i mouseGridPos;
  sf::Vector2i mousePos;

  bool leftMousePressed = false;
  bool rightMousePressed = false;

  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point end =
      std::chrono::high_resolution_clock::now();
  float fps;

  window.setFramerateLimit(1000);
  // simulationSpeed 0.001 - 1
  double simulationSpeed = 1;
  int simulationSpeedCounter = 0;
  while (window.isOpen()) {
    // Performed. Now perform GPU stuff...
    end = std::chrono::high_resolution_clock::now();
    fps =
        (float)1e9 /
        (float)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    start = std::chrono::high_resolution_clock::now();

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
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window.setView(sf::View(visibleArea));
        // Update button positions
        for (int i = 0; i < buttons.size(); i++) {
          buttons[i].setposition(50 * i + 5 * i, window.getSize().y - 50);
        }
        startButton.setposition(window.getSize().x - 55, 50);
        endButton.setposition(window.getSize().x - 55, 105);
      }
    }

    // left mouse button
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      if (leftMousePressed) {
        leftMouseButtonPressedNegativeEdge(mouseGridPos.x, mouseGridPos.y,
                                           &uiCircuit);
      }
      leftMousePressed = false;
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !leftMousePressed &&
               mouseGridPos.x != -1 && mouseGridPos.y != -1) {
      std::cout << "Left Mouse Button Pressed" << std::endl;
      leftMouseButtonPressedEdge(mouseGridPos.x, mouseGridPos.y, &uiCircuit);
      leftMousePressed = true;
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && leftMousePressed &&
               mouseGridPos.x != -1 && mouseGridPos.y != -1) {
      leftMouseButtonPressedHolding(mouseGridPos.x, mouseGridPos.y);
    }

    // right mouse button
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      if (rightMousePressed) {
        rightMouseButtonPressedNegativeEdge(mouseGridPos.x, mouseGridPos.y,
                                           &uiCircuit);
      }
      rightMousePressed = false;
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !rightMousePressed &&
               mouseGridPos.x != -1 && mouseGridPos.y != -1) {
      std::cout << "Right Mouse Button Pressed" << std::endl;
      rightMouseButtonPressedEdge(mouseGridPos.x, mouseGridPos.y, &uiCircuit);
      rightMousePressed = true;
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && rightMousePressed &&
               mouseGridPos.x != -1 && mouseGridPos.y != -1) {
      rightMouseButtonPressedHolding(mouseGridPos.x, mouseGridPos.y);
    }

    window.clear(sf::Color::Black);

    sf::Color gridColor = sf::Color(30, 30, 30);
    showGrid(window, gridColor);

    if (simulationSpeedCounter >= 1 / simulationSpeed) {
      simulationSpeedCounter = 0;
      uiCircuit.showCircuit(&window, true);
    } else {
      simulationSpeedCounter++;
      uiCircuit.showCircuit(&window, false);
    }

    double wireIndicatorWidth = 3;
    if (xloc1 != -1 && yloc1 != -1) {
      sf::CircleShape loc1Circle(wireIndicatorWidth);
      loc1Circle.setFillColor(sf::Color(255, 0, 0));
      sf::Vector2f loc1(xloc1 * 50 - wireIndicatorWidth / 2,
                        yloc1 * 50 - wireIndicatorWidth / 2);
      loc1Circle.setPosition(loc1);
      window.draw(loc1Circle);
    }

    if (xloc2 != -1 && yloc2 != -1) {
      sf::CircleShape loc2Circle(wireIndicatorWidth);
      loc2Circle.setFillColor(sf::Color(255, 0, 0));
      sf::Vector2f loc2(xloc2 * 50 - wireIndicatorWidth / 2,
                        yloc2 * 50 - wireIndicatorWidth / 2);
      loc2Circle.setPosition(loc2);
      window.draw(loc2Circle);
    }

    for (size_t i = 0; i < buttons.size(); i++) {
      buttons[i].render(&window);
      buttons[i].update(sf::Vector2f(sf::Mouse::getPosition(window)));
    }
    startButton.render(&window);
    startButton.update(sf::Vector2f(sf::Mouse::getPosition(window)));
    endButton.render(&window);
    endButton.update(sf::Vector2f(sf::Mouse::getPosition(window)));
    // NMOSUIElement *nmos1UIElement = (NMOSUIElement
    // *)uiCircuit.getUIElement(0); NMOSUIElement *nmos2UIElement =
    // (NMOSUIElement *)uiCircuit.getUIElement(1);
    // text.setString(
    //     "vg: " + std::to_string(nmosUIElement->getShownPinGVolt()) +
    //     " | vd: " + std::to_string(nmosUIElement->getShownPinDVolt()) +
    //     " | vs: " + std::to_string(nmosUIElement->getShownPinSVolt()) +
    //     " | i: " + std::to_string(nmosUIElement->getShownId()) +
    //     " | t: " + std::to_string(uiCircuit.getTime()));

    text.setString(
        "FPS: " + std::to_string(fps) +
        // "|id copy: " + std::to_string(nmos1UIElement->getShownId()) +
        // "|id: " + std::to_string(nmos2UIElement->getShownId()) +
        // "|vg control: " + std::to_string(nmos2UIElement->getShownPinGVolt())
        // +
        // "|vd dest: " + std::to_string(nmos1UIElement->getShownPinDVolt()) +
        "|t: " + std::to_string(uiCircuit.getTime()));

    window.draw(text);

    window.display();
    // window.draw, etc.
  }

  // delete bgD;

  return 0;
}
