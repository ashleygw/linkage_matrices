#ifndef CSV_H
#define CSV_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <unordered_map>

typedef std::tuple<std::string, std::string, std::string> Triad;

class CSVhandler {
public:
	std::vector<std::vector<std::string> > db;
	void readCSV(std::istream &input);
	void writeCSV_FLT();
	void writeFLT(std::istream &input);
	void writeKC(std::istream &input, bool);
	void writeCSV_KT();
	void sum();
	void build_regions();
	void build_sectors();
	void set_sector_names();
	void set_sectors(const int &);
	void set_all_top_contributors();
	void set_num_top_contributors(int);
	void set_reported_contributors(int, int);
	void set_input_filename(char*);
	void set_add_input_filename(char*);
	void set_flag(bool);
	void set_sector_names_no_regions();
	void writeCSV_KC(bool);
	void clear_data();
	void clear_db();
	std::pair<std::string, double> buildpair(int, int);
	std::vector<std::pair<std::string, double> > one_sector_top_contributors(int);

	int _num_sectors;
	int _num_regions;
	int _num_top_contributors = 3;
	int _num_upperbound_reported_contributors = 4;
	int _num_lowerbound_reported_contributors = 3;

	//Used in building the Forward Link table
	std::vector<double> _sumvec;
	std::vector<std::pair<std::string, double> > _sumpair;
	std::vector<std::vector<double> > _regions;

	//All sectors and their corresponding values
	std::vector<std::pair<std::string,std::vector<double> > > _sectors;
	//All Sector names with regions attached
	std::vector<std::string> _sector_names;
	//Only sector names
	std::vector<std::string> _sector_names_no_region;

	std::vector<std::vector<std::pair<std::string, double> > > _all_top_contributors;
	std::vector<Triad> general_collection;

	char* input_file = "";
	char* additional_file = "";
	char* current_file = "";

	//Is the current loaded database a forward link table
	bool forward_flag = false;
};
class sorter {
public:
	bool operator()(std::vector<double>& a, std::vector<double>& b);
	bool operator()(std::pair<std::string, double>& a, std::pair<std::string, double>& b);
	bool operator()(std::pair<std::string, std::vector<double> >& a, std::pair<std::string, std::vector<double> >& b);
	int num_regions;
	int _num_top_contributors;
};
#endif