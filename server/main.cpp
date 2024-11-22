#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <vector>
#include <string>
#include <chrono>
#include <regex>
#include <thread>
#include <direct.h>
#include <fcntl.h>

#pragma comment(lib, "Ws2_32.lib")

std::string currentPath = "C:";
const int PORT = 8080;

bool SaveScreenshotToBMP(const std::string& fileName) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    BITMAPINFOHEADER biHeader = {};
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = screenWidth;
    biHeader.biHeight = -screenHeight;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biSizeImage = ((screenWidth * 24 + 31) / 32) * 4 * screenHeight;

    BITMAPFILEHEADER bfHeader = {};
    bfHeader.bfType = 0x4D42;
    bfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + biHeader.biSizeImage;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::vector<BYTE> pixels(biHeader.biSizeImage);
    GetDIBits(hdcMemory, hBitmap, 0, screenHeight, pixels.data(), reinterpret_cast<BITMAPINFO*>(&biHeader), DIB_RGB_COLORS);

    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);

    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile) {
        std::cerr << "Unable to open file for saving screenshot!" << std::endl;
        return false;
    }

    outFile.write(reinterpret_cast<const char*>(&bfHeader), sizeof(bfHeader));
    outFile.write(reinterpret_cast<const char*>(&biHeader), sizeof(biHeader));
    outFile.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    outFile.close();


    return true;
}

void sendImage(SOCKET clientSocket, const std::string& imagePath) {
    std::ifstream imageFile(imagePath, std::ios::binary);
    if (!imageFile) {
        std::cerr << "Could not open the image file." << std::endl;
        return;
    }

    imageFile.seekg(0, std::ios::end);
    std::streamsize imageSize = imageFile.tellg();
    imageFile.seekg(0, std::ios::beg);

    uint32_t netImageSize = htonl(static_cast<uint32_t>(imageSize));
    send(clientSocket, reinterpret_cast<char*>(&netImageSize), sizeof(netImageSize), 0);

    char buffer[1024];
    while (imageFile.read(buffer, sizeof(buffer))) {
        send(clientSocket, buffer, sizeof(buffer), 0);
    }
    send(clientSocket, buffer, imageFile.gcount(), 0);

    imageFile.close();

}

void receiveCoordinates(SOCKET clientSocket) {

    char buffer[256];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';  // Null-terminate the received data
        std::regex coordRegex(R"(x:\s*(\d+),\s*y:\s*(\d+))");
        std::smatch matches;
        std::string data(buffer);

        // Check if the data matches the coordinate format
        if (std::regex_search(data, matches, coordRegex) && matches.size() == 3) {
            int x = std::stoi(matches[1].str());
            int y = std::stoi(matches[2].str());

            double cofX = 1366 / 620.0;
            double cofY = 768 / 320.0;

            // Move the cursor to the specified coordinates
            SetCursorPos(static_cast<int>(x * cofX), static_cast<int>(y * cofY));
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        // If parsing coordinates fails, simulate typing each character as a keypress
        else {
            if (data == "BACK_SPACE") {
                // Simulate backspace key press
                keybd_event(VK_BACK, 0, 0, 0);
                keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);
                std::cout << "Simulated BACK_SPACE key press." << std::endl;
            }
            else if (data == "ENTER") {
                // Simulate enter key press
                keybd_event(VK_RETURN, 0, 0, 0);
                keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                std::cout << "Simulated ENTER key press." << std::endl;
            }
            else
            {
                for (char ch : data) {
                    // Convert the character to a virtual keycode
                    SHORT vk = VkKeyScan(ch);
                    if (vk != -1) {
                        // Press the key
                        keybd_event(static_cast<BYTE>(vk), 0, 0, 0);
                        // Release the key
                        keybd_event(static_cast<BYTE>(vk), 0, KEYEVENTF_KEYUP, 0);
                    }
                    else {
                        std::cerr << "Cannot simulate key for character: " << ch << std::endl;
                    }
                }
            }
        }
    }
    else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
        std::cerr << "Error receiving data." << std::endl;
    }


}

