#include <SFML/Graphics.hpp>
#include <iostream>

#include <omp.h>

#include <complex>
#include <chrono>

long double map(long double value, long double start1, long double stop1, long double start2, long double stop2) {
	return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

class Fractal {
private:
	sf::Vector2f movedConstant;

	int maxIterations;
	int width;
	int height;

	std::vector<sf::Color> mapping;

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Image image;

public:

	long double zoom = 1;
	long double xOff = 0;
	long double yOff = 0;

	const static long double stdYMin;
	const static long double stdYMax;
	const static long double stdXMin;
	const static long double stdXMax;

public:
	
	Fractal(sf::Vector2f movedConstant, int maxIterations, int width, int height)
		: movedConstant(movedConstant), maxIterations(maxIterations), width(width), height(height) {
		init_color_mapping();
	}
	
	void init_color_mapping() {
		mapping.push_back(sf::Color(66, 30, 15));     // index 0
		mapping.push_back(sf::Color(25, 7, 26));      // index 1
		mapping.push_back(sf::Color(9, 1, 47));       // index 2
		mapping.push_back(sf::Color(4, 4, 73));       // index 3
		mapping.push_back(sf::Color(0, 7, 100));      // index 4
		mapping.push_back(sf::Color(12, 44, 138));    // index 5
		mapping.push_back(sf::Color(24, 82, 177));    // index 6
		mapping.push_back(sf::Color(57, 125, 209));   // index 7
		mapping.push_back(sf::Color(134, 181, 229));  // index 8
		mapping.push_back(sf::Color(211, 236, 248));  // index 9
		mapping.push_back(sf::Color(241, 233, 191));  // index 10
		mapping.push_back(sf::Color(248, 201, 95));   // index 11
		mapping.push_back(sf::Color(255, 170, 0));    // index 12
		mapping.push_back(sf::Color(204, 128, 0));    // index 13
		mapping.push_back(sf::Color(153, 87, 0));     // index 14
		mapping.push_back(sf::Color(106, 52, 3));     // index 15		
	}

	// Draws the set of all values of c, that do not diverge when iterated from a given point z0
	void drawBrot(sf::RenderTarget& target) {
		image.create(width, height, sf::Color::Black);

		const long double xOrigin = movedConstant.x;
		const long double yOrigin = movedConstant.y;

		const long double xMin = (stdXMin + xOff) / zoom;
		const long double xMax = (stdXMax + xOff) / zoom;
		const long double yMin = (stdYMin + yOff) / zoom;
		const long double yMax = (stdYMax + yOff) / zoom;

#pragma omp parallel for collapse(2) 
		for (int x = 0; x < width; ++x) {
			long double cX = map(x, 0, width, xMin, xMax);
			for (int y = 0; y < height; ++y) {
				long double cY = map(y, 0, height, yMin, yMax);

				long double zX = xOrigin;
				long double zY = yOrigin;

				int n = 0;

				long double nzX;
				long double nzY;

				while (n < maxIterations) {
					//Square the complex number
					nzX = zX * zX - zY * zY + cX;
					nzY = 2 * zX * zY + cY;

					zX = nzX;
					zY = nzY;

					if (zX * zX + zY * zY > 4) {
						break;
					}

					++n;
				}

				if (n < maxIterations && n > 0) {
					int id = n % mapping.size();
					image.setPixel(x, y, mapping[id]);
				}
			}
		}

		texture.loadFromImage(image);
		sprite.setTexture(texture);
		target.draw(sprite);
	}

	// Draws all the values of z that do not diverge when iterated from, when c is a given constant
	void drawJulia(sf::RenderTarget& target) {
		image.create(width, height, sf::Color::Black);

		const long double xMin = (stdXMin + xOff) / zoom;
		const long double xMax = (stdXMax + xOff) / zoom;
		const long double yMin = (stdYMin + yOff) / zoom;
		const long double yMax = (stdYMax + yOff) / zoom;

		const long double cX = movedConstant.x;
		const long double cY = movedConstant.y;
		
#pragma omp parallel for collapse(2)
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				long double zX = map(x, 0, width, xMin, xMax);
				long double zY = map(y, 0, height, yMin, yMax);

				int n = 0;

				long double nzX;
				long double nzY;

				while (n < maxIterations) {
					//Square the complex number
					nzX = zX * zX - zY * zY + cX;
					nzY = 2 * zX * zY + cY;

					zX = nzX;
					zY = nzY;

					if (zX * zX + zY * zY > 4) {
						break;
					}

					++n;
				}

				if (n < maxIterations && n > 0) {
					int id = n % mapping.size();
					image.setPixel(x, y, mapping[id]);
				}
			}
		}

		texture.loadFromImage(image);
		sprite.setTexture(texture);
		target.draw(sprite);
	}
	
	void setOffset(long double xOff, long double yOff) {
		this->xOff = xOff;
		this->yOff = yOff;
	}

	void setZoom(long double zoom) {
		this->zoom = zoom;
	}

	void setmovedConstant(sf::Vector2f movedConstant) {
		this->movedConstant = movedConstant;
	}
	
};
const long double Fractal::stdYMin = -1.13;
const long double Fractal::stdYMax = 1.13;
const long double Fractal::stdXMin = Fractal::stdYMin * 16 / 9 - .5;
const long double Fractal::stdXMax = Fractal::stdYMax * 16 / 9 - .5;

int main() {
	
	int width = 1280;
	int height = 720;
	
	sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot");	
	window.setFramerateLimit(60);

	sf::Vector2f originPos(0, 0);
	
	int maxIterations = 256;
	
	Fractal fractal(originPos, maxIterations, width, height);

	sf::CircleShape origin(8);
	origin.setFillColor(sf::Color::Red);
	origin.setOrigin(8, 8);
	origin.setPosition(float(map(originPos.x, fractal.stdXMin, fractal.stdXMax, 0, width)), float(map(originPos.y, fractal.stdYMin, fractal.stdYMax, 0, height)));

	bool isDragging = false;

	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					isDragging = true;
				}
			}

			if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					isDragging = false;
				}
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
					float z = 1 + event.mouseWheelScroll.delta * 0.1;
					fractal.setZoom(fractal.zoom * z);
				}
			}
			
		}
		

		if (isDragging) {
			originPos = sf::Vector2f(map(sf::Mouse::getPosition(window).x, 0, width, fractal.stdXMin, fractal.stdXMax), -map(sf::Mouse::getPosition(window).y, 0, height, fractal.stdYMin, fractal.stdYMax));
			//Apply origin offset
			originPos.x += fractal.xOff;
			originPos.y += fractal.yOff;
			origin.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			fractal.setmovedConstant(originPos);
		}

		window.clear();

		fractal.drawJulia(window);
		window.draw(origin);
		
		window.display();
	}

	return 0;
}