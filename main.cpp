#include <iostream>
#include <fstream>
#include <string>
#include "CSVhandler.h"

int main(int argc, char* argv[]) {
	CSVhandler man;
	if (argc > 5) {
		std::cout << "File: " << argv[1] << std::endl;
		std::cout << "Sectors: " << atoi(argv[2]) << std::endl;
		std::cout << "Top contributors: " << atoi(argv[3]) << std::endl;
		std::cout << "Lower bound: " << atoi(argv[4]) << std::endl;
		std::cout << "Upper bound: " << atoi(argv[5]) << std::endl;

		std::fstream file(argv[1], std::ios::in);
		if (!file.is_open()) {
			std::cout << "File not found!\n";
			return 1;
		}

		man.set_input_filename(argv[1]);
		man.set_sectors(atoi(argv[2]));
		man.set_num_top_contributors(atoi(argv[3]));
		man.set_reported_contributors(atoi(argv[4]), atoi(argv[5]));
		std::cout << "Files building..." << std::endl;
		man.writeFLT(file);
		man.writeKC(file, 1);
		std::cout << "Files made." << std::endl;
		


	}
	//Removed all other cases, needlessly confusing.
	else {
		//Variables are cheap, using only in results in strange error
		//where inputfile is set as the last thing the user entered.
		//I suspect it is part of cin's functionality and the way I 
		//set the input filename
		std::string in;
		std::string file_in;
		std::string add_file_in;
		std::fstream add_file;
		std::cout << "Enter input filename: " << std::endl;
		std::cin >> file_in;

		int temp_lower = 3;
		int temp_upper = 4;

		//Undefined behavior if string is empty
		char *temp = &file_in[0u];
		man.set_input_filename(temp);

		std::fstream file(file_in, std::ios::in);
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
		std::cout << "Lower bound for reported key sectors:" << std::endl;
		std::cin >> in;
		temp_lower = stoi(in);
		std::cout << "Upper bound for reported key sectors:" << std::endl;
		std::cin >> in;
		temp_upper = stoi(in);
		man.set_reported_contributors(temp_lower, temp_upper);
		
		while (1)
		{
			std::cout << "Generate common key contributors table? (y/n) " << std::endl;
			std::cin >> in;
			if (in == "y")
			{
				while (1)
				{
					std::cout << "Please enter the other linkage table filename: " << std::endl;
					std::cin >> add_file_in;
					std::fstream add_file(add_file_in, std::ios::in);
					if (!add_file.is_open()) {
						std::cout << "File not found!" << std::endl;
					}
					else
					{
						//same method for converting to char array, needs new variable
						char *tempo = &add_file_in[0u];
						man.set_add_input_filename(tempo);
						while (1)
						{
							std::cout << "Is this a forward or backwards table? (f/b) " << std::endl;
							std::cin >> in;
							if (in == "f")
							{
								man.set_flag(true);
							}
							else if (in == "b")
							{
								man.set_flag(false);
							}
							else
							{
								std::cout << "Input not recognized." << std::endl;
								continue;
							}
							break;
						}
						break;
					}
					//probably will never hit this
					break;
				}
				break;
			}
			else if (in == "n")
			{
				//Check is whether or not the additional file name variable
				//has a non-default name, nothing needs to change here.
				break;
			}
			else
				std::cout << "Character not recognized, pleace write 'y' or 'n'." << std::endl;
		}
		std::cout << "Files building..." << std::endl;
		
			
		man.writeFLT(file);
		man.writeKC(file, !man.forward_flag);
		man.writeKC(add_file, man.forward_flag);
		if (man.additional_file != "")
		{
			man.set_sector_names_no_regions();
			man.writeCSV_KT();
		}
			
		std::cout << "Files made." << std::endl;
		return 0;
	}
	std::cout << "Files made successfully." << std::endl;
}
