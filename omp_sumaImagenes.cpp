#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <fmt/core.h>
#include <chrono>
static int image_width;
static int image_height;
static int image_channels=4;
namespace ch = std::chrono;

std::vector<sf::Uint8> blur_image_pixels;

std::vector<sf::Uint8> sumaImagen_Serial(const sf::Uint8* imagen, const sf::Uint8* imagen2, int with, int height){

    std::vector<sf::Uint8> buffer(with*height*4);
    for(int i= 0; i<height; i++){
        for(int j =0; j<with; j++){

            int index= (i*with+j)*image_channels;
            buffer[index] = (imagen[index]+ imagen2[index])/2;
            buffer[index+1] = (imagen[index+1]+ imagen2[index+1])/2;
            buffer[index+2] = (imagen[index+2]+ imagen2[index+2])/2;;
            buffer[index+3] = 255;


        }

    }
    return buffer;
}

std::vector<sf::Uint8> sumaImagen_Paralelo(const sf::Uint8* imagen, const sf::Uint8* imagen2, int with, int height){

    std::vector<sf::Uint8> buffer(with*height*4);

#pragma omp parallel for collapse(2)
    for(int i= 0; i<height; i++){
        for(int j =0; j<with; j++){

            int index= (i*with+j)*image_channels;
            buffer[index] = (imagen[index]+ imagen2[index])/2;
            buffer[index+1] = (imagen[index+1]+ imagen2[index+1])/2;
            buffer[index+2] = (imagen[index+2]+ imagen2[index+2])/2;;
            buffer[index+3] = 255;


        }

    }
    return buffer;
}


int main(int argc, char** argv){

    sf::Image image;
    sf::Image image2;
    image.loadFromFile("C:/Users/user/CLionProjects/examen/imagen.jpg");
    image.saveToFile("imagen.jpg");
    image2.loadFromFile("C:/Users/user/CLionProjects/examen/pikachu.jpg");
    image2.saveToFile("pikachu.jpg");
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

    image_width = image.getSize().x;
    image_height = image.getSize().y;

    sf::Texture texture;
    texture.create(image_width, image_height);
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

    while(window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }else if(event.type == sf::Event::KeyReleased){
                if(event.key.scancode==sf::Keyboard::Scan::R) {
                    texture.update(image.getPixelsPtr());

                }
                else if(event.key.scancode==sf::Keyboard::Scan::B) {
                    blur_image_pixels = sumaImagen_Paralelo(image.getPixelsPtr(), image2.getPixelsPtr(), image_width, image_height); // Aplicar filtro de escala de grises
                    texture.update(blur_image_pixels.data()); // Actualizar textura con píxeles desenfocados
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