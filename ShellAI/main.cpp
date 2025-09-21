#include "Logger.cpp"
#include <iostream>
#include <cstdio>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cctype>
#include <string>
#include <unordered_set>
#include <sstream>


// ANSI color codes
namespace TerminalColors {
    const std::string BLUE   = "\033[1;34m";
    const std::string CYAN   = "\033[1;36m";
    const std::string GREEN  = "\033[1;32m";
    const std::string YELLOW = "\033[1;33m";
    const std::string RED    = "\033[1;31m";
    const std::string RESET  = "\033[0m";
}

// History
const std::string HISTORY_FILE = "ai_terminal_history.txt";
std::vector<std::string> commandHistory;
const size_t MAX_HISTORY_SIZE = 5000;

void addCommand(std::string command){
    if(command.empty()) 
        return;

    if(commandHistory.size() > MAX_HISTORY_SIZE )
        commandHistory.erase(commandHistory.begin());
    
    commandHistory.push_back(command);

    std::ofstream file(HISTORY_FILE, std::ios::app);
    if (file.is_open()) {
        file << command << "\n";
    } else {
        std::cerr << TerminalColors::RED << "Warning: Could not write command to history file!" << TerminalColors::RESET << "\n";
    }

}

bool isGibberishInput(const std::string& input) {
    if (input.empty()) return true;

    // Whitelist of valid single-word commands
    static const std::unordered_set<std::string> valid_commands = {
        "exit", "clear", "history"
    };

    // Trim input
    std::string trimmed = input;
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char c){ return !std::isspace(c); }));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), trimmed.end());

    // Allow valid commands
    if (valid_commands.count(trimmed)) return false;

    // Handle commands starting with ! or ~
    if (!trimmed.empty() && (trimmed[0] == '!' || trimmed[0] == '~')) {
        std::string after = trimmed.substr(1);

        // If it's only digits -> valid
        if (!after.empty() && std::all_of(after.begin(), after.end(), ::isdigit))
            return false;

        // If it's a sentence (contains space) -> valid
        if (after.find(' ') != std::string::npos)
            return false;

        // Otherwise -> gibberish
        return true;
    }

    // Count letters, digits, spaces, and special characters
    int letter_count = 0, digit_count = 0, special_count = 0, space_count = 0;
    std::string allowed_symbols = "-_./:\"'"; // allowed terminal chars

    for (char c : trimmed) {
        if (std::isalpha(c)) letter_count++;
        else if (std::isdigit(c)) digit_count++;
        else if (std::isspace(c)) space_count++;
        else if (allowed_symbols.find(c) != std::string::npos) {
            // treat as normal, not special
        }
        else special_count++;
    }

    int total_chars = trimmed.length() - space_count;

    // Gibberish if mostly symbols/numbers
    if (total_chars > 0 && letter_count < total_chars / 3) return true; // more tolerant

    // Long sequences of letters without vowels are gibberish
    std::string vowels = "aeiouAEIOU";
    if (letter_count >= 6) {
        bool has_vowel = std::any_of(trimmed.begin(), trimmed.end(), [&](char c){ return vowels.find(c) != std::string::npos; });
        if (!has_vowel) return true;
    }

    return false; // valid input
}


void loadHistory(){
    std::ifstream file(HISTORY_FILE);
    std::string line;
    while(getline(file, line)){
        if(!line.empty())
            commandHistory.push_back(line);
    }
}

const void showHistory() {
    std::cout << TerminalColors::BLUE << "\n=== Command History ===\n" << TerminalColors::RESET;
    int i = 1;
    for (const auto& cmd : commandHistory) {
        std::cout << TerminalColors::YELLOW << i++ << ": " << TerminalColors::GREEN << cmd << TerminalColors::RESET << "\n";
    }
    std::cout << TerminalColors::BLUE << "=======================\n" << TerminalColors::RESET;
}

