#include <iostream>
#include <stdlib.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include <thread>

const int height = 20;
const int width = 40;

void dibujarTabla(int ballX, int ballY)
{

    std::vector<std::string> tabla(height, std::string(width, ' '));

    // Dibujar los bordes
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (y == 0 || y == height - 1)
                tabla[y][x] = '*'; // bordes superior e inferior
            else if (x == 0 || x == width - 1)
                tabla[y][x] = '*'; // bordes laterales
        }
    }

    // Poner la bola en posicion incial
    tabla[ballY][ballX] = 'o';

    // paleta jugador (izquierda)
    for (int py = 8; py <= 12; py++)
    {
        tabla[py][1] = '|';
    }
    // paleta

    // Imprimir la tabla
    for (const auto &row : tabla)
    {
        std::cout << row << std::endl;
    }
}
int main()
{
    // posicion inical de la pelota (en el centro)
    int ballX = width / 2;
    int ballY = height / 2;
    dibujarTabla(ballX, ballY);

    // ancho y largo
}
