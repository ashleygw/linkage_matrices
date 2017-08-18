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
			total += std::stod(db[j+1][i+1]);
		sum.push_back(total);
		_sumpair.push_back(std::pair<std::string,double> (db[i+1][0], total));
	}
	_sumvec = sum;
}

std::string remove_region(std::string in)
{
	if (in[0] == '\"')
		return in.substr(3, in.length() - 4);
	else
		return in.substr(2);
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
	}
	//Sort by average
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
	set_sector_names_no_regions();

	//Fixed is unenforceable through csv output read in excel
	//Precludes need to round though.
	std::ofstream file;
	file << std::fixed;
	file.precision(2);
	file.setf(_IOSfixed);
	file.setf(_IOSshowpoint);

	//Building filename
	std::string temp = "Linkage_Table(";
	std::string temp2 = current_file;
	temp += temp2 + ").csv";
	file.open(temp.c_str());
	file << "Linkage Table," << "Filename: " << input_file << "\n,,";

	//Writes region with random sector, for categorization
	for (i = 1; i < _num_regions+1; i++)
	{
		file << db[0][i*_num_sectors];
		file << ",";
	}
	file << "Average," << "Std Dev," << "CV";
	file << "\n";
	//Writes all remaining info to file
	for (i = 0; i < _sectors.size(); i++)
	{
		//Write corresponding sector number
		file << sector_numbers[remove_region(_sectors[i].first)] << ",";
	
		//Writes sector names to file
		file << remove_region(_sectors[i].first) << ",";
		
		//Writes the rest of the info up to CV
		for (j = 0; j < _sectors[i].second.size(); j++)
		{
			//two decimal places for regions
			if(j < _num_regions)
				file << std::setprecision(1) << (_sectors[i].second[j]) << ",";
			else
				file << std::setprecision(2) << (_sectors[i].second[j]) << ",";
		}
		//Writes CV value calculated in CC table
		file << std::setprecision(2) << CVs[remove_region(_sectors[i].first)];
		file << "\n";
	}
}


