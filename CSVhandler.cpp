#include "CSVhandler.h"

void CSVhandler::set_sectors(const int & sectors)
{
	_num_sectors = sectors;
}

//Generates a double vector of sums by column
//sets sumpairs, the pair of name and value
void CSVhandler::sum()
{
	std::vector<double> sum;
	int counter = 0;
	double total = 0;
	for (int i = 0; i < db.size()-1; i++)
	{
		total = 0;
		for (int j = 0; j < db.size()-1; j++)
		{
			total += std::stod(db[j+1][i+1]);
		}
		sum.push_back(total);
		_sumpair.push_back(std::pair<std::string,double> (db[i+1][0], total));
	}
	_sumvec = sum;
}

//Used to build sectors and calculate mean and std dev
//Each index in this vector consists of all sector sums in a region in double form
void CSVhandler::build_regions()
{
	_regions.clear();
	std::vector<double> temp;
	for (int i = 0; i <_sumpair.size()/_num_sectors; i++)
	{
		temp.clear();
		for (int j = 0; j < _num_sectors; j++)
		{
			temp.push_back(_sumpair[j+i*_num_sectors].second);
		}
		_regions.push_back(temp);
	}
}

//Calculates all the information needed to print the FLT.
void CSVhandler::build_sectors()
{
	//Puts names in _sectors
	for (int i = 0; i < _num_sectors; i++)
	{
		std::pair<std::string, std::vector<double> > temp;
		temp.first = db[i + 1][0];
		_sectors.push_back(temp);
	}
	_num_regions = _sumpair.size() / _num_sectors;
	std::vector<double> sector; // has values left to right for FLT
	double mean;
	double counter;
	for (int i = 0; i < _num_sectors; i++)
	{
		sector.clear();
		counter = 0;
		for (int j = 0; j < _num_regions; j++)
		{
			counter += _regions[j][i];
			sector.push_back(_regions[j][i]);
		}
		mean = counter / _num_regions;
		sector.push_back(mean);
		counter = 0;
		for (double k : sector)
			counter += (k - mean)*(k - mean);
		sector.push_back(sqrt(counter / _num_regions));
		_sectors[i].second = sector;
		//Sort by average
	}
	sorter sort;
	sort.num_regions = _num_regions;
	sort._num_top_contributors = _num_top_contributors;
	std::sort(_sectors.begin(), _sectors.end(), sort);
}

//Writes FLT to file.
void CSVhandler::writeCSV_FLT()
{
	int i = 0;
	int j = 0;
	std::ofstream file;
	std::string temp = "Forward_Link_Table(";
	std::string temp2 = current_file;
	temp += temp2 + ").csv";
	file.open(temp.c_str());
	file << "Table 1: Total Forward Link Table," << "Filename: " << input_file << "\n,";
	for (i = 1; i < _num_regions+1; i++)
	{
		file << db[0][i*_num_sectors];
		file << ",";
	}
	file << "Average," << "Std Dev";
	file << "\n";
	for (i = 0; i < _sectors.size(); i++)
	{
		//Writes sector names to file
		if (_sectors[i].first[0] == '\"')
		{
			for (j = 3; j < _sectors[i].first.length() - 1; ++j)
				file << _sectors[i].first[j];
		}
		else
		{
			for (j = 2; j < _sectors[i].first.length(); ++j)
				file << _sectors[i].first[j];
		}
		
		file << ",";
		for (j = 0; j < _sectors[i].second.size(); j++)
		{
			file << _sectors[i].second[j] << ",";
		}
		file << "\n";
	}
}

