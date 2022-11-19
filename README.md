# networking-db-cpp

# TODO
- Increment Client counter when adding into session
- Safety check: Check if the session exists when adding a client
- Safety check: Check if the client already exists in the session.
- Increment Topic counter when adding into Client
- Safety check: Check if the client exists when adding a topic
- Safety check: Check if the client already contains the topic.
- Find a better way to insert new data into tables.
- Security connections


# Optional settings - .vscode environment

This environment is prepared to have two build types (Debug and Release).

``` bash
# To create a copy of vscode contents
./contrib/import_vscode.bash --copy

# To create a link of vscode contents
./contrib/import_vscode.bash --link

# Note: This script is designed to be run only, and only if, the contrib directory is located in the project root.
```

# Optional settings - .devcontainer environment

Insert info here

``` bash
# To create a copy of vscode contents
./contrib/import_devcontainer.bash --copy

# To create a link of vscode contents
./contrib/import_devcontainer.bash --link

# Note: This script is designed to be run only, and only if, the contrib directory is located in the project root.
```

# Optional settings - .clang-tidy environment

Insert info here

``` bash
# To create a copy of vscode contents
./contrib/import_clang_tidy.bash --copy

# To create a link of vscode contents
./contrib/import_clang_tidy.bash --link

# Note: This script is designed to be run only, and only if, the contrib directory is located in the project root.
```