const void printWelcomeBanner(const std::string& os_type) {
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
        std::cout << TerminalColors::BLUE << "║" 
                  << TerminalColors::RESET
                  << repeat(" ", pad_left)
                  << color << text << TerminalColors::RESET
                  << repeat(" ", pad_right)
                  << TerminalColors::BLUE << "║\n" << TerminalColors::RESET;
    };

    std::cout << TerminalColors::BLUE << "╔" << repeat("═", width) << "╗\n" << TerminalColors::RESET;
    printLine("AI Terminal Assistant: ShellAI v1.0", TerminalColors::CYAN);
    printLine("Ask anything about system commands!", TerminalColors::GREEN);
    printLine("OS detected: " + os_type, TerminalColors::YELLOW);
    printLine("Type a command request and hit Enter.", TerminalColors::CYAN);
    printLine("Press Enter on empty line to exit.", TerminalColors::CYAN);
    std::cout << TerminalColors::BLUE << "╚" << repeat("═", width) << "╝\n" << TerminalColors::RESET;
}

std::string prepareCommand(std::string cmd) {
    // --- Remove backticks and quotes ---
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '`'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '"'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\''), cmd.end());

    // --- Trim leading/trailing whitespace ---
    cmd.erase(0, cmd.find_first_not_of(" \t\n\r"));
    cmd.erase(cmd.find_last_not_of(" \t\n\r") + 1);

    return cmd;
}

bool CheckAndConfirmDangerousCommand(const std::string& command) {
    std::string lower_cmd = command;
    std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), ::tolower);

    const char* dangerousCommands[] = {
        "rm ",
        "shutdown",
        "reboot",
        "mkfs",
        "dd if=",
        "del ",
        "format ",
        "rd ",
        ":(){",
    };

    for (const auto& d : dangerousCommands) {
        if (lower_cmd.find(d) != std::string::npos) {
            // Dangerous command found → ask user
            std::string input;
            while (true) {
                std::cout << TerminalColors::RED << "WARNING: This command may be dangerous! Do you want to continue? (Y/n): " << TerminalColors::RESET;
                std::getline(std::cin, input);

                // Trim spaces
                input.erase(0, input.find_first_not_of(" \t\n\r"));
                input.erase(input.find_last_not_of(" \t\n\r") + 1);

                if (input.empty() || std::tolower(input[0]) == 'y') return true;  // execute
                if (std::tolower(input[0]) == 'n') return false; // skip

                std::cout << TerminalColors::RED << "Please enter Y or n.\n" << TerminalColors::RESET;
            }
        }
    }

    return true; // safe command → execute
}

void printCommandOutput(FILE* cmd_pipe, Logger& logger) {
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
            std::cout << TerminalColors::GREEN << std::setw(20) << std::left << line << TerminalColors::RESET;
            if (++count % 4 == 0) std::cout << "\n";
        } else {
            // multi-line output -> print normally
            std::cout << TerminalColors::GREEN << line << TerminalColors::RESET << "\n";
        }
        logger.output(line);
    }

    if (!multiLine && count % 4 != 0) std::cout << "\n"; // final newline for columns
}