void CSVhandler::writeCSV_KC(bool is_forward)
{
	std::ofstream file;
	std::string temp = "Key_Contributors(";
	std::string temp2 = current_file;
	temp += temp2 + ").csv";
	std::unordered_map<std::string, int> key_sector_counter;
	std::unordered_map<std::string, int> general_sector_counter;
	int i, j, k;
	std::vector<std::vector<double> > summer(_num_regions*_num_sectors);

	file.open(temp.c_str());
	file << "Input filename: " << current_file << "\n";
	double counter = 0;
	for (i = 0; i < _num_sectors; i++)
	{
		key_sector_counter.clear();
		general_sector_counter.clear();
		for (j = 0; j < _num_regions; j++)
		{
			//Regions in columns. Writes each selected sector.
			file << "," << _sector_names[i + j*_num_sectors] << ",,";
		}
		file << "\n";
		//Finds top "size of pairlist" contributors.
		//Can change reported top contributors by modifying number of pairs calculated
		//in one_sector_top_contributors.
		for (j = 0; j < _all_top_contributors[i].size(); j++)
		{
			for (k = 0; k < _num_regions; k++)
			{
				//Writes top n contributors
				file << _all_top_contributors[i + k*_num_sectors][j].first << "," << _all_top_contributors[i + k*_num_sectors][j].second << ",,";
				std::string name = _all_top_contributors[i + k*_num_sectors][j].first;
				//Adds region specific contributor to map for counting purposes.
				if (key_sector_counter.find(name) != key_sector_counter.end())
				{
					key_sector_counter[name]++;
				}
				else
				{
					key_sector_counter[name] = 1;
				}
				//Having a - at the start makes Excel treat this is a formula. Breaks program.
				if (name[0] == '\"')
				{
					if (general_sector_counter.find(name.substr(3, name.length() - 4)) != general_sector_counter.end())
					{
						general_sector_counter[name.substr(3, name.length() - 4)]++;
					}
					else
					{
						general_sector_counter[name.substr(3, name.length() - 4)] = 1;
					}
				}
				else
				{
					if (general_sector_counter.find(name.substr(1)) != general_sector_counter.end())
					{
						general_sector_counter[name.substr(1)]++;
					}
					else
					{
						general_sector_counter[name.substr(1)] = 1;
					}
				}
				
				//Calculate averages of top contributors
				summer[i + k*_num_sectors].push_back(_all_top_contributors[i + k*_num_sectors][j].second);
			}
			file << "\n";
		}
		counter = 0;
		for (j = 0; j < _num_regions; j++)
		{
			counter = 0;
			for (k = 0; k < summer[i + j*_num_sectors].size(); k++)
			{
				counter += summer[i + j*_num_sectors][k];
			}
			file << "sum," << counter << ",,";
			summer[i + j*_num_sectors][0] = counter;
		}
		file << "\n";
		for (j = 0; j < _num_regions; j++)
		{
			file << "TOTAL," << _sumvec[i + j*_num_sectors] << ",,";
		}
		file << "\n";
		for (j = 0; j < _num_regions; j++)
		{
			file << "ratio," << summer[i + j*_num_sectors][0]/_sumvec[i + j*_num_sectors]*100 << ",,";
		}
		file << "\n";

		//Section 2 Key Sectors
		//Data is small enough to allow multiple passes
		file << "Key Sectors\n";
		for (j = _num_upperbound_reported_contributors; j >= _num_lowerbound_reported_contributors; --j)
		{
			counter = 0; // Used as boolean basically
			if (j == _num_upperbound_reported_contributors)
			{
				file << j << " or more,";
				for (auto it = key_sector_counter.begin(); it != key_sector_counter.end(); ++it)
				{
					if (it->second >= j)
					{
						file << it->first << ","; //<< " (" << it->second << "),";
						counter++;
					}
				}
				if (counter == 0)
					file << "None,";
				file << "\n";
			}
			else
			{
				file << j << ",";
				for (auto it = key_sector_counter.begin(); it != key_sector_counter.end(); ++it)
				{
					if (it->second == j)
					{
						file << it->first; //<< " (" << it->second << "),";
						counter++;
					}
				}
				if (counter == 0)
					file << "None,";
				file << "\n";
			}
			
		}
		
		counter = 0;
		file << "General Sector,";
		for (auto it = general_sector_counter.begin(); it != general_sector_counter.end(); ++it)
		{
			if (it->second >= _num_regions)
			{
				file << it->first << "(" << it->second << "),";

				//form is current sector - found notable sector - forward or backward
				//This is stored so that if the user wants a KCT these values don't have to be
				//Calculated again.
				if(is_forward)
					general_collection.push_back(Triad(_sector_names[i].substr(1), it->first, "F"));
				else
					general_collection.push_back(Triad(_sector_names[i].substr(1), it->first, "B"));
			}
		}
		file << "\n\n";
	}
}

void CSVhandler::clear_data()
{
	//Does not clear the database- Call cleardb
	_sumvec.clear();
	_sumpair.clear();
	_regions.clear();
	_sectors.clear();
	_sector_names.clear();
	_all_top_contributors.clear();
	_sector_names_no_region.clear();
}

void CSVhandler::clear_db()
{
	//Clearing the database can't be done every time the rest of the data is cleared
	//This function is standalone due to how the same db is used between building FLT and KC
	db.clear();
}

//Does everything needed for FLT
void CSVhandler::writeFLT(std::istream & input)
{
	clear_data();
	readCSV(input);
	set_sector_names();
	sum();
	build_regions();
	build_sectors();
	writeCSV_FLT();
}

//Does everything needed for KC
void CSVhandler::writeKC(std::istream & input, bool forward_table)
{
	clear_data();
	readCSV(input);
	set_sector_names();
	sum();
	build_regions();
	build_sectors();
	set_all_top_contributors();
	writeCSV_KC(forward_table);
}//////////////AT THE END OF FIOLE!??

