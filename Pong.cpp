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
#include <mutex>
using namespace std;
using namespace std::chrono_literals;

static const int kHeight = 23;
static const int kWidth = 40;
static const int kTop = 2;
static const int kBottom = Kheight - 1;   
static const int kLeft = 0;
static const int kRigth = kWidth - 1;   


struct Estadojuego{

    //Pelota
    int ballX = kWidth / 2; // en la mitad eje x 
    int ballY = (kHeigth / 2) + 1; // se le suma 1, porque la linea 11 no es centrica, por la puntuacion 
    int velX = -1; // Velocidad en X
    int velY = 1;  // Velocidad en Y

    //Paletas
    int tamanoPaleta = 5; 
    //cordendas de la paleta 1
    int p1x = 1; int p1y =10;
    //cordendas de la paleta 2
    int p2x = kWidth - 2; int p2y = 10;

    //Puntos
    int puntosJ = 0; //jugador
    int puntosC = 0; // computadora

    //Cotrol del estado del juego 
    atomic<bool> juegoActivo; // Controla si el juego está activo
    atomic<bool> SalirJuego; // para salir del juego 
    atomic<bool> PuntosAnotados; // proteger los puntos de la partida 
    mutex m;
};

void dibujarBordes(){
    //arriba y abajo
    for (int x = kLeft; x < kRigth; x++){
        mvaddch(top, x, '*');
        mvaddch(bottom, x, '*');
    }
    //lados
    for (int y = KTop; y<=kBottom; y++){
        mvaddch(y, KLeft, '*');
        mvaddch(y, kRigth, '*');
    }

}





static void dibujarPaleta(int x, int y0, int size)
{
    for (int y = y0; y < y0 + size; y++)
    {
        if(y>kTop && y<kBottom) mvaddch(y,x, '|');
    }
}

void dibujarMarcador( const estadoJuego& g){
    mvprintw(0,2, "Jugador: %d CPU: %d (Q:salir)", g.puntosJ, g.puntosC);
}
// para asegurarnos que la paleta no se salga de los bordes
static void clampPaleta(int& y, int size) {
    if (y < kTop + 1) y = kTop + 1;
    if (y + size >= kBottom) y = kBottom - size;
}

//control paleta jugador
void moverPaletaJugador(EstadoJuego* g){
    while (g->juegoActivo && !g->salirJuego){
        int ch = getch();
        if(ch == 'q'|| ch=='Q'){g->juegoActivo = false; break;}
        if (ch == 'w' || ch = 'W'){
            lock_guard<mutex> lock(g->m);
            g->.p1y--; clampPaleta(g->p1y, g->tamanoPaleta);
        }else if(ch == 's' || ch == 'S'){
            lock_guard<mutex> lock(g->m);
            g->p1y++; clampPaleta(g->p1y, g->tamanoPaleta);
        }
        this_thread::sleep_for(5ms);
    }
}

void movePaletaComPu(EstadoJuego* g){
    while(g->juegoActivo && !g->salirJuego){
        lock_guard<mutex> lock(g->m);
        int target = g->bally - g->tamanoPaleta / 2;
        if(g->p2y < target) g->p2y++;
        else if(g->p2y > target) g->p2y--;
        clampPaleta(g->p2y, g->tamanoPaleta);
    }
        this_thread::sleep_for(25ms);
}

void actualizarPuntos(EstadoJuego* g) {
    auto resetBall = [](EstadoJuego* s, int dirX){
        s->ballX = kWidth/2;
        s->ballY = (kHeight/2)+1;
        s->velX  = dirX;
        s->velY  = (rand()%2==0)? -1 : +1;
        s->puntoReciente = false;
    };

    while (g->juegoActivo && !g->salirJuego) {
        bool gol = false;
        int dirX = 0;

        {
            lock_guard<mutex> lock(g->m);
            if (g->ballX <= kLeft)  { g->puntosC++; gol = true; dirX = +1; }
            if (g->ballX >= kRight) { g->puntosJ++; gol = true; dirX = -1; }
        }

        if (gol) {
            g->puntoReciente = true;
            this_thread::sleep_for(400ms); // pequeña pausa
            lock_guard<mutex> lock(g->m);
            resetBall(g, dirX);
        }

        this_thread::sleep_for(10ms);
    }
}

void renderLoop(EstadoJuego* g){
    while (g->juegoActivo && !g ->salirJuego){
        lock_guard<mutex> lock(g->m);
        clear();
        dibujarBordes();
        dibujarMarcador(*g);
        dibujarPaleta(g->p1x, g->p1y, g->tamanoPaleta);
        dibujarPaleta(g->p2x, g->p2y, g->tamanoPaleta);
        mvaddch(g->ballY, g->ballX, 'O');
    }
    refresh();
    this_thread::sleep_for(16ms); //  60 fps
}
//setup cuando comineza la partida 
void iniciarJuego(){
    EstadoJuego g;

    //configurar ncurses para el juego
    nodelay(); // getch no bloqueante
    keypad(stdscr, TRUE); //permitir que las teclas funciones
    noecho();
    curs_set(0); // ocultar mouse

    g.juegoActivo = true;

    thread tPlayer(moverPaletaJugador, &g);
    thread tCPU(moverPaletaComPu, &g);
    thread tBall(renderLoop, &g);
    thread tScore(actualizarMarcador, &g);
    thread tRender(renderLoop, &g);


    //condicion para que la partida acabe con ganador
    while(g.juegoActivo && !g.salirJuego){
        {lock_guard<mutex> lock(g.m);
        if(g.puntosJ >= 11 || g.puntosC >=11) g.juegoActivo = false;
        }
        this_thread::sleep_for(50ms);
    }

    g.juegoActivo = false;
    if (tplayer.joinable()) tPlayer.join();
    if (tCPU.joinable()) tCPU.join();
    if (tBall.joinable()) tBall.join();
    if (tScore.joinable()) tScore.join();
    if (tRender.joinable()) tRender.join();

    //mensaje cuando temrina la partida
    nodelay(stdscr, false); 
    clear();
    mvprintw(5,8, "Juego Terminado!");
    mvprint(7,8, "Apacha cualquier tecla para voler al menu")
    refresh();
    getch();






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

    nodelay(stdscr, FALSE); // getch bloqueante
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
        mvprintw(5 + options.size()*2+1, 10, "Use las flechas para navegar y Enter para seleccionar");

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
            case KEY_ENTER:
            case '\r':
                choice = highlight;
                if(choice == 0){
                    //Jugar
                    clear();
                    refresh();
                    iniciarJuego();
                    
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
            case 'q':
            case 'Q':
                return;
                
        }
    }
}

int main()
{

    // inciar ncurses para dibujar la tabla
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    // Menu principal
    menu();

    endwin(); // terminar  ncurses

}