import json

# Read the original JSON file
with open("cp_config.json", "r", encoding="utf-8") as f:
    data = json.load(f)


def sort_key(item):
    param_name, details = item
    group = details.get("group")
    # Non-null group names sort first (alphabetically); null/None groups sort last.
    # Within the same group, sort alphabetically by parameter name.
    group_key = (0, group.lower()) if group is not None else (1, "")
    return (group_key, param_name.lower())


# Sort items according to group name and parameter name
sorted_items = sorted(data.items(), key=sort_key)

# Convert back to dictionary (Python 3.7+ preserves key insertion order)
sorted_data = dict(sorted_items)

# Save the output to a new JSON file
with open("cp_config.json", "w", encoding="utf-8") as f:
    json.dump(sorted_data, f, indent=2)

print("Successfully sorted cp_config.json by group and parameter name!")