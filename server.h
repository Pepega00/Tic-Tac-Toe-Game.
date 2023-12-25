#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <map>
#include <chrono>
#include <arpa/inet.h>

std::map<int, std::string> clients; //Вектор с клиентами в сети
std::map<int, bool> queue; //Вектор со значениями ходов для каждого клиента в сети
int count_of_move = 0;  //Количество ходов
int port = 0; 
struct sockaddr_in address; 
int addrlen = sizeof(address);
//std::ofstream log("log_server.txt"); //Файл с логами

//Игровое поле
char game_map[3][3] = {{' ', ' ', ' '},
                       {' ', ' ', ' '},
                       {' ', ' ', ' '}};
                       
bool gamer = true; //Состояние текущего хода

