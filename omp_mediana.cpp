
#include <iostream>
#include <chrono>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fmt/core.h>

namespace ch = std::chrono;

static int image_width1;
static int image_height1;
static int image_channels;

sf::Uint8* blur_image_pixels = nullptr;

#define BLUR_RADIO  21

double last_time = 0;

std::tuple<sf::Uint8, sf::Uint8, sf::Uint8>
process_pixel_median(const sf::Uint8 *image, int width, int height, int pos) {
    int r_values[9]; // Array para almacenar los valores de rojo en la ventana
    int g_values[9]; // Array para almacenar los valores de verde en la ventana
    int b_values[9]; // Array para almacenar los valores de azul en la ventana

    int k = 0; // Contador para el índice en los arrays de valores de color

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int index = (pos * 4) + (i * 4) + (j * width * 4);

            if (index >= 0 && index <= width * height * 4) {
                // Guardar los valores de color en los arrays correspondientes
                r_values[k] = image[index];
                g_values[k] = image[index + 1];
                b_values[k] = image[index + 2];
            } else {
                // Si el índice está fuera de los límites de la imagen, usar un valor predeterminado (0)
                r_values[k] = 0;
                g_values[k] = 0;
                b_values[k] = 0;
            }
            k++;
        }
    }

    // Ordenar los arrays de valores de color para calcular la mediana
    std::sort(r_values, r_values + 9);
    std::sort(g_values, g_values + 9);
    std::sort(b_values, b_values + 9);

    // Calcular la mediana para cada canal de color
    sf::Uint8 r_median = r_values[4]; // El valor mediano está en el índice 4 del array ordenado (9 elementos)
    sf::Uint8 g_median = g_values[4];
    sf::Uint8 b_median = b_values[4];

    return { r_median, g_median, b_median };
}

void blur_image_median(const sf::Uint8 *image) {
    if (!blur_image_pixels)
        blur_image_pixels = new sf::Uint8[image_width1 * image_height1 * image_channels];

    for (int i = 0; i < image_width1; i++) {
        for (int j = 0; j < image_height1; j++) {

            auto [r, g, b] = process_pixel_median(image, image_width1, image_height1, j * image_width1 + i);

            int index = (j * image_width1 + i) * image_channels;

            blur_image_pixels[index] = r;
            blur_image_pixels[index + 1] = g;
            blur_image_pixels[index + 2] = b;
            blur_image_pixels[index + 3] = 255;
        }
    }
}

int main(int argc, char** argv) {


    sf::Image image;
    image.loadFromFile("C:/Users/user/CLionProjects/examen/image02.jpg");


    image.saveToFile("imagen.jpg");
    sf::Text text;
    sf::Font font;
    {
        font.loadFromFile("C:/Users/user/CLionProjects/examen/arial.ttf");
        text.setFont(font);
        text.setString("Mandelbrot set");
        text.setCharacterSize(24); // in pixels, not points!
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setPosition(10,10);
    }

    sf::Text textOptions;
    {
        font.loadFromFile("C:/Users/user/CLionProjects/examen/arial.ttf");
        textOptions.setFont(font);
        textOptions.setCharacterSize(24);
        textOptions.setFillColor(sf::Color::White);
        textOptions.setStyle(sf::Text::Bold);
        textOptions.setString("OPTIONS: [R] Reset [B] Blur");
    }

    image_width1 = image.getSize().x;
    image_height1 = image.getSize().y;
    image_channels = 4;

    sf::Texture texture;
    texture.create(image_width1, image_height1);
    texture.update(image.getPixelsPtr());

    int w = 1600;
    int h = 900;

    sf::RenderWindow  window(sf::VideoMode(w, h), "OMP Blur example");

    sf::Sprite sprite;
    {
        sprite.setTexture(texture);

        float scaleFactorX = w * 1.0 / image.getSize().x;
        float scaleFactorY = h * 1.0 / image.getSize().y;
        sprite.scale(scaleFactorX, scaleFactorY);
    }

    sf::Clock clock;

    sf::Clock clockFrames;
    int frames = 0;
    int fps = 0;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type==sf::Event::KeyReleased) {
                if(event.key.scancode==sf::Keyboard::Scan::R) {
                    texture.update(image.getPixelsPtr());
                    last_time = 0;
                }

                else if(event.key.scancode==sf::Keyboard::Scan::B) {
                    blur_image_median(image.getPixelsPtr()); // Aplicar filtro de escala de grises
                    texture.update(blur_image_pixels);
                }

            }

            else if(event.type==sf::Event::Resized) {
                float scaleFactorX = event.size.width *1.0 / image.getSize().x;
                float scaleFactorY = event.size.height *1.0 /image.getSize().y;

                sprite = sf::Sprite();
                sprite.setTexture(texture);
                sprite.scale(scaleFactorX, scaleFactorY);
            }
        }

        if(clockFrames.getElapsedTime().asSeconds()>=1.0) {
            fps = frames;
            frames = 0;
            clockFrames.restart();
        }
        frames++;

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.draw(text);
        window.draw(textOptions);
        window.display();
    }

    return 0;
}

