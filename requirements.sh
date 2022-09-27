# Saves the current directory
project_directory=$PWD

# Creates a temporary folder for some installations (will be deleted in the end)
mkdir -p $project_directory/temp
cd $project_directory/temp

# Functions
install_fmt_library() {
    sudo add-apt-repository universe
    sudo apt update
    sudo apt install -y libfmt-dev
}

install_crow_library() {
    sudo apt-get update

    # Pre-requirements
    sudo apt-get install -y libasio-dev

    # Library installation
    git clone https://github.com/CrowCpp/Crow.git
    cd Crow

    mkdir -p build
    cd build

    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
    sudo make install

    cd $project_directory/temp
}

install_oatpp_library() {
    # Pre-requirements
    # sudo apt install -y git cmake build-essential

    # Library installation
    git clone https://github.com/oatpp/oatpp.git
    cd oatpp/

    mkdir -p build
    cd build

    cmake ..
    sudo make install

    cd $project_directory/temp
}

install_sqlite3_library() {
    sudo apt update
    sudo apt install -y sqlite3 libsqlite3-dev
}

install_google_unit_testing_cpp() {
    sudo apt update
    sudo apt install -y libgtest-dev
}

# Actual installation orders
install_fmt_library
install_crow_library
install_sqlite3_library
install_google_unit_testing_cpp

# Returns to the project directory and removes the temporary folder
cd $project_directory
rm -rf $project_directory/temp
