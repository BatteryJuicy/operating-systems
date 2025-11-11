# operating-systems
Assignments for HY-345 OS class
  - [Assignment 1 - Linux Shell](hy345/ask1)
  - [Assignment 2 - Thread Synchronization](hy345/ask2)

## Build

To build and run each assignment:
    
1. Make sure make is installed on your system by running the command
    ```bash
    make -version
    ```
2. In the terminal, move to the directory of the wanted assignment and find the `Makefile` file.

3. run the command
    ```bash
    make all
    ```
    This tells make to build the executable according the instructions of the `Makefile` file.

4. Run the command
    ```bash
    ./<executable name>
    ```
    For example in assignment 1:
    ```bash
    ./hy345sh.out
    ```
    or double click the executable on the folder (Although in some cases the program could immidiately terminate after it's finished so this is not recomended).

### Optional
To delete the files make created run
   ```bash
   make clean
   ```




