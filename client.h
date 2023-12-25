#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <future>
#include <ctime>

int time_for_move = 0; //Время на ход
int port; //Порт
bool gamer = true; //Состояние текущего хода
bool move; //Состояние хода определенного игрока
//std::ofstream log("log_clients.txt", std::ios::app); //Файл с логами

//Игровое поле
char game_map[3][3] = {{' ', ' ', ' '},
                       {' ', ' ', ' '},
                       {' ', ' ', ' '}};
