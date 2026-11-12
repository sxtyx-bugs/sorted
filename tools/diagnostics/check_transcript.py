import json

with open(r'C:\Users\satya\.gemini\antigravity\brain\6146e3ed-a344-4a5c-aa29-a0c8ad8ef9f6\.system_generated\logs\transcript_full.jsonl', 'r', encoding='utf-8') as f:
    for i, line in enumerate(f):
        if 230 <= i <= 420:
            obj = json.loads(line)
            for c in obj.get('tool_calls', []):
                cmd = c.get('args', {}).get('CommandLine', '')
                if cmd and ('python' in cmd or 'bat' in cmd or 'sorted' in cmd.lower() or 'move' in cmd.lower() or 'shutil' in cmd):
                    print(f"Line {i}: {cmd[:120]}")
