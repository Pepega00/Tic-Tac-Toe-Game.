#include "server.h"

void clear_buffer(char* buffer){
    for(int i=0;i<1024;++i){
        buffer[i]=0;
    }
}
std::string get_address(std::string config_name){     
    std::ifstream config(config_name);
    if (!config.is_open()){
        const char* error = "No config file!";
        std::cout << error << std::endl;
        exit(0);
    }
    std::string string;
    std::string addr;
    while (config.peek() != EOF){
        std::getline(config, string);
        if (string.find("address: ") != std::string::npos){
            for (int i = 9; i < string.length(); ++i){
                addr += string[i];
            }
            config.close();
            return addr;
        }        
    }
    config.close();
    return addr;
}

std::string get_logName(std::string config_name){     
    std::ifstream config(config_name);
    if (!config.is_open()){
        const char* error = "No config file!";
        std::cout << error << std::endl;
        exit(0);
    }
    std::string string;
    std::string name;
    while (config.peek() != EOF){
        std::getline(config, string);
        if (string.find("log_file: ") != std::string::npos){
            for (int i = 10; i < string.length(); ++i){
                name += string[i];
            }
            config.close();
            return name;
        }        
    }
    config.close();
    return name;
}
//Запись в логи
void logWriter(std::string buffer,std::string config_name){
    std::ofstream log(get_logName(config_name),std::ios::app);
    time_t now = time(nullptr);
    std::tm *localTime = std::localtime(&now);
    log << "[" << localTime->tm_mday << "." <<
                  localTime->tm_mon + 1 << "." <<
                  localTime->tm_year + 1900 << " ";
    if (localTime->tm_hour < 10){
        log << 0 << localTime->tm_hour;
    }
    else {
        log << localTime->tm_hour;
    }
    log << ":";
    if (localTime->tm_min < 10){
        log << 0 << localTime->tm_min;
    }
    else {
        log << localTime->tm_min;
    }
    log << ":"; 
    if (localTime->tm_sec < 10){
        log << 0 << localTime->tm_sec;
    }
    else {
        log << localTime->tm_sec;
    }
    log << "] " << buffer << std::endl;   
}

//Очистка игрового поля
void clearMap(){
    for (int i = 0; i < 3; ++i){
        for (int k = 0; k < 3; ++k){
            game_map[i][k] = ' ';
        }
    }
}

//Определение победителя
int winner(){
    int sum_diag = 0;
    int sum_diag_2 = 0; 
    for (int i = 0; i < 3; ++i){
        int sum_v = 0;
        int sum_h = 0;
        sum_diag += game_map[i][i];
        sum_diag_2 += game_map[2 - i][i];
        for (int j = 0; j < 3; ++j){
            sum_v += game_map[j][i];
            sum_h += game_map[i][j];
        }
        if (sum_v == 3 * 'X' || sum_h == 3 * 'X'){ return 1; }
        if (sum_v == 3 * 'O' || sum_h == 3 * 'O'){ return -1; }
    }
    if (sum_diag == 3 * 'X' || sum_diag_2 == 3 * 'X'){ return 1; }
    if (sum_diag == 3 * 'O' || sum_diag_2 == 3 * 'O'){ return -1; }
    return 0;
}

//Заполнение игрового поля по координате, которую ввел пользователь
bool ticTacToe(int coord){
    char symbol;
    if (gamer) { symbol = 'X'; }
    else { symbol = 'O'; }

    if (game_map[(coord - 1) / 3][(coord - 1) % 3] == ' '){ 
        game_map[(coord - 1) / 3][(coord - 1) % 3] = symbol; 
        gamer = !gamer;
    }
    else { return false; }
    return true;
}

//Проверка на то, существует ли клиент, который пытается подключиться
bool checkNewClientExist(std::string Nickname, std::string Password,std::string config_name){
    std::ifstream config(config_name);
    while (config.peek() != EOF){
        std::string str;
        std::getline(config, str);
        if (str == (Nickname + ' ' + Password)){
            config.close();
            return true; 
        }
    }
    config.close();
    return false;
}

