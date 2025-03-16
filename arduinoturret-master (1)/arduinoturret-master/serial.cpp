#include "serial.hpp"

HANDLE _openPort(int n, int baud) {
    DCB dcb = {0};
    HANDLE serialPort;
    BOOL fSuccess;
    std::string pcCommPort;
    pcCommPort = "\\\\.\\COM" + std::to_string(n);

    serialPort = CreateFileA(
        pcCommPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL
    );

    if (serialPort == INVALID_HANDLE_VALUE) {
        return nullptr;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (SetCommTimeouts(serialPort, &timeouts) == FALSE) {
        return nullptr;
    }

    if (SetCommMask(serialPort, EV_RXCHAR) == FALSE) {
        return nullptr;
    }

    dcb.BaudRate = baud;
    dcb.DCBlength = sizeof(dcb);

    fSuccess = GetCommState(serialPort, &dcb);

    if (!fSuccess) {
        return nullptr;
    }

    fSuccess = SetCommState(serialPort, &dcb);

    if (!fSuccess) {
        return nullptr;
    }

    fSuccess = GetCommState(serialPort, &dcb);

    if (!fSuccess) {
        return nullptr;
    }
    return serialPort;
}

void _closePort(HANDLE serialPort) {
    CloseHandle(serialPort);
}

bool _sendData(HANDLE serialPort, const std::string s) {
    DWORD dNoOFBytestoWrite = s.length() + 1;
    DWORD dNoOfBytesWritten = 0;

    BOOL Status = WriteFile(
        serialPort, s.c_str(), dNoOFBytestoWrite, &dNoOfBytesWritten, NULL
    );

    if (!Status) {
        return false;
    }
    return true;
}

bool _receiveData(HANDLE serialPort, std::string& s, const int len) {
    DWORD dwEventMask = 0;
    DWORD NoBytesRead = 0;

    BOOL Status = WaitCommEvent(serialPort, &dwEventMask, NULL);

    if (!Status) {
        return false;
    }

    char* buffer = nullptr;
    buffer = new char[len];

    Status = ReadFile(serialPort, buffer, len, &NoBytesRead, NULL);

    if (!Status) {
        if (buffer) {
            delete[] buffer;
        }
        return false;
    }

    s.erase();
    for (int i = 0; i < NoBytesRead; ++i) {
        s.push_back(buffer[i]);
    }

    if (buffer) {
        delete[] buffer;
    }
    return true;
}
