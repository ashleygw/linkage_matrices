#include <iostream>
#include <fstream>
#include <string>
#include "CSVhandler.h"

int main(int argc, char* argv[]) {
	CSVhandler man;
	if (argc > 3) {
		std::cout << "File: " << argv[1] << std::endl;
		std::cout << "Sectors: " << atoi(argv[2]) << std::endl;
		std::cout << "Top contributors: " << atoi(argv[3]) << std::endl;
		std::fstream file(argv[1], std::ios::in);
		if (!file.is_open()) {
			std::cout << "File not found!\n";
			return 1;
		}
		man.set_sectors(atoi(argv[2]));
		man.set_num_top_contributors(atoi(argv[3]));
		man.writeFLT(file);
		man.writeKC(file);
	}
	else if (argc > 2) {
		std::cout << "File: " << argv[1] << std::endl;
		std::cout << "Sectors per region: " << atoi(argv[2]) << std::endl;
		std::cout << "Top contributors: 3" << std::endl;
		std::fstream file(argv[1], std::ios::in);
		if (!file.is_open()) {
			std::cout << "File not found!\n";
			return 1;
		}
		man.set_sectors(atoi(argv[2]));
		man.set_num_top_contributors(3);
		man.writeFLT(file);
		man.writeKC(file);
	}
	else if (argc > 1) {
		std::cout << "Opening default input file: \"20Sector_FL_DP.csv\"" << std::endl;
		std::fstream file("20Sector_FL_DP.csv", std::ios::in);
		if (!file.is_open()) {
			std::cout << "File not found!\n";
			return 1;
		}
		man.set_sectors(atoi(argv[1]));
		man.set_num_top_contributors(3);
		std::cout << "Sectors per region: " << atoi(argv[1]) << std::endl;
		std::cout << "Top contributors: 3" << std::endl;
		man.writeFLT(file);
		man.writeKC(file);
	}
	/*else{
		std::cout << "Please enter extra parameters." << std::endl;
		std::cout << "It should be formatted \"input filename\" \"number of sectors\"\n"
			<< "(optional)\"number of top contributors\""<< std::endl;
		std::cout << "\n\nNo new files generated." << std::endl;
		return 1;
	}*/
	else {
		std::string in;
		std::cout << "Enter input filename: " << std::endl;
		std::cin >> in;
		std::fstream file(in, std::ios::in);
		if (!file.is_open()) {
			std::cout << "File not found!\nEnter to exit.";
			return 1;
		}

		std::cout << "Enter number of sectors per region:" << std::endl;
		std::cin >> in;
		man.set_sectors(atoi(in.c_str()));
		std::cout << "Number of top contributors:" << std::endl;
		std::cin >> in;
		man.set_num_top_contributors(atoi(in.c_str()));
		std::cout << "Files building..." << std::endl;
		man.writeFLT(file);
		man.writeKC(file);
		std::cout << "Files made." << std::endl;
		return 0;
	}
	std::cout << "Files made successfully." << std::endl;
}
