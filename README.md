# Simple FTP - A C++ based FTP Server to Transfer File 

## Features 
- File Uploading
- File Downloading
- File Transferring

## Technologies
- C++

## Run in Local Machine
1. Download or clone the project and unzip the project if necessery.
2. Go to the `server` directory and open terminal.
3. Run the command
   ```bash
   g++ ftp_server.cpp -o ftp_server
   ```
   ```bash
   ./ftp_server
   ```

4. Go to the `client` directory, open terminal and run the command.
   ```bash
   g++ ftp_client.cpp -o ftp_client
    ```
   ```bash
   ./ftp_client
   ```
5. Testing the server :
     - File Uploading  
           - Create a file in `client` directory [ig : test.txt]  
           - Run the command `./ftp_client` and `UPLOAD test.txt  `
           - Uploaded file location will be `server/uploads  `
      - File Downloading   
           - Run the command `./ftp_client` and `DOWNLOAD test.txt`
           - Downloaded file location will be `client/downloads`
   
   

