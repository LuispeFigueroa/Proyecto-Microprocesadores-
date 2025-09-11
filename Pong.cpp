#include <iostream>
#include <stdlib.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include <thread>
using namespace std;

const int height = 23;
const int width = 40;

void dibujarPaleta(int x, int yInicial, int size) {
    for (int py = yInicial; py < yInicial + size; py++) {
        mvaddch(py, x, '|');
    }
}

void dibujarMarcador(int x){

    int marcadorj = 0;    //marcador del jugador
    int marcadorc = 0;    //marcador de la computadora

    mvprintw(0, x, "Jugador: %d", marcadorj);
    mvprintw(0, x + 12, "CPU: %d", marcadorc);
}


void dibujarTabla(int ballX, int ballY)
{

    clear();
    // Dibujar los bordes
    for (int y = 2; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (y == 2 || y == height - 1)
                mvaddch(y,x, '*'); // bordes superior e inferior
            else if (x == 0 || x == width - 1)
                mvaddch(y,x,'*'); // bordes laterales
        }
    }

    // Poner la bola en posicion incial
    mvaddch(ballY, ballX, 'o');
    

    // Dibujar paletas usando la nueva función
    dibujarPaleta(1, 10, 5);         // paleta izquierda
    dibujarPaleta(width - 2, 10, 5); // paleta derecha

    //Dibujar marcador
    dibujarMarcador(11);

    // Imprimir la tabla
    refresh();
}

void menu(){
    int opcion = 0;

    while(true){
        clear();
        mvprintw(2,10, "=== Menu ===");
        mvprintw(4,10, "1. Jugar");
        mvprintw(6,10,"2. Instrucciones");
        mvprintw(8,10,"3. Salir");
        opcion = getch(); // leer la opcion del usuario
        
        if(opcion == '1'){
            
            //posicion inical de la pelota (en el centro)
            int ballX = width /2;
            int ballY = (height /2)+1;

            // limpiar la pantalla antes de jugar
            system("clear");

            //llamada a la funcion para dibujar la tabla
            dibujarTabla(ballX, ballY);
            getch(); // no regresa al menu hasta que se toque una tecla 
        }
        else if(opcion == '2'){
            clear();

            mvprintw(2,5, "Instrucciones del juego:");
            mvprintw(4,5,"1. Usa las teclas W y S para mover la paleta arriba y abajo.");
            mvprintw(6,5,"2. El objetivo es evitar que la pelota pase tu paleta.");
            mvprintw(8,5,"3. Cada vez que la pelota pase tu paleta, pierdes un punto.");
            mvprintw(10,5,"4. El juego termina cuando pierdes 5 puntos.");
            mvprintw(12,5,"Presiona cualquier tecla para volver al menu.");
            refresh();
            getch(); // espera a que el usuario presione una tecla para volver al menu
        }
        else if(opcion == '3'){
            clear();
            mvprintw(2,10,"Saliendo del juego...");
            refresh();
            break;
        }
        else{
            mvprintw(10,10,"Opcion no valida. Intente de nuevo.");
            refresh();
            getch();
        }
    }
}

int main()
{

    //inciar ncurses para dibujar la tabla
    initscr();
    noecho();
    curs_set(0);
    //Menu principal
    menu();

    endwin(); // terminar  ncurses
    


    // ancho y largo
}
