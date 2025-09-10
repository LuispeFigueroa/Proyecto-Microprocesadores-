#include <iostream>
#include <stdlib.h>
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
                tabla[y][x] = '|'; // bordes laterales
        }
    }

    // Poner la bola en posicion incial
    tabla[ballY][ballX] = 'o';

    // Imprimir la tabla
    for (const auto &row : tabla)
    {
        std::cout << row << std::endl;
    }
}

void menu(){
    int opcion = 0;

    while(true){
        std::cout << "=== Menu ===\n";
        std::cout << "1. Jugar\n";
        std::cout << "2. Instrucciones\n";
        std::cout << "3. Salir\n";
        std::cin >> opcion;
        
        if(opcion == 1){
            
            //posicion inical de la pelota (en el centro)
            int ballX = width /2;
            int ballY = height /2;

            // limpiar la pantalla antes de jugar
            system("clear");

            //llamada a la funcion para dibujar la tabla
            dibujarTabla(ballX, ballY);
        }
        else if(opcion == 2){
            std::cout << "Instrucciones del juego:\n";
            std::cout << "1. Usa las teclas W y S para mover la paleta arriba y abajo.\n";
            std::cout << "2. El objetivo es evitar que la pelota pase tu paleta.\n";
            std::cout << "3. Cada vez que la pelota pase tu paleta, pierdes un punto.\n";
            std::cout << "4. El juego termina cuando pierdes 5 puntos.\n";
        }
        else if(opcion == 3){
            std::cout << "Saliendo del juego...\n";
            break;
        }
        else{
            std::cout << "Opcion no valida. Intente de nuevo.\n";
        }
    }
}

int main()
{

    menu();
    
    // posicion inical de la pelota (en el centro)
    int ballX = width / 2;
    int ballY = height / 2;
    dibujarTabla(ballX, ballY);

    // ancho y largo
}
