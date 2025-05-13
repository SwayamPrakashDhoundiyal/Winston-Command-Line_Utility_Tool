// main.cpp
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "headers/function.h"

using namespace std;

void printUsage(){
    setconsolecolor(2,"Usage: winston.exe <command>\n");
    cout << "Commands:\n";
    cout << "  updateSpice                    - Update or install Spicetify\n";
    //cout << "  settings set <key> <value>     - Set a setting (e.g., spicetify_auto_update true)\n";
    //cout << "  settings get <key>             - Get a setting's value\n";
    //cout << "  settings list                  - List all settings\n";
    cout << "  bluetooth on                   - Enable Bluetooth\n";
    cout << "  bluetooth off                  - Disable Bluetooth\n";
}

int main(int argc, char* argv[]) {
    showArt();
    cout<<"Type Help or help to get a list of commands.\n";
    while(true)
    {
        string input;
        cout<<">> ";
        getline(cin, input);
        lowercase(input);
        istringstream iss(input);
        vector<string> command;
        string word;

        while (iss >> word) {
            command.push_back(word);
        }

        if (command.size() < 1) {
            printUsage();
            continue;
        }else if (command.size() > 3)
        {
            printUsage();
            continue;
        }
        
        if(command[0] == "exit")
        {
            ClearConsole(true);
            return 0;
        }else if (command[0] == "updatespice") {
            installSpicetify();
        }else if (command[0] == "bluetooth") {
            if (command.size() < 2) {
                printUsage();
            }
            string subcommand = command[1];
            if (subcommand == "on") {
                if (!enablebluetooth()) {
                    setconsolecolor(0,"Retry in a bit.....");
                }
            }else if (subcommand == "off") {
                if (!disablebluetooth()) {
                    setconsolecolor(0,"Retry in a bit.....");
                }
            } else {
                cerr << "Error: Unknown bluetooth subcommand '" << subcommand << "'\n";
                printUsage();
            }
        }else if(command[0] == "clear")
        {
            if (command.size() < 2){
                ClearConsole(false);
            }else{
                if(command[1] == "full")
                {
                    ClearConsole(true);
                }else{
                    printUsage();
                }
            }
        }else if (command[0] == "taskpf")
        {
            showTaskPerf();
        }else {
            printUsage();
        }

    }
    return 0;
}