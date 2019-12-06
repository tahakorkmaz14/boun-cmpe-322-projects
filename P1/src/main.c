#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
// C++ implementation to read 
// file word by word 
#include <bits/stdc++.h> 
using namespace std; 

// driver code 
int main() 
{ 
	// filestream variable file 
	fstream file; 
	string word, t, q, filename; 

	// filename of the file 
	filename = "file.txt"; 

	// opening file 
	file.open(filename.c_str()); 

	// extracting words form the file 
	while (file >> word) 
	{ 
		// displaying content 
		cout << word << endl; 
	} 

	return 0; 
} 

