#include "client.h"

void clear_buffer(char* buffer){
    for(int i=0;i<1024;++i){
        buffer[i]=0;
    }
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

//Установка времени хода
void getTimeForMove(std::string config_name){
    std::ifstream config(config_name);
    std::ofstream log(get_logName(config_name),std::ios::app);
    if (!config){
        const char* error = "No config file!";
        std::cout << error << std::endl;
        logWriter(error,config_name);
        log.close();
        exit(0);
    }
    std::string string;
    while (config.peek() != EOF){
        std::getline(config, string);
        if (string.find("time_for_move: ") != std::string::npos){
            std::string num;
            for (int i = 15; i < string.length(); ++i){
                num += string[i];
            }
            try {
                time_for_move = std::stoi(num);
                logWriter("Installed time for move: " + num,config_name);
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

//Установка порта
void getPort(std::string config_name){
    std::ifstream config(config_name);
    std::ofstream log(get_logName(config_name),std::ios::app);
    if (!config){
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

bool get_flag(std::string config_name){
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
        if (string.find("auto: ") != std::string::npos){
            for (int i = 6; i < string.length(); ++i){
                name += string[i];
            }
            config.close();
            return (name=="yes");
        }        
    }
    config.close();
    return false;
}
std::string get_name(std::string config_name){
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
        if (string.find("name: ") != std::string::npos){
            for (int i = 6; i < string.length(); ++i){
                name += string[i];
            }
            config.close();
            return name;
        }        
    }
    config.close();
    return name;
}
std::string get_password(std::string config_name){
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
        if (string.find("pass: ") != std::string::npos){
            for (int i = 6; i < string.length(); ++i){
                name += string[i];
            }
            config.close();
            return name;
        }        
    }
    config.close();
    return name;
}
//Авторизация клиента
bool authorize(int socket,std::string config_name){
    char buffer[1024] = {0};
    std::ofstream log(get_logName(config_name),std::ios::app);
    bool automatic=get_flag(config_name);
    std::string nickname;
    if(automatic){
        //std::cout<<"auto\n";
        automatic=false;
        nickname = get_name(config_name);
        //std::cout<<nickname<<'\n';
        strcpy(buffer, nickname.c_str());
        send(socket, buffer, strlen(buffer), 0);
        std::string pass=get_password(config_name);
        //std::cout<<pass<<'\n';
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, pass.c_str());
        send(socket, buffer, strlen(buffer), 0);
    }
    else{
        std::cout << "\033[1m" << "Nickname: \033[0m";
        std::cin >> buffer;
        nickname = buffer;

        send(socket, buffer, strlen(buffer), 0);
        std::cout << "\033[1m" << "Password: \033[0m";
        std::cin >> buffer;
        send(socket, buffer, strlen(buffer), 0);
    }
    // std::cout << "\033[1m" << "Nickname: \033[0m";
    // std::cin >> buffer;
    // std::string nickname = buffer;

    // send(socket, buffer, strlen(buffer), 0);
    // std::cout << "\033[1m" << "Password: \033[0m";
    // std::cin >> buffer;
    // send(socket, buffer, strlen(buffer), 0);

    //std::system("clear");

    memset(buffer, 0, sizeof(buffer));
    int valread = read(socket, buffer, 1024);
    if (valread <= 0) {
        logWriter("Invalid message from server on authorize client",config_name);
        log.close();
        exit(0);
    }

    if (strcmp(buffer, "Success connect!\n") == 0){
        logWriter(nickname + " successfully connect",config_name);
        std::cout << "\033[1;32m" << buffer << "\033[0m";
        return true;
    }
    
    logWriter("Unknown user try to connect to the server!",config_name);
    std::cout << "\033[1;31m" << "Unknown user!\n" << "\033[0m";
    return false;    
}

//Заполнение игрового поля по буфферу, полученному с сервера
void fillGameMap(const char* buffer){
    for (int i = 0, j = 0; i < 3; ++i){
        for (int k = 0; k < 3; ++k, ++j){
            game_map[i][k] = buffer[j];
        }
    }
}

//Вывод игрового поля
void printGameMap(){
    std::cout << "\033[1m";
    for (int i = 0; i < 3; ++i){
        for (int k = 0; k < 3; ++k){
            if ( k == 0){ 
                std::cout << " "; 
            }
            if (game_map[i][k] == 'X'){
                if (k != 2){ 
                    std::cout << "\033[0;31m"<<  game_map[i][k] << "\033[0m" << " ┃ ";
                }
                else { 
                    std::cout << "\033[0;31m"<<  game_map[i][k] << "\033[0m" << std::endl;
                }
            }
            else {
                if (k != 2){ 
                    std::cout << "\033[0;34m"<<  game_map[i][k] << "\033[0m" << " ┃ ";
                }
                else { 
                    std::cout << "\033[0;34m"<<  game_map[i][k] << "\033[0m" << std::endl;
                }
            }
                        
        }
        if (i != 2) { std::cout << "―――――――――――\n";}
    }
    std::cout << "\033[0m";
}

//Обработка сообщений с сервера
void receiveMessages(int socket) {
    int valread;
    char buffer[1024] = {0};
    while (true) {
        valread = read(socket, buffer, 1024);
        if (valread > 0) {
            //std::system("clear");
            //Игра закончилась, выводим победителя и выходим из цикла
            if (strcmp(buffer, "O winner!") == 0 || 
                strcmp(buffer, "X winner!") == 0 || 
                strcmp(buffer, "Draw!") == 0) {
                    //logWriter(buffer);
                    std::cout << "\033[1m" << buffer << "\033[0m" << std::endl;
                    break;
            }
            //Пользователь ввел некорректное значение
            else if (strcmp(buffer, "Wrong input!\n") == 0) {
                std::cout << "\033[1;31m" << buffer << "\033[0m" << std::endl;
                //logWriter("Wrong input");
            }
            //Игра продолжается
            else {
                //logWriter("Next move!");
                fillGameMap(buffer);
                gamer = !gamer;
            }            
            printGameMap();        
        }
        else {
            //logWriter("Invalid message from server on recieveMessages!");
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    close(socket);
    //log.close();
    exit(0); //Игра закончилась, выходим из программы
}

//Отправка значения хода, введенное пользователем
void sendMove(int socket,std::string config_name){   
    while (true) {
        //Если ход другого игрока, то пропускаем ввод
        if (move != gamer){
            continue; 
        }
        std::string message;
        // Создание асинхронной задачи для чтения ввода
        auto readInput = std::async(std::launch::async, []() {
            std::string input;
            std::cin >> input;
            return input;
        });

        // Установка времени ожидания ввода, которое указано в конфиге
        std::future_status status = readInput.wait_for(std::chrono::seconds(time_for_move));

        // Проверка состояния ожидания
        if (status == std::future_status::ready) {
            // Ввод был завершен до истечения времени
            message = readInput.get();
        } else if (status == std::future_status::timeout) {
            // Ввод не был завершен до истечения времени
            std::cout << "Input timed out." << std::endl;
            logWriter("Input timed out!",config_name);
            send(socket, "-1", 3, 0);            
        }
        int value;
        try {
            value = std::stoi(message);
        } catch(std::invalid_argument const& ex){
            //Пользователь ввел не число
            std::cout << "\033[0;31mIncorrect input: " << ex.what() << "\n\033[0m";
            logWriter(std::string("Incorrect input: ") + ex.what(),config_name);
            continue;
        }
        if (value < 1 || value > 9){
            //Пользователь ввел некорректное число
            std::cout << "\033[0;31mIncorrect input: input number in range (1 <= value <= 9)\n\033[0m";
            logWriter("Incorrect input: input number in range (1 <= value <= 9)",config_name);
            continue;
        }    
        //Если все хорошо, то отправляем значение, введенное пользователем, на сервер
        send(socket, message.c_str(), message.length(), 0);
    }
}

//Выводим сообщение-приветствие
void printHelloMessage() {
    std::cout << "\033[1m" << "\nHey! Nice to meet you in my Tic Tac Toe Game!!!\n" << "\033[0m"
                 "――――――――――――――――――――――――――――――――――――――――――――――――――――――\n" 
                 "Our rules:\n" 
                 "1. The cross always goes first. The user's figure is determined randomly.\n"
                 "2. You have " << "\033[1;34m" << time_for_move << "\033[0m" << " seconds to make your move. Didn't have time? So You lost.\n"
                 "3. To put your figure, enter a number from 1 to 9, where the fields of the game are numbered like this:\n"
                 "1 | 2 | 3\n"
                 "―――――――――\n"
                 "4 | 5 | 6\n"
                 "―――――――――\n"
                 "7 | 8 | 9\n\n"
                 "--& Have a nice game! &-- \n"
                 "To continue, you need to" << "\033[1;32m" << " log in.\n" << "\033[0m"
                 "Write 'go' if you are ready for battle!\n"
                 "Write 'exit' if you are not ready for battle!\n";   
}

//Пользователь решает хочет ли он сыграть в Крестики-Нолики
bool startGame(std::string config_name) {
    std::string inp;
    
    while(true){
        std::cin >> inp;
        if (inp == "exit") {
            logWriter("The client is out of the game",config_name);
            return 0;
        }
        if (inp == "go"){
            logWriter("The client has logged into the game",config_name);
            return 1;;
        }
        std::cout << "Incorrect input! Write 'exit' for exit or 'go' for continue\n";
    }    
}

//Ожидание второго игрока
void waitOtherPlayer(int socket,std::string config_name) {
    char buffer[1024] = {0};
    int valread = read(socket, buffer, sizeof(buffer));
    if (valread > 0){
        if (strcmp(buffer, "Wait other player!\n") == 0){
            std::cout << buffer << std::endl;
            logWriter("Client wait other player!",config_name);
            memset(buffer, 0, 1024);
            valread = read(socket, buffer, sizeof(buffer));
            if (valread > 0) {
                //std::system("clear");  
                logWriter(buffer,config_name);  
                std::cout << buffer << std::endl;
            }
        }
        else {
            //std::system("clear");    
            logWriter(buffer,config_name);  
            std::cout << buffer << std::endl;
        }            
    }
}

//Определение очередности хода
void getMove(int socket) {
    char buffer[1024] = {0};
    int valread = read(socket, buffer, sizeof(buffer));
    if (strcmp(buffer, "1") == 0){
        move = true;
    }
    else {
        move = false;
    }
}

//Подключение к серверу
int connect2Server(std::string config_name){
    int sock = 0;
    struct sockaddr_in serv_addr;
    std::ofstream log(get_logName(config_name),std::ios::app);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\n Socket creation error \n";
        logWriter("Socket creation error",config_name);
        log.close();
        return -1;
    }

    getPort(config_name);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "\nInvalid address/ Address not supported \n";
        logWriter("Invalid address/ Address not supported",config_name);
        log.close();
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed \n";
        logWriter("Connection Failed",config_name);
        log.close();
        return -1;
    }
    return sock;
}

int main(int argc, char* argv[]) {
    std::string config_name(argv[1]);
    int sock = connect2Server(config_name);
    if (sock == -1){
        return -1;
    }

    logWriter("Succecss connect to server",config_name);

    getTimeForMove(config_name);
    printHelloMessage();

    if (!startGame(config_name)) {
        close(sock);
        exit(0);
    }    

    //std::system("clear");

    if (authorize(sock,config_name) == 0){
        close(sock);
        exit(0);
    }
    waitOtherPlayer(sock,config_name);
    getMove(sock);
    printGameMap();
    std::thread receiveThread(receiveMessages, sock);
    receiveThread.detach();
    sendMove(sock,config_name);    
    return 0;
}
