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

class CSVhandler {
public:
	std::vector<std::vector<std::string> > db;
	void readCSV(std::istream &input);
	void writeCSV_FLT();
	void writeFLT(std::istream &input);
	void writeKC(std::istream &input);
	void sum();
	void build_regions();
	void build_sectors();
	void set_sector_names();
	void set_sectors(const int &);
	void set_all_top_contributors();
	void set_num_top_contributors(int);
	void set_reported_contributors(int, int);
	void set_input_filename(char*);
	void writeCSV_KC();
	void clear_data();
	std::pair<std::string, double> buildpair(int, int);
	std::vector<std::pair<std::string, double> > one_sector_top_contributors(int);

	int _num_sectors;
	int _num_regions;
	int _num_top_contributors = 3;
	int _num_upperbound_reported_contributors = 4;
	int _num_lowerbound_reported_contributors = 3;

	std::vector<double> _sumvec;
	std::vector<std::pair<std::string, double> > _sumpair;
	std::vector<std::vector<double> > _regions;
	std::vector<std::pair<std::string,std::vector<double> > > _sectors;
	std::vector<std::string> _sector_names;
	std::vector<std::vector<std::pair<std::string, double> > > _all_top_contributors;
	char* input_file = "";
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