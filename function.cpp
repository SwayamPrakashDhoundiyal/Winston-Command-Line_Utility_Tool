#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <deque>
#include <windows.h>
#include <pdh.h>
#include <bluetoothapis.h>
#include <setupapi.h>
#include <devguid.h>
#include <iomanip>
#include <conio.h>
#include "headers/defination.h"
#include "headers/function.h"

using namespace std;

//For showing ascii art
void showArt(){
    setconsolecolor(1,STARTING_ART,"\n");
}

void ClearConsole(const bool &showart) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written, cells;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    cells = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cells, {0, 0}, &written);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cells, {0, 0}, &written);
    SetConsoleCursorPosition(hConsole, {0, 0});

    if (!showart){
        showArt();
    }
}

void installSpicetify() {
    cout << "Initiating Spicetify installation...\n";
    int result = system(SPICETIFY_INSTALL_COMMAND);
    if (result == 0) {
        setconsolecolor(1,"Spicetify installation completed successfully.\n");
    } else {
        setconsolecolor(0,"Error: Spicetify installation failed with exit code",result,".\n");
        setconsolecolor(0,"Please ensure PowerShell execution policy allows scripts (e.g., run 'Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned').\n");
    }
}

// Helper function to get device property
wstring GetDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA* pDeviceData, DWORD property) {
    DWORD requiredSize = 0;
    SetupDiGetDeviceRegistryPropertyW(hDevInfo, pDeviceData, property, nullptr, nullptr, 0, &requiredSize);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return L"";

    wstring buffer(requiredSize / sizeof(WCHAR), L'\0');
    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, pDeviceData, property, nullptr, (PBYTE)buffer.data(), requiredSize, nullptr)) {
        return buffer;
    }
    return L"";
}

//enable bluetooth and handle its corresponding errors if any
bool enablebluetooth(){
    setconsolecolor(1,"Enabling Bluetooth.....\n");

    setconsolecolor(1,"Calling devcon to enable Bluetooth adapter...\n");
    wstring devconCmd = L"devcon enable \"BTH*\" >nul 2>&1"; //initial command for the bluetooth to start and tell the adapter to wakeup
    int devconResult = _wsystem(devconCmd.c_str());
    if (devconResult != 0) {
        setconsolecolor(0, "Error: Failed to enable Bluetooth adapter. devcon exit code: ", devconResult, ".\n");
        setconsolecolor(0, "Ensure devcon.exe is in PATH or build/Release and run as administrator.\n");
        //return false;
    }
    setconsolecolor(2,"Waiting for the device to reboot......"); //waiting for the bluetooth device to reboot properly
    Sleep(2000);
    devconResult = system("devcon enable \"BTH*\""); //command for devcon to enable the device
    if (devconResult != 0) {
        setconsolecolor(0, "Error: Failed to enable Bluetooth adapter. devcon exit code: ", devconResult, ".\n");
        setconsolecolor(0, "Ensure devcon.exe is in PATH or build/Release and run as administrator.\n");
        return false;
    }
    return true;
}

// Disable Bluetooth adapter
bool disablebluetooth() {
    setconsolecolor(1, "Disabling Bluetooth...\n");

    // Find Bluetooth adapter
    int devconResult = system("devcon disable \"BTH*\"");
        if (devconResult != 0) {
            setconsolecolor(0, "Error: Failed to disable Bluetooth adapter. devcon exit code: ", devconResult, ".\n");
            setconsolecolor(0, "Ensure devcon.exe is in PATH or build/Debug and run as administrator.\n");
            return false;
        }
        setconsolecolor(1, "Disabled Bluetooth successfully!\n");
        wcout << L"Disabled Bluetooth adapter.\n";

    return true;
}

