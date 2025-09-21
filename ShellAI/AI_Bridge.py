import sys
import os
from openai import OpenAI
import re
import tempfile

# --- API Key handling ---
api_key = os.getenv("OPENAI_API_KEY")

if not api_key:
    # fallback: hardcoded key 
    api_key = "YOUR_API_KEY"

if not api_key:
    sys.exit("API key is required! Set OPENAI_API_KEY in your environment.")

# --- OpenRouter client setup (DeepSeek / other models through OpenRouter) ---
client = OpenAI(
    base_url="https://openrouter.ai/api/v1",  # OpenRouter endpoint
    api_key=api_key,
)

# --- If you want to use GPT directly (OpenAI endpoint), uncomment this instead ---
# client = OpenAI(
#     api_key=api_key,  # OpenAI official endpoint (default base_url)
# )

# --- Mode handling ---
mode = "normal"

if len(sys.argv) < 2:
    sys.exit("Usage: python AI_Bridge.py '<user_request>' [os_type]")

# If first arg is "explain", we switch mode
if sys.argv[1].lower() == "explain":
    mode = "explain"
    if len(sys.argv) < 3:
        sys.exit("Usage: python AI_Bridge.py explain '<command>' [os_type]")
    user_request = sys.argv[2]
    os_type = sys.argv[3] if len(sys.argv) > 3 else "Linux"
else:
    user_request = sys.argv[1]
    os_type = sys.argv[2] if len(sys.argv) > 2 else "Linux"


def ask_openai(prompt, mode="normal"):
    try:
        if mode == "explain":
            system_prompt = (
                f"You are a helpful assistant. Explain clearly what this terminal "
                f"command does on {os_type}. Keep it short and easy to understand. "
                f"Do not return the command itself, only the explanation."
            )
            user_message = prompt
        else:
            # Detect if this is a code request
            is_code_request = any(keyword in prompt.lower() for keyword in [
                'code', 'program', 'write a', 'create a', 
                'c++', 'c#', 'python', 'java', 'javascript'
            ])

            if is_code_request:
                system_prompt = (
                    "You are a helpful assistant. Generate code as requested. "
                    "Return only the code without any explanation."
                )
            else:
                system_prompt = (
                    f"You are a terminal assistant. Generate only a single terminal "
                    f"command for {os_type}. Do not combine multiple commands. "
                    f"Return only the command without any explanation."
                )

            user_message = prompt

        # --- API call ---
        response = client.chat.completions.create(
            # For DeepSeek on OpenRouter
            model="openai/gpt-4o-mini",

            # If you want to switch to GPT official API, just use (example):
            # model="gpt-4o-mini",  # or "gpt-4.1", "gpt-3.5-turbo", etc.

            messages=[
                {"role": "system", "content": system_prompt},
                {"role": "user", "content": user_message}
            ]
        )

        result = response.choices[0].message.content.strip()

        if mode == "explain":
            return result

        # --- Normal mode cleanup ---
        # Handle code block cleanup if it's a code request
        if "```" in result:
            result = re.sub(r'```(?:\w+)?\s*', '', result)
            result = re.sub(r'```\s*$', '', result)

        # If it's code, save it to a temp file
        if is_code_request:
            fd, temp_path = tempfile.mkstemp(suffix='.txt', text=True)
            with os.fdopen(fd, 'w') as tmp:
                tmp.write(result)

            if os_type.lower() == "windows":
                return f'type "{temp_path}"'
            else:
                return f'cat "{temp_path}"'

        # For commands, remove markdown remnants
        result = re.sub(r'```bash', '', result, flags=re.IGNORECASE)
        result = re.sub(r'```', '', result)

        return result.strip()

    except Exception as e:
        sys.exit(f"Error calling API: {e}")


command_or_explanation = ask_openai(user_request, mode)
print(command_or_explanation)
