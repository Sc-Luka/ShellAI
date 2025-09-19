#include <iostream>
#include <cstdio>
#include <limits>
#include <iomanip>
#include <string>
#include <algorithm>

// ANSI color codes
const std::string BLUE   = "\033[1;34m";
const std::string CYAN   = "\033[1;36m";
const std::string GREEN  = "\033[1;32m";
const std::string YELLOW = "\033[1;33m";
const std::string RED = "\033[1;31m";
const std::string RESET  = "\033[0m";

const void PrintWelcomeBanner(const std::string& os_type) {
    const int width = 42; // inner width
 
    auto repeat = [](const std::string& s, int n) -> std::string {
        std::string r;
        for (int i = 0; i < n; ++i) r += s;
        return r;
    };

    auto printLine = [&](const std::string& text, const std::string& color = "") {
        int padding = width - text.size();
        int pad_left = padding / 2;
        int pad_right = padding - pad_left;
        std::cout << BLUE << "║" 
                  << RESET
                  << repeat(" ", pad_left)
                  << color << text << RESET
                  << repeat(" ", pad_right)
                  << BLUE << "║\n" << RESET;
    };

    std::cout << BLUE << "╔" << repeat("═", width) << "╗\n" << RESET;
    printLine("AI Terminal Assistant v1.0", CYAN);
    printLine("Ask anything about system commands!", GREEN);
    printLine("OS detected: " + os_type, YELLOW);
    printLine("Type a command request and hit Enter.", CYAN);
    printLine("Press Enter on empty line to exit.", CYAN);
    std::cout << BLUE << "╚" << repeat("═", width) << "╝\n" << RESET;
}

std::string PrepareCommand(std::string cmd) {
    // --- Remove backticks and quotes ---
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '`'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '"'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\''), cmd.end());

    // --- Trim leading/trailing whitespace ---
    cmd.erase(0, cmd.find_first_not_of(" \t\n\r"));
    cmd.erase(cmd.find_last_not_of(" \t\n\r") + 1);

#ifdef _WIN32
    // --- Windows adjustments ---
    // Replace "ls" with "dir"
    size_t pos = cmd.find("ls ");
    if (pos != std::string::npos) cmd.replace(pos, 3, "dir");
    // Replace ~ with %USERPROFILE%
    pos = cmd.find("~");
    if (pos != std::string::npos) cmd.replace(pos, 1, std::string(getenv("USERPROFILE")));
#else
    // --- Linux adjustments ---
    // Expand ~ to HOME
    size_t pos = cmd.find("~");
    if (pos != std::string::npos) {
        const char* home = getenv("HOME");
        if (home) cmd.replace(pos, 1, home);
    }
#endif

    return cmd;
}

bool AskContinue() {
    std::string input;
    while (true) {
        std::cout << YELLOW << "\nDo you want to enter another command? (Y/n): " << RESET;
        std::getline(std::cin, input);

        // If input failed (e.g., EOF), clear error and continue
        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        // Remove leading/trailing spaces
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);

        // Default Yes if empty
        if (input.empty()) return true;

        // Check first character (case-insensitive)
        char c = std::tolower(input[0]);
        if (c == 'y') return true;
        if (c == 'n') return false;

        std::cout << RED << "Please enter Y or n.\n" << RESET;
    }
}

void PrintCommandOutput(FILE* cmd_pipe) {
    char buffer[256];
    int count = 0;

    // First, read everything into a string to check if it's multi-line
    std::string full_output;
    while (fgets(buffer, sizeof(buffer), cmd_pipe) != nullptr) {
        full_output += buffer;
    }

    // Re-open the pipe or split lines
    std::istringstream iss(full_output);
    std::string line;

    bool multiLine = full_output.find('\n') != std::string::npos;

    while (std::getline(iss, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        if (!multiLine && line.length() <= 20) {
            // short output -> columns
            std::cout << GREEN << std::setw(20) << std::left << line << RESET;
            if (++count % 4 == 0) std::cout << "\n";
        } else {
            // multi-line output -> print normally
            std::cout << GREEN << line << RESET << "\n";
        }
    }

    if (!multiLine && count % 4 != 0) std::cout << "\n"; // final newline for columns
}

int main(){
     // --- Detect OS ---
    #ifdef _WIN32
    const std::string os_type = "Windows";
    #else
    const std::string os_type = "Linux";
    #endif

    PrintWelcomeBanner(os_type);
   
    do {
        // --- Get user request ---
        std::string user_input;
        std::cout << YELLOW << "Enter your request: " << RESET;
        std::getline(std::cin, user_input);
        if (user_input.empty()) break; // exit if empty

        // --- Call Python AI script ---
        std::string python_command = "python AI_Bridge.py \"" + user_input + "\" " + os_type;
        FILE* pipe = popen(python_command.c_str(), "r");
        if (!pipe) {
            std::cerr << RED << "Failed to run AI script!" << RESET << "\n";
            continue;
        }

        // --- Read AI suggestion ---
        char buffer[256];
        std::string ai_command;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            ai_command += buffer;
        }
        pclose(pipe);

        // --- Trim newline ---
        ai_command.erase(std::remove(ai_command.begin(), ai_command.end(), '\n'), ai_command.end());

        // --- Print AI Suggestion ---
        std::cout << CYAN << "\nAI Suggests: " << ai_command << RESET << "\n";

        // --- Execute command with colored output ---
        FILE* cmd_pipe = popen(ai_command.c_str(), "r");
        if (!cmd_pipe) {
            std::cerr << RED << "Failed to execute command!" << RESET << "\n";
            continue;
        }

        PrintCommandOutput(cmd_pipe);
        pclose(cmd_pipe);

    }while (AskContinue());

    std::cout << GREEN << "\nExiting AI Terminal Assistant. Goodbye!\n" << RESET;
    return 0;
}