void CSVhandler::writeCSV_CC(bool is_forward)
{
	//Builds filename
	std::ofstream file;
	std::string temp = "Critical_Contributors(";
	std::string temp2 = current_file;
	temp += temp2 + ").csv";
	file.open(temp.c_str());

	std::unordered_map<std::string, int> critical_sector_counter, general_sector_counter;
	std::vector<std::vector<double> > summer(_num_regions*_num_sectors);
	std::vector<double> sigma;

	int i, j, k;
	double ratios, TBLs, stddev, counter = 0, left_indexer = 1;
	
	//Precision is 2 everywhere in this file
	file.setf(std::ios::fixed);
	file.setf(std::ios::showpoint);
	file.precision(2);

	set_sector_names_no_regions();

	file << "Input filename: " << current_file << "\n";
	for (i = 0; i < _num_sectors; i++)
	{
		ratios = 0;
		TBLs = 0;
		sigma.clear();
		critical_sector_counter.clear();
		general_sector_counter.clear();

		file << left_indexer << ",";
		file << _sector_names_no_region[i] << " Sector\n";
		file << left_indexer << ",";
		file << std::fixed;
		file << std::setprecision(2);
		for (j = 0; j < _num_regions; j++)
		{
			//Regions in columns. Writes each selected sector.
			file << "Critical," << _sector_names[i + j*_num_sectors] << ",,";
		}
		file << "\n";
		file << left_indexer << ",";
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
				if (critical_sector_counter.find(name) != critical_sector_counter.end())
				{
					critical_sector_counter[name]++;
				}
				else
				{
					critical_sector_counter[name] = 1;
				}
				//Having a - at the start makes Excel treat this is a formula. Breaks program.
				if (name[0] == '\"')
				{
					if (general_sector_counter.find(name.substr(4, name.length() - 5)) != general_sector_counter.end())
					{
						general_sector_counter[name.substr(4, name.length() - 5)]++;
					}
					else
					{
						general_sector_counter[name.substr(4, name.length() - 5)] = 1;
					}
				}
				else
				{
					if (general_sector_counter.find(name.substr(2)) != general_sector_counter.end())
					{
						general_sector_counter[name.substr(2)]++;
					}
					else
					{
						general_sector_counter[name.substr(2)] = 1;
					}
				}
				
				//Calculate averages of top contributors
				summer[i + k*_num_sectors].push_back(_all_top_contributors[i + k*_num_sectors][j].second);
			}
			//if(j < _all_top_contributors[i].size() - 1)
				file << "\n";
				file << left_indexer << ",";
		}
		//file << "TBL Summary\n";
		counter = 0;
		for (j = 0; j < _num_regions; j++)
		{
			counter = 0;
			for (k = 0; k < summer[i + j*_num_sectors].size(); k++)
			{
				counter += summer[i + j*_num_sectors][k];
			}
			file << "Sub Total BL," << counter << ",,";
			summer[i + j*_num_sectors][0] = counter;
		}
		file << "\n";
		file << left_indexer << ",";
		for (j = 0; j < _num_regions; j++)
		{
			file << "TOTAL," << _sumvec[i + j*_num_sectors] << ",,";
			TBLs += _sumvec[i + j*_num_sectors];
			sigma.push_back(_sumvec[i + j*_num_sectors]);
		}
		file << "\n";
		file << left_indexer << ",";
		for (j = 0; j < _num_regions; j++)
		{
			file << "Ratio," << summer[i + j*_num_sectors][0]/_sumvec[i + j*_num_sectors]*100 << ",,";
			ratios += summer[i + j*_num_sectors][0] / _sumvec[i + j*_num_sectors] * 100;
		}
		file << "\n";
		file << left_indexer << ",";

		//calculate sigma https://goo.gl/BC36Tq
		double sum = std::accumulate(sigma.begin(), sigma.end(), 0.0);
		double mean = sum / sigma.size();

		std::vector<double> diff(sigma.size());
		std::transform(sigma.begin(), sigma.end(), diff.begin(), [mean](double x) { return x - mean; });
		double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		stddev = std::sqrt(sq_sum / sigma.size());
		
		//Section 2 Critical Sectors
		//Data is small enough to allow multiple passes
		file << "Common Multiregional Critical Sectors,,,,,,,,,TBL Summary\n";
		file << left_indexer << ",";

		//These variables are used to count the number of commas needed for proper formatting
		int indexer = 0;
		int new_catagory_counter = 3; //stddev,cvs,tbls,ave - 1
		int reverse_index = 0;
		for (j = _num_upperbound_reported_contributors; j >= _num_lowerbound_reported_contributors; --j)
		{
			counter = 0; // Used as boolean with more functionality
			reverse_index = 0; // used to limit commas for file write
			if (j == _num_upperbound_reported_contributors)
			{
				file << j << " or more,";
			}
			else
				file << j << ",";
				
			for (auto it = critical_sector_counter.begin(); it != critical_sector_counter.end(); ++it)
			{
				if (it->second == j)
				{
					file << it->first << ","; //<< " (" << it->second << "),";
					reverse_index++;
					counter++;
				}
			}
			//Checks if no critical sectors registered
			if (counter == 0)
			{
				file << "None,";
				reverse_index++;
			}
			switch (new_catagory_counter)
			{
				case(3): 
				{
					for (int n = 0; n < 8 - reverse_index; ++n)
					{
						file << ",";
					}
					file << "Ave-TBL," << TBLs / _num_regions;
					break;
				}
				case(2):
				{
					for (int n = 0; n < 8 - reverse_index; ++n)
					{
						file << ",";
					}
					file << "Stdev," << stddev;
					break;
				}
				case(1):
				{
					for (int n = 0; n < 8 - reverse_index; ++n)
					{
						file << ",";
					}
					file << "CV," << (TBLs / _num_regions) / stddev;
					CVs[_sector_names_no_region[i]] = (TBLs / _num_regions) / stddev;
					break;
				}
				case(0):
				{
					for (int n = 0; n < 8 - reverse_index; ++n)
					{
						file << ",";
					}
					file << "Ratio Ave," << ratios / _num_regions;
					break;
				}
			}
			--new_catagory_counter;
			file << "\n";
			file << left_indexer << ",";
		}
			
		
		counter = 0;
		int reverse_index2 = 0;
		file << "All Common Critical Sectors";
		switch (new_catagory_counter)
		{
			case(2) :
			{
				file << ",,,,,,,,,Stdev," << stddev;
				break;
			}
			case(1):
			{
				file << ",,,,,,,,,CV," << (TBLs / _num_regions) / stddev;
				CVs[_sector_names_no_region[i]] = (TBLs / _num_regions) / stddev;
				break;
			}
			case(0) :
			{
				file << ",,,,,,,,,Ratio Ave," << ratios / _num_regions;
				break;
			}
		}
		file << "\n";
		new_catagory_counter--;
		file << left_indexer << ",";

		//Writes general sectors
		for (auto it = general_sector_counter.begin(); it != general_sector_counter.end(); ++it)
		{
			//if (it->second >= _num_regions)
			if (it->second >= _general_sector_contributors)
			{
				file << it->first << "(" << it->second << "),";
				reverse_index2++;
				//form is current sector - found notable sector - forward or backward
				//This is stored so that if the user wants a KCT these values don't have to be
				//Calculated again.
				if(is_forward)
					general_collection.push_back(Triad(_sector_names[i].substr(2), it->first, "F"));
				else
					general_collection.push_back(Triad(_sector_names[i].substr(2), it->first, "B"));
			}
			
		}
		switch (new_catagory_counter)
		{
			case(3) :
			{
				for (int n = 0; n < 9 - reverse_index2; ++n)
				{
					file << ",";
				}
				file << "Ave-TBL," << TBLs / _num_regions;
				break;
			}
			case(2) :
			{
				for (int n = 0; n < 9 - reverse_index2; ++n)
				{
					file << ",";
				}
				file << "Stdev," << stddev;
				break;
			}
			case(1) :
			{
				for (int n = 0; n < 9 - reverse_index2; ++n)
				{
					file << ",";
				}
				file << "CV," << (TBLs / _num_regions) / stddev;
				CVs[_sector_names_no_region[i]] = (TBLs / _num_regions) / stddev;
				break;
			}
			case(0) :
			{
				for (int n = 0; n < 9 - reverse_index2; ++n)
				{
					file << ",";
				}
				file << "Ratio Ave," << ratios / _num_regions;
				break;
			}
		}
		file << "\n\n";
		left_indexer++;
	}
}

