#include "read_file.h"
void read_file(std::string* data)
{
	// Create an input stream to the file
	// std::ifstream file("example\\test.nl");
	std::ifstream file("example\\hello.nl");
	//std::ifstream file("object_oriented_programming.rcl");
	if (file.is_open()) {
		// Read the file line by line and append to the content
		std::string line;
		while (std::getline(file, line)) {
			*data += line + "\n";
		}
		// Close the file
		file.close();
	}
	else {
		// Handle the error
		std::cout << "Unable to open the file.\n";
	}

}