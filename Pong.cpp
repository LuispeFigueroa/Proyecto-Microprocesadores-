// Proyecto 1 Programacion de MicroProcesadores
// Juego Pong en la terminal con funciones divididas en hilos
// Luis Pedro Figueroa 24087, Adair Velasquez 24586, Diego Gonzalez 24170
// Compilar: g++ Pong.cpp -o Pong -lncurses -pthread

#include <iostream>
#include <ncurses.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>


using namespace std;
using namespace std::chrono_literals;






// Constantes del tablero
static const int kHeight = 23;
static const int kWidth = 40;
static const int kTop = 2;
static const int kBottom = kHeight - 1;   
static const int kLeft = 0;
static const int kRight = kWidth - 1;   
//dificultades
struct Dificultad{
    int tickMs;// cuanto tiempo se tarda en moverse la paleta
    int noise;// ruido vertical para que no 100% preciso el movimiento de la compu
};

constexpr Dificultad DIF_FACIL {80,2};
constexpr Dificultad DIF_MEDIO {50,1};
constexpr Dificultad DIF_DIFICIL {30,0};

//empieza en normal predeterminadamente
static Dificultad gDIF = DIF_MEDIO;
static string gDifName = "Medio";



// Estado del juego
struct EstadoJuego {
    int ballX = kWidth / 2; 
    int ballY = (kHeight / 2) + 1; 
    int velX = -1; 
    int velY = 1;  

    int tamanoPaleta = 5; 
    int p1x = 1, p1y = 10;          
    int p2x = kWidth - 2, p2y = 10; 

    int puntosJ = 0;
    int puntosC = 0;

    atomic<bool> juegoActivo; 
    atomic<bool> salirJuego; 
    atomic<bool> puntoReciente; 
    mutex m;
};

// Dibujar bordes
void dibujarBordes() {
    for (int x = kLeft; x < kRight; x++) {
        mvaddch(kTop, x, '*');
        mvaddch(kBottom, x, '*');
    }
    for (int y = kTop; y <= kBottom; y++) {
        mvaddch(y, kLeft, '*');
        mvaddch(y, kRight, '*');
    }
}

// Dibujar paletas
static void dibujarPaleta(int x, int y0, int size) {
    for (int y = y0; y < y0 + size; y++) {
        if (y > kTop && y < kBottom) mvaddch(y, x, '|');
    }
}

// Dibujar marcador
void dibujarMarcador(const EstadoJuego& g) {
    mvprintw(0, 2, "Jugador: %d CPU: %d (Q:salir)", g.puntosJ, g.puntosC);
}

// Limitar paletas
static void clampPaleta(int& y, int size) {
    if (y < kTop + 1) y = kTop + 1;
    if (y + size >= kBottom) y = kBottom - size;
}

// Paleta jugador
void moverPaletaJugador(EstadoJuego* g) {
    while (g->juegoActivo && !g->salirJuego) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') { g->juegoActivo = false; break; }
        if (ch == 'w' || ch == 'W') {
            lock_guard<mutex> lock(g->m);
            g->p1y--; clampPaleta(g->p1y, g->tamanoPaleta);
        } else if (ch == 's' || ch == 'S') {
            lock_guard<mutex> lock(g->m);
            g->p1y++; clampPaleta(g->p1y, g->tamanoPaleta);
        }
        this_thread::sleep_for(10ms); 
    }
}

// Paleta CPU
void moverPaletaCPU(EstadoJuego* g) {
    using clk = chrono::steady_clock;
    auto next = clk::now();


    while (g->juegoActivo && !g->salirJuego) {
        next += chrono::milliseconds(gDIF.tickMs);
        {
            lock_guard<mutex> lock(g->m);

            // objetivo seria la pelota mas el ruido de movimiento
            int ruido = (gDIF.noise ==0) ? 0:(rand() % (2 * gDIF.noise + 1)) - gDIF.noise;
            int target= g->ballY + ruido;

            int center = g->p2y + g->tamanoPaleta / 2; // posicion de la pelota, mas la mitad de la paleta
           if(target > center) g->p2y++; // si esta mas abajo de la bola sube
           else if(target < center) g->p2y--; // si esta mas arriba de la bola baja

           //asegurar que no se pase de los bordes 
            clampPaleta(g->p2y, g->tamanoPaleta);
        }
        this_thread::sleep_until(next); 
    }
}

