#include "../include/Work.h"


void Work::start(std::string Path)
{
    if (Path != "") {
        dirPath = Path;
    }

    read_folder_YYYYMMDD();
    read_folder_YYYYMMDD_TrackNN();
    read_track_files();

    std::string str;
    std::cout << "[+] Ready.\nPress any key..." << std::endl;
    std::getline(std::cin, str);
}

void Work::read_folder_YYYYMMDD()
{
    try {
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
            dirPath = "../../Folders";

            if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
                std::cerr << "[-] Error path." << std::endl;
                return;
            }
        }

        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_directory()) {
                YYYYMMDD.push_back(entry);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[-] Error filesystem: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[-] Error: " << e.what() << std::endl;
    }
}

void Work::read_folder_YYYYMMDD_TrackNN()
{
    for (const auto& dateFolder : YYYYMMDD) {
        std::filesystem::path trackDirPath = dateFolder.path();

        try {
            if (std::filesystem::exists(trackDirPath) && std::filesystem::is_directory(trackDirPath)) {
                for (const auto& entry : std::filesystem::directory_iterator(trackDirPath)) {
                    if (std::filesystem::is_directory(entry.status())) {
                        YYYYMMDD_TrackNN.push_back(entry);
                    }
                }
            }
            else {
                std::cerr << "[-] Error: Path " << trackDirPath << " does not exist or is not a directory." << std::endl;
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[-] Error accessing folder " << trackDirPath << ": " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[-] General error: " << e.what() << std::endl;
        }
    }
}

void Work::read_track_files()
{
    for (const auto& trackFolder : YYYYMMDD_TrackNN) {
        std::filesystem::path trackPath = trackFolder.path();
        std::string trackName = trackPath.filename().string();
        std::string filePattern = trackName + "_C1_";

        for (const auto& entry : std::filesystem::directory_iterator(trackPath)) {
            if (entry.is_regular_file() && entry.path().filename().string().find(filePattern) != std::string::npos) {
                std::ifstream inFile(entry.path());
                std::string line;
                std::vector<Record> records;

                while (std::getline(inFile, line)) {
                    Record record;
                    std::istringstream iss(line);

                    std::getline(iss, record.filename, ';');
                    std::string token;

                    std::getline(iss, token, ';');
                    record.latitude = std::stod(token);
                    std::getline(iss, token, ';');
                    record.longitude = std::stod(token);
                    std::getline(iss, token, ';');
                    record.height = std::stod(token);
                    std::getline(iss, record.time, ';');

                    records.push_back(std::move(record));
                }

                interpolate_coordinates(records);

                write_interpolated_file(trackPath, records);
            }
        }
    }
}

void Work::interpolate_coordinates(std::vector<Record>& records) {
    for (size_t i = 1; i < records.size() - 1; ++i) {
        if (records[i].latitude == 0.0 && records[i].longitude == 0.0) {
            int prev = i - 1, next = i + 1;

            while (prev >= 0 && (records[prev].latitude == 0.0 || records[prev].longitude == 0.0)) --prev;
            while (next < records.size() && (records[next].latitude == 0.0 || records[next].longitude == 0.0)) ++next;

            if (prev >= 0 && next < records.size()) {
                double fraction = static_cast<double>(i - prev) / (next - prev);
                records[i].latitude = records[prev].latitude + fraction * (records[next].latitude - records[prev].latitude);
                records[i].longitude = records[prev].longitude + fraction * (records[next].longitude - records[prev].longitude);
                records[i].height = records[prev].height + fraction * (records[next].height - records[prev].height);
            }
        }
    }
}

void Work::write_interpolated_file(const std::filesystem::path& trackPath, const std::vector<Record>& records) {
    std::filesystem::path outputFilePath = trackPath / "interpolated_output.txt";
    std::ofstream outFile(outputFilePath);

    for (const auto& record : records) {
        outFile << record.filename << ';'
            << std::fixed << std::setprecision(8) << record.latitude << ';'
            << std::fixed << std::setprecision(8) << record.longitude << ';'
            << std::fixed << std::setprecision(1) << record.height << ';'
            << record.time << '\n';
    }
}