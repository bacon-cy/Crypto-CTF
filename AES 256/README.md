## AES-256 cipher : encryption and decryption
This tool is written for AES-256 cipher.
Users should first execute it ,and then give the following command:

`{-e|-d} -k <key> -f <file>`

### input options
	-e
		If the file is going to be encrypted, choose -e.
	-d
		If the file is going to be decrypted, choose -d.

	-k <key>
		The 256 bits key for cipher is required. Please enter 64 hex (32 bytes) after "-k ".

	-f <file>
		The input file should be mentioned. Please enter the path of file and filename after "-f ".

## plaintext converter : hex to binary
If the input file for encryption/decryption is written in hex string, we can use this tool to convert it into binary file.

`<hex string> <filename>`

### inputs
	<hex string>
		Please input the hex string which is going to be converted into binary file. 
	<filename>
		Please input the output filename. For example, "plaintext.txt".