void CSVhandler::writeCSV_KT()
{
	std::ofstream file;
	std::string temp = "FL_BL_Table(";
	std::string temp2 = input_file;
	std::string temp3 = additional_file;
	temp += temp2 + " " + temp3 + ").csv";
	file.open(temp.c_str());
	file << "Input filenames:, " << input_file << "," << additional_file << "\n,";
	int i;
	std::unordered_map<std::string, int> sec_counter;
	//Write sectors to top
	for (i = 0; i < _num_sectors; ++i)
	{
		file << _sector_names_no_region[i] << ",";

		//Maps sector names to number at the same time
		sec_counter[_sector_names_no_region[i]] = i;
	}

	file << "\n";

	//First write sector name, then if that sector has a general contributor, 
	//write that in the appropriate place.
	for (i = 0; i < _num_sectors; ++i)
	{
		//write sector name
		file << _sector_names_no_region[i] << ",";

		//check general collection
		std::vector<std::string> write_buffer(_num_sectors);
		for (int j = 0; j < general_collection.size(); ++j)
		{
			int index = 0;
			//General collection has current sector name
			if (std::get<0>(general_collection[j]) == _sector_names_no_region[i])
			{
				//Find which sector this corresponds too
				std::string contributor = std::get<1>(general_collection[j]);

				auto it = std::find(_sector_names_no_region.begin(), _sector_names_no_region.end(), contributor);
				if (it == _sector_names_no_region.end())
				{
					//This should not happen oh no
					std::cout << "Critical naming error. Try renaming sectors in form H4_Finance for example." << std::endl;
				}
				else
				{
					index = std::distance(_sector_names_no_region.begin(), it);
				}
				std::string link = std::get<2>(general_collection[j]);

				if ((write_buffer[index] == "F" && link == "B") || (write_buffer[index] == "B" && link == "F"))
					write_buffer[index] = "*";
				else
					write_buffer[index] = link;
			}
		}

		//Write to file using the buffer
		for (int j = 0; j < write_buffer.size(); ++j)
		{
			if (write_buffer[j] == "F" || write_buffer[j] == "B" || write_buffer[j] == "*")
				file << write_buffer[j] << ",";
			else
				file << ",";
		}
		file << "\n";
	}
}

//https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
//Pretty basic but this is where this is from.
void CSVhandler::readCSV(std::istream &input) {
	std::string csvLine;
	//db.clear();
	// read every line from the stream
	while (std::getline(input, csvLine)) {
		std::istringstream csvStream(csvLine);
		std::vector<std::string> csvRow;
		std::string csvCol;
		// read every element from the line that is seperated by commas
		// and put it into the vector or strings
		while (std::getline(csvStream, csvCol, ','))
			csvRow.push_back(csvCol);
		db.push_back(csvRow);
	}
	/*for (int j = 0; j < db[1].size(); ++j)
		std::cout << db[1][j];*/
}

//Builds vector of sector names
void CSVhandler::set_sector_names()
{
	std::vector<std::string> temp;
	for (int i = 1; i < db.size(); i++)
	{
		//Deal with quotes messing up the csv
		//if (db[i][0][0] == '\"')
			//temp.push_back(db[i][0].substr(1, db[i][0].length() - 2));
		//else
			temp.push_back(db[i][0]);
	}
	_sector_names = temp;
}

std::vector<std::pair<std::string, double> > CSVhandler::one_sector_top_contributors(int index)
{
	std::vector<std::pair<std::string, double> > to_sort;
	for (int i = 1; i < _num_sectors*_num_regions+1; i++)
	{
		to_sort.push_back(buildpair(i, index));
	}
	sorter sort;
	sort._num_top_contributors = _num_top_contributors;
	sort.num_regions = _num_regions;
	std::sort(to_sort.begin(), to_sort.end(), sort);
	std::vector<std::pair<std::string, double> > top_n;
	for (int i = 0; i < _num_top_contributors; i++)
	{
		top_n.push_back(to_sort[i]);
	}
	return top_n;
}

void CSVhandler::set_all_top_contributors()
{
	std::vector<std::vector<std::pair<std::string, double> > > temp;
	for (int i = 1; i < _num_sectors*_num_regions+1; i++)
	{
		temp.push_back(one_sector_top_contributors(i));
	}
	_all_top_contributors = temp;
}

void CSVhandler::set_num_top_contributors(int num)
{
	_num_top_contributors = num;
}

void CSVhandler::set_reported_contributors(int lower, int upper)
{
	_num_upperbound_reported_contributors = upper;
	_num_lowerbound_reported_contributors = lower;
}

void CSVhandler::set_input_filename(char * in)
{
	input_file = in;
}

void CSVhandler::set_add_input_filename(char * name)
{
	additional_file = name;
}

void CSVhandler::set_flag(bool boo)
{
	forward_flag = boo;
}

void CSVhandler::set_sector_names_no_regions()
{
	for (int i = 0; i < _num_sectors; ++i)
	{
		if (_sector_names[i][0] == '\"')
		{
			_sector_names_no_region.push_back(_sector_names[i].substr(3, _sector_names[i].length() - 4));
		}
		else
		{
			_sector_names_no_region.push_back(_sector_names[i].substr(1));
		}
	}
}

std::pair<std::string, double> CSVhandler::buildpair(int row, int col)
{
	return std::pair<std::string, double>(db[row][0], std::stod(db[row][col]));
}


bool sorter::operator()(std::vector<double>& a, std::vector<double>& b)
{
	return a[num_regions] > b[num_regions];
}

bool sorter::operator()(std::pair<std::string,double>& a, std::pair<std::string,double>& b)
{
	return a.second > b.second;
}

//Used to sort the first table by average. Could add 1 to index and sort by std.dev
bool sorter::operator()(std::pair<std::string, std::vector<double> >& a, std::pair<std::string, std::vector<double> >& b)
{
	return a.second[num_regions] > b.second[num_regions];
}