void CSVhandler::clear_data()
{
	//Does not clear the database- Call cleardb
	//CVs should be cleared at the same time as db, it's in that function
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
	CVs.clear();
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
void CSVhandler::writeCC(std::istream & input, bool forward_table)
{
	clear_data();
	readCSV(input);
	set_sector_names();
	sum();
	build_regions();
	build_sectors();
	set_all_top_contributors();
	writeCSV_CC(forward_table);
}

void CSVhandler::writeCSV_CT()
{
	std::ofstream file;
	int i;
	std::unordered_map<std::string, int> sec_counter;
	//Build filename
	std::string temp = "FL_BL_Table(";
	std::string temp2 = input_file;
	std::string temp3 = additional_file;
	temp += temp2 + " " + temp3 + ").csv";

	file.open(temp.c_str());
	file << "Input filenames:,,, " << input_file << "," << additional_file << "\n";
	file << "General Threshold:,,," << _general_sector_contributors << "\n,";
	
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

		//Write buffer is used to keep appropriate indexes in table
		std::vector<std::string> write_buffer(_num_sectors);

		//check general collection for common contributor
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
					//This should not happen- chances are sectors are named something crazy
					std::cout << "Critical naming error. Try renaming sectors in form H4_Finance for example." << std::endl;
				}
				else
				{
					index = std::distance(_sector_names_no_region.begin(), it);
				}
				std::string link = std::get<2>(general_collection[j]);

				//Writes appropriate symbol to buffer
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
}

//Builds vector of sector names
void CSVhandler::set_sector_names()
{
	std::vector<std::string> temp;
	for (int i = 1; i < db.size(); i++)
	{
		temp.push_back(db[i][0]);
	}
	_sector_names = temp;
}

//Builds a single pair of top contributors
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

void CSVhandler::set_general_sector_contributors(int in)
{
	_general_sector_contributors = in;
}

void CSVhandler::set_input_filename(char * in)
{
	input_file = in;
}

void CSVhandler::set_add_input_filename(char * name)
{
	additional_file = name;
}

//Handles whether we are dealing with a forward or backward linkage table
void CSVhandler::set_flag(bool boo)
{
	forward_flag = boo;
}

void CSVhandler::set_sector_names_no_regions()
{
	_sector_names_no_region.clear();
	sector_numbers.clear();
	for (int i = 0; i < _num_sectors; ++i)
	{
		sector_numbers[remove_region(_sector_names[i])] = i+1;
		_sector_names_no_region.push_back(remove_region(_sector_names[i]));
	}
}

std::pair<std::string, double> CSVhandler::buildpair(int row, int col)
{
	return std::pair<std::string, double>(db[row][0], std::stod(db[row][col]));
}

/*Sorting functions below*/
bool sorter::operator()(std::vector<double>& a, std::vector<double>& b)
{
	return a[num_regions] > b[num_regions];
}

bool sorter::operator()(std::pair<std::string,double>& a, std::pair<std::string,double>& b)
{
	return a.second > b.second;
}

//Used to sort the first table by average. Could add 1 to index and sort by std.dev - Sort in Excel for CVs
bool sorter::operator()(std::pair<std::string, std::vector<double> >& a, std::pair<std::string, std::vector<double> >& b)
{
	return a.second[num_regions] > b.second[num_regions];
}