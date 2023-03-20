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
	sf::Vector2f origin;

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
	
	Fractal(sf::Vector2f origin, int maxIterations, int width, int height) 
		: origin(origin), maxIterations(maxIterations), width(width), height(height) {
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

	void drawParallel(sf::RenderTarget& target) {
		image.create(width, height, sf::Color::Black);
		
		const long double xOrigin = origin.x;
		const long double yOrigin = origin.y;
		
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

	void setOffset(long double xOff, long double yOff) {
		this->xOff = xOff;
		this->yOff = yOff;
	}

	void setZoom(long double zoom) {
		this->zoom = zoom;
	}

	void setOrigin(sf::Vector2f origin) {
		this->origin = origin;
	}
	
};
const long double Fractal::stdYMin = -1.13;
const long double Fractal::stdYMax = 1.13;
const long double Fractal::stdXMin = Fractal::stdYMin * 16 / 9 - .5;
const long double Fractal::stdXMax = Fractal::stdYMax * 16 / 9 - .5;

	
void renderHighRes() {

	int maxIterations = 5000;

	//16k
	int width = 15360;
	int height = 8640;
	
	sf::CircleShape origin(5);
	origin.setFillColor(sf::Color::Red);
	origin.setOrigin(5, 5);
	origin.setPosition(width / 2, height / 2);

	Fractal fractal(origin.getPosition(), maxIterations, width, height);
	
	sf::RenderTexture renderTexture;
	
	renderTexture.create(width, height);
	
	renderTexture.clear(sf::Color::Black);
	fractal.drawParallel(renderTexture);
	
	renderTexture.getTexture().copyToImage().saveToFile("mandelbrot.png");
}

int main() {
	
	int width = 1280;
	int height = 720;
	
	sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot");	
	window.setFramerateLimit(60);

	sf::Vector2f originPos(0, 0);
	sf::Vector2f r1i0Pos(1, 0);
	sf::Vector2f r0i1Pos(0, 1);
	
	int maxIterations = 256;

	sf::Vector2i prevMousePos = sf::Mouse::getPosition(window);

	Fractal fractal(originPos, maxIterations, width, height);

	sf::CircleShape origin(8);
	origin.setFillColor(sf::Color::Red);
	origin.setOrigin(8, 8);
	origin.setPosition(float(map(originPos.x, fractal.stdXMin, fractal.stdXMax, 0, width)), float(map(originPos.y, fractal.stdYMin, fractal.stdYMax, 0, height)));

	sf::CircleShape r1i0(3);
	r1i0.setFillColor(sf::Color::Green);
	r1i0.setOrigin(3, 3);
	r1i0.setPosition(float(map(r1i0Pos.x, fractal.stdXMin, fractal.stdXMax, 0, width)), float(-map(r1i0Pos.y, fractal.stdYMin, fractal.stdYMax, 0, height)));
	
	sf::CircleShape r0i1(3);
	r0i1.setFillColor(sf::Color::Blue);
	r0i1.setOrigin(3, 3);
	r0i1.setPosition(float(map(r0i1Pos.x, fractal.stdXMin, fractal.stdXMax, 0, width)), float(-map(r0i1Pos.y, fractal.stdYMin, fractal.stdYMax, 0, height)));

	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseWheelScrolled) {

				const long double zoomFactor = 1 + event.mouseWheelScroll.delta * .1;

				fractal.setZoom(fractal.zoom * zoomFactor);
			}
		}

		
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus() && !origin.getGlobalBounds().contains(prevMousePos.x, prevMousePos.y)) {
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			sf::Vector2i delta = prevMousePos - mousePos;

			long double px2coord = (fractal.stdXMax - fractal.stdXMin) / width;
		
			long double xOff = delta.x * px2coord;
			long double yOff = delta.y * px2coord;
			
			fractal.setOffset(fractal.xOff + xOff, fractal.yOff + yOff);
			
			origin.setPosition(origin.getPosition().x - delta.x, origin.getPosition().y - delta.y);
			
			
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
			originPos = sf::Vector2f(map(sf::Mouse::getPosition(window).x, 0, width, fractal.stdXMin, fractal.stdXMax), -map(sf::Mouse::getPosition(window).y, 0, height, fractal.stdYMin, fractal.stdYMax));
			//Apply origin offset
			originPos.x += fractal.xOff;
			originPos.y += fractal.yOff;
			origin.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			fractal.setOrigin(originPos);
		}
		prevMousePos = sf::Mouse::getPosition(window);

		window.clear();

		fractal.drawParallel(window);
		window.draw(origin);
		window.draw(r1i0);
		window.draw(r0i1);
		
		window.display();
	}

	return 0;
}