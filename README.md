# project3
■ LSB Image Steganography in C
This project implements image steganography using the Least Significant Bit (LSB) technique.
It allows hiding secret text or files inside BMP images and retrieving them later without
noticeable changes to the image.

■ Features
• Encode secret file into a cover BMP image
• Decode hidden file from stego image
• Support for BMP image format
• Simple CLI-based interface
• Error handling for incorrect inputs

■ Tech Stack
Language: C (GCC)
Concepts: Bitwise operations, File I/O, BMP image format
Tools: GCC, VS Code, Git

■ Usage
Encoding
./a.out -e <source_image.bmp> <secret.txt> <stego_image.bmp>
Decoding
./a.out -d <stego_image.bmp> <output.txt>
