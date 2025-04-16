```
 ░▒▓███████▓▒░▒▓████████▓▒░▒▓████████▓▒░▒▓████████▓▒░▒▓█▓▒░▒▓█▓▒░      ░▒▓████████▓▒░ 
░▒▓█▓▒░         ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░        
░▒▓█▓▒░         ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░        
 ░▒▓██████▓▒░   ░▒▓█▓▒░   ░▒▓██████▓▒░ ░▒▓██████▓▒░ ░▒▓█▓▒░▒▓█▓▒░      ░▒▓██████▓▒░   
       ░▒▓█▓▒░  ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░        
       ░▒▓█▓▒░  ░▒▓█▓▒░   ░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░        
░▒▓███████▓▒░   ░▒▓█▓▒░   ░▒▓████████▓▒░▒▓█▓▒░      ░▒▓█▓▒░▒▓████████▓▒░▒▓████████▓▒░
```

### Information
This application is able to create .ste files from .txt files. Stefile is a basic picture file, containing a 4 byte 
identifier (STE0), 2 bytes for height and width, and a byte pointer for the body. The body is expected to be equal to 
width * height. As each pixel in the body is only represented by a byte, steFile only recognizes 256 colors. 

### Test environment
Currently, the test environment has to explicitly hardcode the .ste files they wish to represent in the window of the 
test environment

### Prerequisites
- A linux operating system
- gcc installed
- "rawImageFiles" folder for holding the txt files which is going to be converted
- "images" folder for holding the .ste files

### Running the program
- make
- ./main
  - -c
    - (Leave area empty after -c would make all files in rawImageFolder into .ste files and place them inside images folder)
    - rawImageFiles/myTextFile OR myTextFile (You can convert one .txt file at a time by mentioning that specific file)
  - -r (Read and see how your files look. This is a test page, and is not able to see files bigger than 1024*1024)

```bash
make
./main -c
```

```bash
./main -r
```