unsigned int toNetworkByteOrder(unsigned int value) {
    unsigned char b1 = (value >> 24) & 0xFF;
    unsigned char b2 = (value >> 16) & 0xFF;
    unsigned char b3 = (value >> 8) & 0xFF;
    unsigned char b4 = value & 0xFF;
    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

std::string executeCommand(const std::string& command) {
    std::cout << command << std::endl;
    if (command.empty()) {
        return currentPath;
    }
    std::cout << command << std::endl;
    if (command.substr(0, 3) == "cd ") {

        std::string newPath = command.substr(3);
        if (newPath == "..") {
            size_t pos = currentPath.find_last_of("/");
            if (pos != std::string::npos) {
                currentPath = currentPath.substr(0, pos);
            }
        }
        else {
            std::string fullPath = currentPath + "/" + newPath;
            if (_chdir(fullPath.c_str()) == 0) {
                currentPath = fullPath;
                return currentPath;
            }
            else {
                return "Failed to change directory: " + fullPath;
            }
        }
        return currentPath;
    }
    else if (command.substr(0, 5) == "read ") {
        std::string filename = currentPath + "/" + command.substr(5);
        std::ifstream file(filename);

        if (!file.is_open()) {
            return "Failed to open file for reading: " + filename;
        }

        std::string line, result;
        while (std::getline(file, line)) {
            result += line + "\n";
        }
        file.close();
        return result;
    }
    else if (command.substr(0, 6) == "write ") {
        size_t spacePos = command.find(' ', 6);
        if (spacePos == std::string::npos) {
            return "Invalid write command format. Usage: write <filename> <text>";
        }

        std::string filename = currentPath + "/" + command.substr(6, spacePos - 6);
        std::string content = command.substr(spacePos + 1);

        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            return "Failed to open file for writing: " + filename;
        }

        file << content << "\n";
        file.close();
        return "Successfully wrote to " + filename;
    }

    char buffer[128];
    std::string result;
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return "Failed to run command";

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

int main() {


    std::string currentDir = "C:\\";
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }


    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    sockaddr_in clientAddress;
    int clientAddrLen = sizeof(clientAddress);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLen);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Client connection failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    bool desktopmod = false, consolemod = false;

    std::cout << "Client connected. Streaming images and handling input..." << std::endl;

    // Add your condition here (e.g., based on a specific event)

    while (true) {
        if (desktopmod) {
            // Save the screenshot and send it to the client
            std::cout << "image sending" << std::endl;
            if (SaveScreenshotToBMP("image.bmp")) {
                sendImage(clientSocket, "image.bmp");
                std::cout << "image sended" << std::endl;
            }
            else {
                std::cerr << "Failed to save screenshot!" << std::endl;
                break;

            }
            u_long mode = 1;
            ioctlsocket(clientSocket, FIONBIO, &mode);
            receiveCoordinates(clientSocket);
            mode = 0;
            ioctlsocket(clientSocket, FIONBIO, &mode);
            // Receive mouse position and keyboard input data

        }

        if (consolemod) {
            // Console mode handling
           // char buffer[256];
            int bytesReceived;
            char lengthBuffer[4]; // 4-byte length header

            // Step 1: Read the length header first
            bytesReceived = recv(clientSocket, lengthBuffer, sizeof(lengthBuffer), 0);
            if (bytesReceived <= 0) {
                std::cerr << "Failed to read length header or connection closed" << std::endl;

            }

            // Convert lengthBuffer to an integer
            int messageLength1;
            std::memcpy(&messageLength1, lengthBuffer, sizeof(messageLength1));
            messageLength1 = ntohl(messageLength1); // Convert from network byte order to host byte order if necessary

            // Step 2: Now, read the actual data based on the length header
            std::vector<char> buffer(messageLength1); // Buffer to hold the actual message data
            bytesReceived = recv(clientSocket, buffer.data(), messageLength1, 0);
            if (!bytesReceived <= 0) {


            }

            // Null-terminate and convert to string
            buffer.push_back('\0'); // Make sure it’s null-terminated
            std::string data(buffer.data());

            std::string output = executeCommand(data);
            unsigned int messageLength = output.size();
            unsigned int networkLength = toNetworkByteOrder(messageLength);

            // Send the length first (4 bytes)
            send(clientSocket, reinterpret_cast<char*>(&networkLength), sizeof(networkLength), 0);

            // Send the actual message data
            send(clientSocket, output.c_str(), messageLength, 0);
        }

        char buffer[256];
        u_long mode = 1;
        ioctlsocket(clientSocket, FIONBIO, &mode);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        mode = 0;
        ioctlsocket(clientSocket, FIONBIO, &mode);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';  // Null-terminate the received data

            // Use std::string for safer comparison
            std::string receivedData(buffer);
            std::cout << receivedData << std::endl;
            // Compare the received data with the "DESKTOP" string
            if (receivedData == "DESKTOP") {
                std::cout << "changet to desk" << std::endl;
                desktopmod = true;
                consolemod = false;
            }
            if (receivedData == "CONSOLE") {
                std::cout << "changed to console" << std::endl;
                consolemod = true;
                desktopmod = false;

            }
            if (receivedData == "MENU") {
                desktopmod = false;
                consolemod = false;
            }
        }


        // Sleep to achieve ~30 FPS
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
