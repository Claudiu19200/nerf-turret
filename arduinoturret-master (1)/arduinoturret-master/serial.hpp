#pragma once

#include <Windows.h>
#include <string>

HANDLE _openPort(int n, int baud);
void _closePort(HANDLE serialPort);
bool _sendData(HANDLE serialPort, const std::string s);
bool _receiveData(HANDLE serialPort, std::string& s, const int len);
