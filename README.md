
# Tic-tac-toe Game
Крестики-Нолики онлайн!
## Описание
Данный код разработан для игры в Крестики-Нолики онлайн. В разработке использовались TCP-сокеты под операционную систему linux. Пока что могут играть только 2 человека одновременно, но в будущем планируется реализация проекта для многопользовательской игры с выбором оппонента.
## Струкутра клиентской части
За клиента отвечают файлы `client.cpp` и `client.h` 
### `client.h`:
В данном файле хранятся библиотеки, которые используются в приложении и глобальные переменные: время на ход, порт, состояние текущего хода, состояние хода определенного игрока, переменная, для открытия файла с логами клиента и игровое поле.
### `client.cpp`:
В данном файле реализованы следующие функции:
1. `void logWriter(std::string buffer)` - функция записи действий в лог файл. На вход подается сообщение с действием. 
2. `void getTimeForMove()` - функция считывания значения времени на ход с кофигурационного файла.
3. `void getPort()` - функция считывания значения порта с кофигурационного файла.
4. `bool authorize()` - функция авторизации клиента. Если введены правильный логин и пароль, то возвращает `true`, иначе `false`.
5. `void fillGameMap(const char* buffer)` - функция заполнения игрового поля по буферу, полученному с сервера. На вход подается буфер.
6. `void printGameMap()` - функция печати игрового поля в консоль.
7. `void recieveMessages(int socket)` - функция обработки сообщений с сервера. На вход подается значение сокета соответствующего клиента.
8. `void sendMove(int socket)` - функция отправки хода пользователя, который он ввел в консоль. На вход подается значение сокета, присвоенного этому клиенту.
9. `void printHelloMessage()` - функция печати приветственного сообщения.
10. `bool startGame()` - функция начала игры. Пользователь решает, хочет ли он сыграть в Крестики-Нолики. Если да, то функция вернет `true`, иначе функция вернет `false`.
11. `void WaitOtherPlayer()` - функция ожидания второго игрока.
12. `void getMove(int socket)` - функция определения текущего статуса хода игрока - Крестик или Нолик. На вход подается значение сокета, присвоенного этому клиенту.
13. `int connect2Server()` - функция подключения клиента к серверу. Возвращает значение сокета, присвоенного этому клиенту.
14. `int main()` - основная функция.

## Струкутра серверной части
### `server.h`:
В данном файле хранятся библиотеки, которые используются в приложении и глобальные переменные: вектор, хранящий клиентов онлайн, вектор, хранящий значения ходов клиентов, количество ходов, порт, адрес сервера, переменная, для открытия файла с логами сервера, игровое поле и состояние текущего хода.

### `server.cpp`:
В данном файле реализованы следующие функции:
1. `void logWriter(std::string buffer)` - функция записи действий в лог файл. На вход подается сообщение с действием. 
2.  `void clearMap()` - функция очистки игрового поля.
3. `int winner()` - функция определения победителя на каждом ходу. Возвращает `1`, если победили Крестики, `-1`, если победили Нолики, `0`, если еще никто не победил.
4. `bool ticTacToe(int coord)` - функция заполнения игрового поля фигурой пользователя по соответствующей координате, которая подается на вход. Возвращает `false`, если пользователь пытается занять клетку, на которой уже стоит другая фигура, иначе возвращает `true`.
5. `bool checkNewClientsExists(std::string Nickname, std::string Password)` - функция проверки существования клиента в конфигурационном файле. На вход подается логин и пароль. Возвращает `true`, если находит клиента с таким же именем и паролем, иначе возвращает `false`.
6. `bool checkNewClientNotOld(std::string Nickname)` - функция проверки того, что клиент не пытается зайти под логином, который уже в сети. На вход подается логин. Возвращает `true`, если клиента с таким именем еще нет в сети, иначе возвращает `false`.
7. `void fillBuffer(char* buffer)` - функция заполнения буфера по элементам с игрового поля. На вход подается указатель на буфер.
8. `void clientHandler(int socket)` - функция обработки сообщений с клиента. На вход подается значения сокета соответствующего клиента.
9. `void clearLogClients()` - функция очистки логов клиента.
10. `void getPort()` - функция считывания значения порта с кофигурационного файла.
11. `int openServer()` - функция открытия сервера. Возвращает значение дескриптора сокета.
12. `bool firstMove()` - функция определения первого хода. Если вернет `true`, то первым будет ходить клиент, который первый подключился, иначе - наоборот.
13. `void connectionClients(int socket, bool& gamer_l)` - функция подключения клиентов. На вход подается значение сокета соответствующего клиента и адрес переменной, содержащей информацию о состоянии хода текущего клиента.
14. `int main()` - основная функция 
## Компиляция
Компиляция проекта осуществляется через консоль: \
Компмляция сервера: `g++ server.cpp -o server` \
Компиляция клиента: `g++ client.cpp -o client` \
Сторонних библиотек в проекте нет!
## Запуск и работа с приложением

Сперва запускаем сервер: `./server`, затем запускаем первого клиента `./client` \
Авторизуемся на первом клиенте и запускаем второй: `./client` \
Выход из клиента производится автоматически по окончанию игры или по комбинации клавиш `ctrl + C` \
Выход из сервера осуществляется только по комбинации клавиш `ctrl + C`.
## Конфигурационный файл
В проекте присутствует конфигурационный файл `config.txt`. Там находится порт, по которому мы слушаем, время для хода игрока, логины и пароли пользователей.

## Лог-файлы
Все действия клиента записываются в его лог-файл: `log_clients.txt` \
Все действия на сервере также записываются в лог-файл: `log_server.txt`\
Логи создаются автоматически!

## Баги
Проект не идеалаен и в нем присутствуют баги: 
1. Если игра завершилась, потому что один из игроков не закончил ход вовремя, то победителем всегда будет нолик, а следующую игру вы не сможете начать, пока не перезапустите сервер.
2. Если выйти из клиента в то время, когда он ожидает второго игррока, то сервер не сможет отловить, что клиент отсоединился и поэтому дальнейшая работа приложения будет некорректна 
\
Больше багов пока что не было найдено, но если вы нашли, то пишите в тг: `@FergeS22`. 

Текущие баги будут фикситься по мере выхода новых версий. Проект не окончательный!
