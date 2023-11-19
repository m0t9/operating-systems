#!/bin/bash

# Create an lofs.img file with the size of 100 MB using zero bytes
sudo dd if=/dev/zero of=lofs.img bs=1M count=100

# Setting up a loop device with the first free index on the created file lofs.img
sudo losetup -f lofs.img

# Creating a variable loop that contains the last created loop device name (greatest index of loop device)
loop=$(sudo losetup -a | awk '{print $1}' | sort -V | tail -n 1 | rev | cut -c 2- | rev)

# Creating a filesystem of ext4 on the loop device
sudo mkfs -t ext4 $loop

# Create a directory ./lofsdisk
mkdir ./lofsdisk

# Mounting a new filesystem to ./lofsdisk directory from loop device
sudo mount $loop ./lofsdisk

# Writing a file1 with first name
echo "Matvey" | sudo tee ./lofsdisk/file1

# Writing a file2 with second name
echo "Korinenko" | sudo tee ./lofsdisk/file2

# A function to get all the dependencies of file with provided path
get_libs() {
    src=$1                                              # Declare a first argument -- path
    libs=$(ldd $src | grep "=> /" | awk '{print $3}')   # Parse the output of the ldd command for the provided path
    echo $libs                                          # Return the text representation of libs
}

bash_src=/bin/bash                                      # Create a variable with path to bash 
cat_src=/bin/cat                                        # Create a variable with path to cat
echo_src=/bin/echo                                      # Create a variable with path to echo
ls_src=/bin/ls                                          # Create a variable with path to ls

bash_dep=$(get_libs /bin/bash)                          # Create a variable for bash dependencies, load their paths
cat_dep=$(get_libs /bin/cat)                            # Create a variable for cat dependencies, load their paths
echo_dep=$(get_libs /bin/echo)                          # Create a variable for echo dependencies, load their paths
ls_dep=$(get_libs /bin/ls)                              # Create a variable for ls dependencies, load their paths

# A function to add executable files to new filesystem
add_exec() {
    executable=$1                                       # Create a variable for first argument of the function
    new_path="./lofsdisk${executable%/*}"               # Create a variable for the path to the executables dir in new filesystem
    sudo mkdir -p $new_path                             # Create a new path
    sudo cp $executable $new_path                       # Copy executable to new filesystem    
}

# A function to add shared libraries of the executables
add_libs() {
    libs=$1                                             # Variable for the first argument
    IFS=' '                                             # Set a separator as ' '
    for lib in $libs; do                                # For-each loop
        new_path="./lofsdisk${lib%/*}"                  # Create a variable for the path to the executables dir in new filesystem
        sudo mkdir -p $new_path                         # Create a directories on the new path
        sudo cp $lib $new_path                          # Copy library of the binary
    done
}

# Copying all the sources of bash, echo, cat, ls and create a directory for them (if not exists)
add_exec $bash_src && add_exec $echo_src && add_exec $cat_src && add_exec $ls_src

# Copying all the shared libraries of bash, echo, cat, ls and create a directory for them (if not exists)
add_libs $bash_dep && add_libs $echo_dep && add_libs $cat_dep && add_libs $ls_dep

# Copy linker (I am using VM on Mac M1, so, probably, it may have another place)
sudo cp /lib/ld-linux-aarch64.so.1 ./lofsdisk/lib/

# Compiling a ex1 file that prints the content of root directory
gcc -Wall -Wextra -Werror ex1.c -o ex1

# Copy ex1 to ./lofsdisk
sudo cp ./ex1 ./lofsdisk

# Run ex1 with chroot
# Write "Run with chroot" to ex1.txt 
echo "Run with chroot" >> ex1.txt
# Append the output running with chroot
sudo chroot ./lofsdisk ./ex1 >> ex1.txt

# Run ex1 without chroot
# Write "Run without chroot" to ex1.txt with correct interpretation of "\n" symbols
echo -e "\nRun without chroot" >> ex1.txt
# Append the output running with chroot
./ex1 >> ex1.txt

