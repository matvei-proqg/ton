#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>
#include <random>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

class Node {
public:
    std::string id;
    std::unordered_map<std::string, Node*> routingTable; // Таблица маршрутизации
    std::mutex mtx; // Мьютекс для защиты данных

    Node(std::string nodeId) : id(nodeId) {}

    // Добавление маршрута
    void addRoute(Node* node) {
        std::lock_guard<std::mutex> lock(mtx);
        routingTable[node->id] = node;
    }

    // Отправка сообщения с использованием Onion Routing
    void sendOnionMessage(const std::vector<std::string>& route, const std::string& message) {
        // Создание "луковичного" пакета
        std::string onionPacket = createOnionPacket(route, message);
        std::cout << "Node " << id << " sending onion packet: " << onionPacket << std::endl;

        // Отправка пакета первому узлу в маршруте
        routingTable[route[0]]->receiveOnionPacket(route, onionPacket);
    }

    // Получение "луковичного" пакета
    void receiveOnionPacket(const std::vector<std::string>& route, const std::string& onionPacket) {
        // Снятие одного уровня шифрования
        std::string decryptedMessage = decryptOnionPacket(onionPacket);

        if (route.size() > 1) {
            // Передача следующему узлу в маршруте
            std::vector<std::string> nextRoute(route.begin() + 1, route.end());
            std::cout << "Node " << id << " forwarding to next node: " << nextRoute[0] << std::endl;
            routingTable[nextRoute[0]]->receiveOnionPacket(nextRoute, decryptedMessage);
        } else {
            // Достигнут конечный узел
            std::cout << "Node " << id << " received final message: " << decryptedMessage << std::endl;
        }
    }

private:
    // Создание "луковичного" пакета
    std::string createOnionPacket(const std::vector<std::string>& route, const std::string& message) {
        std::string onionPacket = message;
        for (const std::string& nodeId : route) {
            onionPacket = encryptMessage(onionPacket, nodeId); // Шифрование для каждого узла
        }
        return onionPacket;
    }

    // Шифрование с использованием AES
    std::string encryptMessage(const std::string& message, const std::string& nodeId) {
        // Генерация ключа и вектора инициализации
        unsigned char key[16]; // 128-битный ключ
        unsigned char iv[AES_BLOCK_SIZE]; // Вектор инициализации
        RAND_bytes(key, sizeof(key));
        RAND_bytes(iv, sizeof(iv));

        // Шифрование
        AES_KEY encryptKey;
        AES_set_encrypt_key(key, 128, &encryptKey);
        std::vector<unsigned char> encrypted(message.size() + AES_BLOCK_SIZE);
        int numBytesEncrypted;
        AES_cfb128_encrypt(reinterpret_cast<const unsigned char*>(message.c_str()), encrypted.data(), message.size(), &encryptKey, iv, &numBytesEncrypted, AES_ENCRYPT);

        return std::string(reinterpret_cast<char*>(encrypted.data()), encrypted.size());
    }

    // Расшифровка "луковичного" пакета
    std::string decryptOnionPacket(const std::string& onionPacket) {
        // Реализуйте аналогичную логику для расшифровки
        return onionPacket; // Заглушка
    }
};

class Network {
public:
    std::vector<Node*> nodes;
    std::mutex mtx;

    // Добавление узла в сеть
    void addNode(Node* node) {
        std::lock_guard<std::mutex> lock(mtx);
        nodes.push_back(node);
    }

    // Соединение узлов
    void connectNodes(Node* node1, Node* node2) {
        node1->addRoute(node2);
        node2->addRoute(node1);
    }

    // Отправка "луковичного" сообщения
    void sendOnionMessage(const std::string& fromId, const std::vector<std::string>& route, const std::string& message) {
        for (Node* node : nodes) {
            if (node->id == fromId) {
                node->sendOnionMessage(route, message);
                return;
            }
        }
        std::cout << "Node " << fromId << " not found in the network." << std::endl;
    }
};

int main() {
    Network network;

    // Создание узлов
    Node* nodeA = new Node("A");
    Node* nodeB = new Node("B");
    Node* nodeC = new Node("C");
    Node* nodeD = new Node("D");

    // Добавление узлов в сеть
    network.addNode(nodeA);
    network.addNode(nodeB);
    network.addNode(nodeC);
    network.addNode(nodeD);

    // Соединение узлов
    network.connectNodes(nodeA, nodeB);
    network.connectNodes(nodeB, nodeC);
    network.connectNodes(nodeC, nodeD);

    // Отправка "луковичного" сообщения
    std::vector<std::string> route = {"B", "C", "D"}; // Маршрут через узлы B, C и D
    network.sendOnionMessage("A", route, "Hello from A!");

    // Освобождение памяти
    delete nodeA;
    delete nodeB;
    delete nodeC;
    delete nodeD;

    return 0;
}


    // Освобождение памяти
    delete nodeA;
    delete nodeB;
    delete nodeC;

    return 0;
}
