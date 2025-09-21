# ShellAI v1.0

  - ShellAI is a cross-platform terminal tool that allows you to:

  - Run system commands safely

  - Get AI-generated terminal commands from natural language input

  - Receive explanations of commands

  - Maintain and manage command history

  - Detect gibberish or unsafe inputs

  - It integrates with OpenAI/OpenRouter to suggest terminal commands or generate code snippets.
---
## 📸 Screenshot


<img width="1448" height="747" alt="Screenshot (236)" src="https://github.com/user-attachments/assets/e696078a-efef-45db-9040-d5f8cb131e2c" />


---
## 🧠 Features

  - Cross-platform: Windows, Linux

  - Command history with ! rerun and ~ delete

  - Gibberish input detection

  - AI-suggested commands for any system task

  - Explanation mode for understanding commands

  - Logs all user inputs, AI suggestions, and terminal output

---
## 📟 Commands:

  - exit → quit ShellAI

  - clear → clear terminal screen

  - history → view past commands

  - !<number> → rerun command from history

  - ~<number> → delete command from history

## 🧠 AI Integration:

### Type natural language queries like:

### The AI will suggest a terminal command or code snippet.

  - List all files in home directory

  - Write a C++ program to calculate factorial

  - Show disk usage of current folder 

  - Press E to get an explanation of any command.
---
## 🎯 Logging

  - All terminal input, AI suggestions, and command output are logged in ai_terminal.log.

  - Command history is saved in ai_terminal_history.txt

---
## 🔒 Security

  - Detects dangerous commands like rm -rf /, shutdown, dd if=.

  - Prompts user for confirmation before executing them.
---
## 🛠 Running the Project 

1. Clone the repository and install dependencies:

   ```bash

       git clone https://github.com/Sc-Luka/ShellAI
       cd ShellAI
       pip install openai

2. Create and activate a Python virtual environment (optional but recommended)

      - Linux

       python3 -m venv venv
       source venv/bin/activate

      - Windows

       python -m venv venv
       .\venv\Scripts\Activate.ps1

3. Install Python dependencies

       pip install openai

4. make sure you have C++17 compiler:

      - Linux: g++

      - Windows: Visual Studio or g++ via MinGW

5. Set your OpenAI API key:
  
       export OPENAI_API_KEY="your_api_key"  # Linux
       setx OPENAI_API_KEY "your_api_key"    # Windows

6. Compile the C++ program

       g++ main.cpp -o shellai -std=c++17

7. Run ShellAI:

       ./shellai      # Linux
       shellai.exe    # Windows