// Movimiento pelota
void moverPelota(EstadoJuego* g) {
    auto next = chrono::steady_clock::now();
    const auto step = 50ms;

    while (g->juegoActivo && !g->salirJuego) {
        next += step;
        {
            lock_guard<mutex> lock(g->m);
            g->ballX += g->velX;
            g->ballY += g->velY;

            if (g->ballY <= kTop + 1)     { g->ballY = kTop + 1;     g->velY *= -1; }
            if (g->ballY >= kBottom - 1)  { g->ballY = kBottom - 1;  g->velY *= -1; }

            if (g->ballX == g->p1x + 1 &&
                g->ballY >= g->p1y && g->ballY < g->p1y + g->tamanoPaleta) {
                g->velX = +1;
            }
            if (g->ballX == g->p2x - 1 &&
                g->ballY >= g->p2y && g->ballY < g->p2y + g->tamanoPaleta) {
                g->velX = -1;
            }
        }
        this_thread::sleep_until(next);
    }
}

// Actualizar puntos
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
            this_thread::sleep_for(400ms);
            lock_guard<mutex> lock(g->m);
            resetBall(g, dirX);
        }
        this_thread::sleep_for(10ms);
    }
}

// Render loop
void renderLoop(EstadoJuego* g){
    while (g->juegoActivo && !g->salirJuego){
        {
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
}

// Iniciar juego
void iniciarJuego(){
    EstadoJuego g;
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    g.juegoActivo = true;
    g.salirJuego = false;

    thread tPlayer(moverPaletaJugador, &g);
    thread tCPU(moverPaletaCPU, &g);
    thread tBall(moverPelota, &g);
    thread tScore(actualizarPuntos, &g);
    thread tRender(renderLoop, &g);

    while(g.juegoActivo && !g.salirJuego){
        { lock_guard<mutex> lock(g.m);
          if(g.puntosJ >= 11 || g.puntosC >=11) g.juegoActivo = false;
        }
        this_thread::sleep_for(50ms);
    }

    g.juegoActivo = false;
    if (tPlayer.joinable()) tPlayer.join();
    if (tCPU.joinable()) tCPU.join();
    if (tBall.joinable()) tBall.join();
    if (tScore.joinable()) tScore.join();
    if (tRender.joinable()) tRender.join();

    nodelay(stdscr, FALSE); 
    clear();
    mvprintw(5,8, "Juego Terminado!");
    mvprintw(7,8, "Presiona cualquier tecla para volver al menu");
    refresh();
    getch();
}


// Menú
void menu(){
    int highlight = 0;
    int choice;
    int c;
    
    vector<string> options = {"1. Jugar","2. Instrucciones","3. Dificultad (Actual:" + gDifName+")","4. Salir"};
    nodelay(stdscr, FALSE);
    keypad(stdscr, TRUE);

    while(true){
        clear();
        mvprintw(1, 10, "|--------------------|");
        mvprintw(2, 10, "|        MENU        |");
        mvprintw(3, 10, "|--------------------|");
        for(int i = 0; i < (int)options.size(); i++){
            if(i == highlight){
                attron(A_REVERSE);
                mvprintw(5 + i*2, 10, "%s", options[i].c_str());
                attroff(A_REVERSE);
            }else{
                mvprintw(5 + i*2, 10, "%s", options[i].c_str());
            }
        }
        mvprintw(5 + options.size()*2, 10, "|--------------------|");
        mvprintw(5 + options.size()*2+1, 10, "Use flechas y Enter para seleccionar");
        refresh();

        c = getch();
        switch(c){
            case KEY_UP: highlight = (highlight -1 + options.size()) % options.size(); break;
            case KEY_DOWN: highlight = (highlight +1) % options.size(); break;
            case '\n': case KEY_ENTER: case '\r':
                choice = highlight;
                if(choice == 0){ iniciarJuego();
                nodelay(stdscr, FALSE); }
                else if(choice == 1){
                    clear();
                    mvprintw(2,5, "Instrucciones del juego:");
                    mvprintw(4,5,"1. Usa W y S para mover la paleta.");
                    mvprintw(6,5,"2. Evita que la pelota pase tu lado.");
                    mvprintw(8,5,"3. El CPU se mueve automaticamente.");
                    mvprintw(10,5,"4. Gana el primero que llegue a 11 puntos.");
                    mvprintw(12,5,"Presiona cualquier tecla para volver.");
                    refresh();
                    getch();
                }else if(choice == 2){
                    if(gDifName =="Facil"){
                        gDIF = DIF_MEDIO; gDifName = "Medio";
                    }else if(gDifName == "Medio"){
                        gDIF = DIF_DIFICIL; gDifName = "Dificil";
                    }else{
                        gDIF = DIF_FACIL; gDifName = "Facil";
                    }
                    options[2] = "3. Dificultad (Actual:" + gDifName+")";
                }else if(choice == 3){
                    clear();
                    mvprintw(5,10, "Saliendo del juego.");
                    refresh();
                    getch();
                    return;
                }
                break; 
            case 'q':case 'Q':
                return;
             
        }
    }
}

// Main
int main(){
    srand((unsigned)time(nullptr)); //para el ruido en la dificultad
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    menu();
    endwin();
}

