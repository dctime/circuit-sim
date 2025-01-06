#include <UIElement.h>
#include <Line.h>

class GroundUIElement : public UIElement {
private:
  int xGrid, yGrid;
public:
  ~GroundUIElement() override {};
  GroundUIElement(int xGrid, int yGrid) {
    this->xGrid = xGrid;
    this->yGrid = yGrid;
  }
  void showElement(sf::RenderWindow *window) override {
    showGround(window, xGrid, yGrid); 
  }
private:
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

  void showGround(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showGround(window, loc);
  }
};