// Performance monitoring function with real-time updates
void showTaskPerf() {
    // Initialize PDH query
    PDH_HQUERY cpuQuery, memQuery, gpuQuery;
    PDH_HCOUNTER cpuCounter, memCounter, gpuCounter;
    PdhOpenQuery(nullptr, 0, &cpuQuery);
    PdhOpenQuery(nullptr, 0, &memQuery);
    PdhOpenQuery(nullptr, 0, &gpuQuery);

    // Add counters
    PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuCounter);
    PdhAddEnglishCounter(memQuery, "\\Memory\\% Committed Bytes In Use", 0, &memCounter);
    PdhAddEnglishCounter(gpuQuery, "\\GPU Engine(pid_0_*_engtype_3D)\\Utilization Percentage", 0, &gpuCounter);

    // Collect initial data
    PdhCollectQueryData(cpuQuery);
    PdhCollectQueryData(memQuery);
    PdhCollectQueryData(gpuQuery);

    // Store usage history (max 20 points for graph)
    deque<double> cpuHistory, memHistory, gpuHistory;
    const int maxHistory = 20;
    const int graphHeight = 10;

    // Get console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Initial display setup
    ClearConsole(true); // Clear once at start
    setconsolecolor(1, "Performance Metrics:\n");

    COORD initialPos;
    initialPos.X = 0;
    initialPos.Y = 20;
    SetConsoleCursorPosition(hConsole, initialPos);

    // Track cursor positions for labels
    vector<COORD> labelPos;

    //Draw the graph with calculations
    auto drawGraphInitial = [&](int startX,const string& label,int startY = -1){
        int iniY = initialPos.Y;
        initialPos.X = startX;
        if (startY != -1) initialPos.Y = startY;
        initialPos.Y += 2;
        for(int h = graphHeight; h>=0 ; --h)
        {
            SetConsoleCursorPosition(hConsole, initialPos);
            if (h == 10) cout<<(h*100/graphHeight)<<"% |";
            else if(h==0) cout<<(h*100/graphHeight)<<"%   |";
            else cout<<(h*100/graphHeight)<<"%  |";
            initialPos.Y +=1;
        }
        //initialPos.Y -= 1;
        initialPos.X += 5;
        SetConsoleCursorPosition(hConsole, initialPos);
        cout<<"|---------------------------------------->";
        initialPos.Y +=1;
        SetConsoleCursorPosition(hConsole, initialPos);
        cout<<"|";
        initialPos.X += 3;
        initialPos.Y += 1;
        SetConsoleCursorPosition(hConsole, initialPos);
        setconsolecolor(2,label);
        int space = label.length() + 2;
        labelPos.push_back({initialPos.X += space,initialPos.Y});
        initialPos.X = 0;
        if (startY != -1) initialPos.Y = iniY;
    };

    drawGraphInitial(0,"CPU Usage Graph");
    drawGraphInitial(55,"Memory Usage Graph",20);
    drawGraphInitial(0,"GPU Usage Graph");

    // putting exit instruction
    initialPos.X = 5;
    initialPos.Y += 2;
    SetConsoleCursorPosition(hConsole, initialPos);
    setconsolecolor(0,"Press 'q' to exit");
    
    //bar drawing starting point
    initialPos.X = 0;
    initialPos.Y = 22;
    SetConsoleCursorPosition(hConsole, initialPos);

    while (true)
    {
        PdhCollectQueryData(cpuQuery);
        PdhCollectQueryData(memQuery);
        PdhCollectQueryData(gpuQuery);

        PDH_FMT_COUNTERVALUE cpuValue, memValue, gpuValue;
        PdhGetFormattedCounterValue(cpuCounter, PDH_FMT_DOUBLE, nullptr, &cpuValue);
        PdhGetFormattedCounterValue(memCounter, PDH_FMT_DOUBLE, nullptr, &memValue);
        PdhGetFormattedCounterValue(gpuCounter, PDH_FMT_DOUBLE, nullptr, &gpuValue);

        double cpuUsage = cpuValue.doubleValue;
        double memUsage = memValue.doubleValue;
        double gpuUsage = gpuValue.doubleValue;

        // Store in history
        cpuHistory.push_back(cpuUsage);
        memHistory.push_back(memUsage);
        gpuHistory.push_back(gpuUsage);
        if (cpuHistory.size() > maxHistory) {
            cpuHistory.pop_front();
            memHistory.pop_front();
            gpuHistory.pop_front();
        }
        SetConsoleCursorPosition(hConsole, labelPos[0]);
        cout << fixed << setprecision(1) << cpuUsage << "%  ";
        SetConsoleCursorPosition(hConsole, labelPos[1]);
        cout << fixed << setprecision(1) << memUsage << "%  ";
        SetConsoleCursorPosition(hConsole, labelPos[2]);
        cout << fixed << setprecision(1) << gpuUsage << "%  ";

        //updating graph
        auto updateGraph = [&](const deque<double>& history, int startY,int startX) {
            COORD Pos;
            Pos.X = startX;
            Pos.Y = startY;
            SetConsoleCursorPosition(hConsole, Pos);
            
            for(int h = graphHeight; h >=0; --h){
                Pos.X = startX;
                string* lineFill = new string;
                for(auto value = history.rbegin(); value != history.rend(); ++value){
                    int barHeight = (*value * graphHeight)/100 ;
                    char barFill = char(219);
                    if (barHeight < 1){
                        barFill = char(220);
                    }
                    *lineFill += h<=barHeight ? string {barFill,barFill} : string {' ' , ' '};
                }
                int missing = maxHistory - history.size();
                for (int i = 0; i < missing; ++i) {
                    *lineFill += ' '; // pad to keep length constant
                }
                setconsolecolor(1,*lineFill);
                Pos.Y += 1;
                SetConsoleCursorPosition(hConsole, Pos);
                delete lineFill;
            }
        };
        updateGraph(cpuHistory, 22,6);
        updateGraph(memHistory, 22,61);
        updateGraph(gpuHistory, 37,6);

        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q') break;
        }

        Sleep(1000); // Update every second
    }
    ClearConsole(true);
    showArt();
}



string lowercase(string& s)
{
    for (auto& x : s) { 
        x = tolower(x); 
    } 
    return s;
}