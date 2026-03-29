/* utils.h */
#pragma once


#include <sstream>
#include <fstream>
#include <chrono>
#include "types.h"


namespace utils {

void toUpper(std::string& text) {
	std::transform(
		text.begin(), text.end(), text.begin(),
		[](unsigned char c){return std::toupper(c);}
	);
}
void toLower(std::string& text) {
	std::transform(
		text.begin(), text.end(), text.begin(),
		[](unsigned char c){return std::tolower(c);}
	);
}

std::string getCurrentDate() {
	auto now = std::chrono::system_clock::now();
	auto days = floor<std::chrono::days>(now);
	auto ymd = std::chrono::year_month_day(days);

	return std::format(
		"{:04}-{:02}-{:02}",
		static_cast<int>(ymd.year()),
		static_cast<unsigned int>(ymd.month()),
		static_cast<unsigned int>(ymd.day())
	);
}

}



namespace file {

static std::string readFile(const std::string& filePath) {
	std::ifstream fileStream(filePath, std::ios::binary);
	if (!fileStream.is_open()) {
		throw std::runtime_error("Error: Could not open file: " + filePath);
	}

	std::ostringstream buffer;
	buffer << fileStream.rdbuf();
	return buffer.str();
}


std::vector<std::string> readFileLines(const std::string& filePath) {
	std::string text = readFile(filePath);

	std::vector<std::string> lines;
	std::stringstream ss(text);
	std::string line;

	while (std::getline(ss, line)) {
		lines.push_back(line);
	}

	return lines;
}

}


