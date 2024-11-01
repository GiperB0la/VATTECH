#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>


struct Record 
{
    std::string filename;
    double latitude;
    double longitude;
    double height;
    std::string time;
};

class Work
{
public:
    Work() = default;
	~Work() = default;

public:
    void start(std::string Path = "");

private:
	void read_folder_YYYYMMDD();
	void read_folder_YYYYMMDD_TrackNN();
	void read_track_files();
    void interpolate_coordinates(std::vector<Record>& records);
    void write_interpolated_file(const std::filesystem::path& trackPath, const std::vector<Record>& records);

private:
	std::filesystem::path dirPath = "../Folders";
    std::vector<std::filesystem::directory_entry> YYYYMMDD;
    std::vector<std::filesystem::directory_entry> YYYYMMDD_TrackNN;
};