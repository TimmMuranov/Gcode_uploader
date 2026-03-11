
#ifndef UPLOADER_HPP
#define UPLOADER_HPP

#include <iostream>
#include <string>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

class Uploader {
private:
    int fd = -1;

public:
    bool connect(const std::string& device) {
        fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (fd < 0) return false;

        struct termios tty;
        if (tcgetattr(fd, &tty) != 0) return false;

        cfsetospeed(&tty, B115200);
        cfsetispeed(&tty, B115200);

        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_lflag = 0;
        tty.c_oflag = 0;
        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 10; 

        if (tcsetattr(fd, TCSANOW, &tty) != 0) return false;

        sleep(2);
        tcflush(fd, TCIOFLUSH);
        return true;
    }

    bool send(std::string command) {
        if (fd < 0) return false;
        if (command.back() != '\n') command += "\n";
        if (write(fd, command.c_str(), command.size()) < 0) return false;
        std::string response;
        char buf[128];
        int attempts = 0;
        while (attempts < 20) {
            int n = read(fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                response += buf;
                if (response.find("ok") != std::string::npos) return true;
                if (response.find("error") != std::string::npos) {
                    std::cerr << "GRBL Error: " << response << std::endl;
                    return false;
                }
            }
            usleep(50000); // 50ms
            attempts++;
        }
        return false;
    }

    bool waitForIdle() {
        char buf[128];
        while (true) {
            write(fd, "?", 1);

            std::string status;
            usleep(100000); 

            int n = read(fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                status = buf;
                if (status.find("Idle") != std::string::npos) {
                    return true;
                }
            }
        }
    }

    ~Uploader() {
        if (fd >= 0) close(fd);
    }
};

#endif