//Проверка на то, не подключается ли тот же пользователь еще раз
bool checkNewClientNotOld(std::string Nickname){
    for (auto it = clients.begin(); it != clients.end(); ++it){
        if (it->second == Nickname){ return false; }
    }
    return true;
}

//Заполнение буфера по состояниям игрового поля
void fillBuffer(char* buffer){
    for (int i = 0, j = 0; i < 3; ++i){
        for (int k = 0; k < 3; ++k, ++j){
            buffer[j] = game_map[i][k];
        }
    }
}

//Обработка сообщений с клиента
void clientHandler(int socket) {
    char buffer[1024] = {0};
    int valread;
    bool disconnect=false;
    while (true) {
        memset(buffer, 0, sizeof(buffer));      
        valread = read(socket, buffer, 1024);

         //Клиент отключился
        if (valread == 0) {
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "\033[0;31m" << "Client " << clients[socket] << " disconnected!" << "\033[0m" << std::endl;
            //logWriter("Client " + clients[socket] + " disconnected!");
            std::string text;
            if (queue[socket]) {
                std::cout<<"X disconnected!\n";
                text = "O winner!";                
            }
            else {
                std::cout<<"O disconnected!\n";
                text = "X winner!";           
            }
            
            clients.erase(socket);
            queue.erase(socket);
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                send(it->first, text.c_str(), text.length(), 0);               
            }
            break;
        }
        //Превышено время хода
        if (strcmp(buffer, "-1") == 0 && queue[socket] == gamer){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout<<"timeout!\n";
            std::string text;
            if (queue[socket]) {
                text = "O winner!";                
            }
            else {
                text = "X winner!";           
            }
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                send(it->first, text.c_str(), text.length(), 0);               
            }
            count_of_move = 0;
            gamer = true;
            clearMap();
            //logWriter(text);
            //
            std::cout << "\033[0;31m" << "Client " << clients[socket] << " disconnected!" << "\033[0m" << std::endl;
            //logWriter("Client " + clients[socket] + " disconnected!");
            clients.erase(socket);
            queue.erase(socket);
            break;
        }

        //Если ход другого игрока, то пропускаем
        if (queue[socket] != gamer) { 
            continue;
        }
        //Заполняем игровое поле по пришедшей координате
        if (ticTacToe(static_cast<int>(buffer[0] - '0'))) { 
            std::cout<<"turn: "<<queue[socket]<<'\n';
            ++count_of_move;
        }
        else { 
            send(socket, "Wrong input!\n", sizeof("Wrong input!\n"), 0);
            //logWriter("Wrong input by " + clients[socket]);
            continue;
        }
        
        fillBuffer(buffer);

        std::string text;

        if (winner() == 1){
           text = "X winner!";
        }
        else if (winner() == -1){
            text = "O winner!";
        }
        else if (count_of_move == 9){
            text = "Draw!";
        }
        else {
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                send(it->first, buffer, strlen(buffer), 0);
            }
            continue;
        }

        for (auto it = clients.begin(); it != clients.end(); ++it) {
            send(it->first, text.c_str(), text.length(), 0);
        }

        //logWriter(text);
        count_of_move = 0;
        gamer = true;
        clearMap();  
    }
    std::cout<<"before closing socket\n";
    close(socket);
}

//Очистка логов клиента
void clearLogClients(std::string config_name){
    std::ofstream tmp(get_logName(config_name));
    tmp.close();
}

