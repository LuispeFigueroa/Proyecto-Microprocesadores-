// Proyecto 1 Programacion de MicroProcesadores
// Juego Pong en la terminar con funciones dividas en hilos
// Luis Pedro Figueroa 24087, Adair Velasquez 24586, Diego Gonzalez 24170
// g++ Pong.cpp -lncurses -o pong

#include <iostream>
#include <stdlib.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
using namespace std;

static const int height = 23;
static const int width = 40;
static const int top = 40;
static const int bottom = height - 1;   
static const int left = 40;
static const int rigth = width - 1;   


struct Estadojuego{

    //Pelota
    int ballX = width / 2; // en la mitad eje x 
    int ballY = (height / 2) + 1; // se le suma 1, porque la linea 11 no es centrica, por la puntuacion 
    int velX = -1; // Velocidad en X
    int velY = 1;  // Velocidad en Y

    //Paletas
    int tamanoPaleta = 5; 
    //cordendas de la paleta 1
    int p1x = 1; int p1y =10;
    //cordendas de la paleta 2
    int p2x = width - 2; int p2y = 10;

    //Puntos
    int puntosJ = 0; //jugador
    int puntosC = 0; // computadora

    //Cotrol del estado del juego 
    <atomic<bool> juegoActivo; // Controla si el juego está activo
    <atomic<bool> SalirJuego; // para salir del juego 
    <atomic<bool> PuntosAnotados; // proteger los puntos de la partida 
    mutex m;
};

void dibujarBordes(){
    //arriba y abajo
    for (int x = left; x< rigth; x++){
        mvaddch(top, x, '*');
        mvaddch(bottom, x, '*');
    }
    //lados
    for (int y = top; y<bottom; y++){
        mvaddch(y, left, '*');
        mvaddch(y, rigth, '*');
    }

}





void dibujarPaleta(int x, int yInicial, int size)
{
    for (int py = yInicial; py < yInicial + size; py++)
    {
        mvaddch(py, x, '|');
    }
}

void dibujarMarcador(int x)
{

    int marcadorj = 0; // marcador del jugador
    int marcadorc = 0; // marcador de la computadora

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
                mvaddch(y, x, '*'); // bordes superior e inferior
            else if (x == 0 || x == width - 1)
                mvaddch(y, x, '*'); // bordes laterales
        }
    }

    // Poner la bola en posicion incial
    mvaddch(ballY, ballX, 'o');

    // Dibujar paletas usando la nueva función
    dibujarPaleta(1, 10, 5);         // paleta izquierda
    dibujarPaleta(width - 2, 10, 5); // paleta derecha

    // Dibujar marcador
    dibujarMarcador(11);

    // Imprimir la tabla
    refresh();
}

void menu(){
    int highlight = 0;
    int choice;
    int c;

    std::vector<std::string> options = {
        "1. Jugar",
        "2. Instrucciones",
        "3. Salir"
    };

    keypad(stdscr, TRUE); // habilitar las teclas de flecha

    while(true){
        clear();
        // dibujar el menu
        mvprintw(1, 10, "|--------------------|");
        mvprintw(2, 10, "|        MENU        |");
        mvprintw(3, 10, "|--------------------|");
        
        for(int i = 0; i < (int)options.size(); i++){
            if(i == highlight){
                attron(A_REVERSE); // resaltar la opcion seleccionada
                mvprintw(5 + i*2, 10, "%s", options[i].c_str());
                attroff(A_REVERSE);
            }else{
                mvprintw(5 + i*2, 10, "%s", options[i].c_str());
            }
        }
        
        mvprintw(5 + options.size()*2, 10, "|--------------------|");
        mvprintw(5 + options.size()*2, 10, "Use las flechas para navegar y Enter para seleccionar");

        refresh();

        c = getch();
        switch(c){
            case KEY_UP:
                highlight = (highlight -1 + options.size()) % options.size();
                break;
            case KEY_DOWN:
                highlight = (highlight +1) % options.size();
                break;
            case '\n':
                choice = highlight;
                if(choice == 0){
                    //Jugar
                    int ballX = width / 2;
                    int ballY = (height / 2) + 1;
                    clear();
                    dibujarTabla(ballX, ballY);
                    getch();
                }else if(choice == 1){
                    //Instrcciones
                    clear();
                    mvprintw(2,5, "Instrucciones del juego:");
                    mvprintw(4,5,"1. Usa las teclas W y S para mover la paleta arriba y abajo.");
                    mvprintw(6,5,"2. Evita que la pelota pase tu paleta.");
                    mvprintw(8,5,"3. Cada vez que pase, pierdes un punto.");
                    mvprintw(10,5,"4. El juego termina al perder 5 puntos.");
                    mvprintw(12,5,"Presiona cualquier tecla para volver al menu.");
                    refresh();
                    getch();
                }else if(choice == 2){
                    //salir
                    clear();
                    mvprintw(2, 10, "Saliendo del juego...");
                    refresh();
                    getch();
                    return;
                }
                break;
        }
    }
}
int main()
{

    // inciar ncurses para dibujar la tabla
    initscr();
    noecho();
    curs_set(0);
    // Menu principal
    menu();

    endwin(); // terminar  ncurses

    // ancho y largo
}
*/
