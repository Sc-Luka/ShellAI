ShellAI v1.0

-ShellAI is a cross-platform terminal tool that allows you to:

Run system commands safely

Get AI-generated terminal commands from natural language input

Receive explanations of commands

Maintain and manage command history

Detect gibberish or unsafe inputs

It integrates with OpenAI/OpenRouter to suggest terminal commands or generate code snippets.
---
Features

  ✅ Cross-platform: Windows, Linux

  ✅ Command history with ! rerun and ~ delete

  ✅ Gibberish input detection

  ✅ AI-suggested commands for any system task

  ✅ Explanation mode for understanding commands

  ✅ Logs all user inputs, AI suggestions, and terminal output
---
Installation

Clone the repository:

git clone https://github.com/Sc-Luka/ShellAI.git
cd ShellAI


Make sure you have C++17 compiler:

Linux g++

Windows: Visual Studio or g++ via MinGW

Install Python 3 and OpenAI SDK:

pip install openai

Set your OpenAI API key:

export OPENAI_API_KEY="your_api_key"  # Linux
setx OPENAI_API_KEY "your_api_key"    # Windows
---
Usage

Compile the C++ program:

g++ main.cpp -o shellai -std=c++17


Run ShellAI:

./shellai      # Linux
shellai.exe    # Windows

Commands:

exit → quit ShellAI

clear → clear terminal screen

history → view past commands

!<number> → rerun command from history

~<number> → delete command from history

AI Integration:

Type natural language queries like:

List all files in home directory
Write a C++ program to calculate factorial
Show disk usage of current folder


The AI will suggest a terminal command or code snippet.

Press E to get an explanation of any command.
---
Logging

All terminal input, AI suggestions, and command output are logged in ai_terminal.log.

Command history is saved in ai_terminal_history.txt
---
Security

Detects dangerous commands like rm -rf /, shutdown, dd if=.

Prompts user for confirmation before executing them.