//Установка порта
void getPort(std::string config_name){
    std::ofstream log(get_logName(config_name),std::ios::app);     
    std::ifstream config(config_name);
    if (!config.is_open()){
        const char* error = "No config file!";
        logWriter(error,config_name);
        log.close();
        std::cout << error << std::endl;
        exit(0);
    }
    std::string string;
    while (config.peek() != EOF){
        std::getline(config, string);
        if (string.find("port: ") != std::string::npos){
            std::string num;
            for (int i = 6; i < string.length(); ++i){
                num += string[i];
            }
            try {
                port = std::stoi(num);
                logWriter("Installed port: " + num,config_name);
            } catch(std::invalid_argument const& ex){
                logWriter(ex.what(),config_name);
                std::cout << ex.what() << std::endl;
                config.close();
                log.close();
                exit(0);
            }
            break;
        }        
    }
    config.close();
}

//Запуск сервера
int openServer(std::string config_name){
    int server_fd;
    std::ofstream log(get_logName(config_name),std::ios::app);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "\nsocket failed\n";
        logWriter("socket failed",config_name);
        log.close();
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = INADDR_ANY;
    std::string s = get_address(config_name);
    char* c = new char[s.length() + 1];
    strcpy(c, s.c_str());
    address.sin_addr.s_addr = inet_addr(c);
    getPort(config_name);
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cout << "\nbind failed\n";
        logWriter("bind failed",config_name);
        log.close();
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        std::cout << "\nlisten\n";
        logWriter("listen",config_name);
        log.close();
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is running...\n";  
    logWriter("Server is running...",config_name);
    return server_fd;
}

//Определение первого хода
bool firstMove(){
    srand((int)time(0));
    return rand() % 2 == 1 ? true : false;
}

//Подключение клиентов
void connectionClients(int socket, bool &gamer_l,std::string config_name){
    char buffer[1024] = {0};

    read(socket, buffer, 1024);
    std::string Nickname = buffer;
    memset(buffer, 0, sizeof(buffer));
    read(socket, buffer, 1024);
    std::string Password = buffer;        

    if (checkNewClientExist(Nickname, Password,config_name)){
        send(socket, "Success connect!\n", sizeof("Success connect!\n"), 0);
    } 
    else {
        send(socket, "Unknown user!\n", sizeof("Unknown user!\n"), 0);
        logWriter("Unknown user try to connect to the server!",config_name);
        return;
    }
    
    if (checkNewClientNotOld(Nickname)){
        std::cout << "\033[0;32mClient " << Nickname << " connected!\n\033[0m";
        logWriter(Nickname + " success connected to the server!",config_name);        

        clients.insert(std::pair<int, std::string>(socket, Nickname));
        queue.insert(std::pair<int, bool>(socket, gamer_l));

        if (clients.size() == 1){   
            send(socket, "Wait other player!\n", sizeof("Wait other player!\n"), 0);
            logWriter(Nickname + " waiting other player!",config_name);
        }

        if (clients.size() == 2) {
            clearMap();
            std::string first_move;

            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if (queue[it->first] == true){
                    first_move = "First move is " + clients[it->first];
                    logWriter(first_move,config_name);
                }
            }
            bool playing = true;
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                send(it->first, ("Game statred! " + first_move).c_str(), first_move.length() + sizeof("Game statred! "), 0);
            }

            usleep(100000);

            for (auto it = clients.begin(); it != clients.end(); ++it) {
                send(it->first, queue[it->first] ? "1" : "0", 2, 0);                
            }

            for (auto it = clients.begin(); it != clients.end(); ++it) {
                std::thread clientThread(clientHandler, it->first);
                clientThread.detach();
            }
        }
        gamer_l = !gamer_l;            
    }
    else {
        Nickname = "Client " + Nickname + " is also connected to server!\n";
        send(socket, Nickname.c_str(), Nickname.length(), 0);
        logWriter(Nickname,config_name);
    }
}

int main(int argc, char* argv[]) {
    std::string config_name(argv[1]);
    clearLogClients(config_name);
    int socket;
    int server_fd = openServer(config_name);   
    
    bool gamer_l = firstMove();
    
    while (true) {
        if ((socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        connectionClients(socket, gamer_l,config_name); 
    }
    return 0;
}
