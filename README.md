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

The format expected from the .txt files would be width and height to start with, then add a new line, then the program
will read width amount of bytes, then a newline again, and so forth until the height has been reached.

### Purpose
The purpose was for me to handle a simple picture for storing sprites in my snake game. I tried using PNG to start with,
but found out it was a little out of scope to use such a big system with compression and memory usage. That's why it makes
me go ballistic now that I found out that PNG with compression still beats my non-compressed file. Although, I have only
tested while using just the color 0x00 and 0xFF, so I assume when I test with more colors, I might have them beat. At 
least with such small files.

### Further development
As mentioned earlier, compression is an important part of pictures. I do think it is overkill to implement compression here,
but it would be really cool though. So in the future I might implement Huffman coding, as I don't want to use an algorithm
with any dataloss.

### Test environment
Currently, the test environment has to explicitly hardcode the .ste files they wish to represent in the window of the 
test environment. This will be found in the windowApplicationSteFile.

The test environment renders the picture through a library called SDL2. The window is hardcoded to open with the pixel
density of 1024*1024. But I have made a Pixel struct to make the pixels bigger, and this Pixel struct is required when
using the test environment. You can change the size of the pixel density by changing the Block_length define inside  
the windowApplicationSteFile.h file. You have to change the Block_length to the power of 2 to make it work.

### Prerequisites
- A linux operating system
- gcc installed
- SDL2 installed *(if you expect to use the test environment)*
- "rawImageFiles" folder for holding the txt files which is going to be converted
- "images" folder for holding the .ste files

### Running the program
- make
- ./main
  - -c
    - (Leave area empty after -c would make all files in rawImageFiles into .ste files and place them inside images folder)
    - rawImageFiles/myTextFile OR myTextFile (You can convert one .txt file at a time by mentioning that specific file)
  - -r (Read and see how your files look. This is a test page, and is not able to see files bigger than 1024*1024)

```bash
make
./main -c
```

```bash
./main -r
```