int main(){
    Logger logger;
   
    // --- Detect OS ---
    #ifdef _WIN32
    const std::string os_type = "Windows";
    #else
    const std::string os_type = "Linux";
    #endif

    loadHistory();
    printWelcomeBanner(os_type);
   
    do {

        // --- Get user request ---s
        std::cout << TerminalColors::YELLOW << "Enter your request: " << TerminalColors::RESET;
        std::string user_input;        
        std::getline(std::cin, user_input);
        logger.user(user_input);

        if (isGibberishInput(user_input)) {
            std::cout << "Input skipped.\n";
            logger.warning("Gibberish input skipped.");
            continue;
        }
               
        
        // --- Check for special commands ---
        if (user_input == "history") {
            showHistory();
            logger.info("User viewed history.");
            continue; // go back to ask for next command
        }

        if (user_input == "exit") {
            std::cout << TerminalColors::GREEN << "Exiting AI Terminal Assistant. Goodbye!\n" << TerminalColors::RESET;
            logger.info("User exited program.");
            break; // exit main loop
        }

        if(user_input == "clear"){ os_type == "Linux" ? system("clear") : system("clr"); logger.info("Screen cleared."); continue; }
           
        if (!user_input.empty() && (user_input[0] == '!' || user_input[0] == '~')) {
            char action = user_input[0];
            std::string rest = user_input.substr(1);
            rest.erase(0, rest.find_first_not_of(" \t\n\r"));
        
            try {
                int index = std::stoi(rest);
                if (index <= 0 || index > static_cast<int>(commandHistory.size())) {
                    std::cout << TerminalColors::RED << "Invalid history number.\n" << TerminalColors::RESET;
                    logger.warning("Invalid history number entered: " + rest);
                    continue;
                }
        
                if (action == '!') {
                    std::string history_command = commandHistory[index - 1];
                    std::cout << TerminalColors::CYAN << "Re-running command from history: " << history_command << TerminalColors::RESET << "\n";
                    logger.info("Re-running history command: " + history_command);
        
                    if (!CheckAndConfirmDangerousCommand(history_command)) {
                        std::cout << TerminalColors::YELLOW << "Command skipped by user.\n" << TerminalColors::RESET;
                        logger.warning("Dangerous command skipped: " + history_command);
                        continue;
                    }
        
                    FILE* cmd_pipe = popen(history_command.c_str(), "r");
                    if (!cmd_pipe) { 
                        std::cerr << TerminalColors::RED << "Failed to execute command!" << TerminalColors::RESET << "\n";
                        continue; 
                        logger.error("Failed to execute history command: " + history_command);
                    }
                    printCommandOutput(cmd_pipe,logger);
                    pclose(cmd_pipe);
        
                } else if (action == '~') {
                    commandHistory.erase(commandHistory.begin() + index - 1);
                    std::ofstream file(HISTORY_FILE);
                    for (const auto& cmd : commandHistory) file << cmd << "\n";
                    logger.info("Deleted command from history: #" + std::to_string(index));
                    std::cout << TerminalColors::YELLOW << "Deleted command #" << index << " from history.\n" << TerminalColors::RESET;
                }
        
                continue; // skip AI processing for both
            } catch (...) {
                logger.warning("Non-numeric input after ! or ~, passing to AI: " + rest);
            }
        }        

        // --- Call Python AI script ---
        std::string python_command = "python AI_Bridge.py \"" + user_input + "\" " + os_type;
        FILE* pipe = popen(python_command.c_str(), "r");
        if (!pipe) {
            std::cerr << TerminalColors::RED << "Failed to run AI script!" << TerminalColors::RESET << "\n";
            logger.error("Failed to run AI script for input: " + user_input);
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
        std::cout << TerminalColors::CYAN << "\nAI Suggests: " << ai_command << TerminalColors::RESET << "\n";
        logger.ai(ai_command);
        
        // --- Dangerous commands trigger ---
        if (!CheckAndConfirmDangerousCommand(ai_command)) {
            std::cout << TerminalColors::YELLOW << "Command skipped by user.\n" << TerminalColors::RESET;
            logger.warning("Dangerous AI command skipped: " + ai_command);
            continue;
        }
        
        // --- Ask if user wants explanation ---
        std::cout << TerminalColors::YELLOW << "Press E for explanation, or Enter to continue: " << TerminalColors::RESET;
        std::string choice;
        std::getline(std::cin, choice);
        logger.user("User choice for explanation: " + choice);
        
        if (!choice.empty() && (choice[0] == 'E' || choice[0] == 'e')) {
            std::string explain_command = "python AI_Bridge.py explain \"" + ai_command + "\" " + os_type;
            FILE* explain_pipe = popen(explain_command.c_str(), "r");
            if (explain_pipe) {
                std::string explanation;
                    while (fgets(buffer, sizeof(buffer), explain_pipe) != nullptr) {
                        explanation += buffer;
                    }
                    pclose(explain_pipe);
                    std::cout << TerminalColors::GREEN << "\nExplanation: " << TerminalColors::BLUE << explanation << TerminalColors::RESET <<"\n";
                    logger.ai("Explanation: " + explanation);
            }else {
                std::cerr << TerminalColors::RED << "Failed to get explanation!\n" << TerminalColors::RESET;
                logger.error("Failed to get AI explanation for command: " + ai_command);
                }
        }

        // --- Execute command with colored output ---
        FILE* cmd_pipe = popen(ai_command.c_str(), "r");
        if (!cmd_pipe) {
            std::cerr << TerminalColors::RED << "Failed to execute command!" << TerminalColors::RESET << "\n";
            logger.error("Failed to execute AI command: " + ai_command);
            continue;
        }

        // --- Print command output ---
        printCommandOutput(cmd_pipe,logger);
        pclose(cmd_pipe);

        // --- Add to history after successful execution ---
        addCommand(ai_command);
    
    }while(1);

    return 0;
}
