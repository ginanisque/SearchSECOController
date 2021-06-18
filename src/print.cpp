/*
This program has been developed by students from the bachelor Computer Science at
Utrecht University within the Software Project course.
© Copyright Utrecht University (Department of Information and Computing Sciences)
*/

// Controller includes.
#include "databaseRequests.h"
#include "networkUtils.h"
#include "print.h"
#include "utils.h"

// Parser includes.
#include "Parser.h"

// External includes
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <StringStream.h>
#include <iomanip>

inline
bool operator<(HashData const& lhs, HashData const& rhs)
{ 
	if (lhs.hash != rhs.hash)
	{
		return lhs.hash < rhs.hash;
	}
	if (lhs.fileName != rhs.fileName)
	{
		return lhs.fileName < rhs.fileName;
	}
	if (lhs.functionName != rhs.functionName)
	{
		return lhs.functionName < rhs.functionName;
	}
	if (lhs.lineNumber != rhs.lineNumber)
	{
		return lhs.lineNumber < rhs.lineNumber;
	}
	return lhs.lineNumberEnd < rhs.lineNumberEnd;
}

#pragma region Logging

void print::debug(std::string msg, const char* file, int line)
{
	loguru::log(loguru::Verbosity_1, file, line, "%s", msg.c_str());
}

void print::log(std::string msg, const char* file, int line)
{
	loguru::log(loguru::Verbosity_INFO, file, line, "%s", msg.c_str());
}

void print::warn(std::string msg, const char* file, int line)
{
	loguru::log(loguru::Verbosity_WARNING, file, line, "%s", msg.c_str());
}

void print::loguruSetSilent()
{
	loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
}

void print::loguruResetThreadName() 
{
	loguru::set_thread_name("controller");
}

#pragma endregion


#pragma region Print
std::string line(std::string str)
{
	return str + '\n';
}

void print::printline(std::string str)
{
	std::cout << line(str);
}

void print::writelineToFile(std::string str, std::ofstream& file)
{
	file << line(str);
}

void print::printAndWriteToFile(std::string str, std::ofstream &file)
{
	print::printline(str);
	print::writelineToFile(str, file);
}

std::string print::tab(int n)
{
	std::string result = "";
	for (int i = 0; i < n; ++i)
	{
		result += "\t";
	}
	return result;
}

std::string print::encapsulate(std::string str, char c)
{
	return c + str + c;
}

std::string print::quote(std::string str)
{
	return encapsulate(str, '\"');
}

std::string print::plural(std::string singular, int n)
{
	if (n == 1)
	{
		return singular;
	}
	return singular + 's';
}

void print::versionFull()
{
	std::string mainName = "searchseco";

	// Get subsystem versions.
	int systemc = 3;
	std::string* subsystems = new std::string[systemc]
	{
		"parser",
		"crawler",
		"spider",
		//"database_api"
	};

	std::ifstream versionFile;
	std::string version;

	// print version of the main program.
	versionFile.open("VERSION");
	std::getline(versionFile, version);

	print::printline(mainName + " version " + version);
	
	versionFile.close();

	// Loop over the subsystems.
	for (int i = 0; i < systemc; ++i)
	{
		std::string system = subsystems[i];
		versionFile.open(system + "/VERSION");

		std::getline(versionFile, version);

		print::printline(">> " + system + " version " + version);
		
		versionFile.close();
	}
}
#pragma endregion Print

#pragma region Matches

void printMatches::printHashMatches(
	std::vector<HashData> &hashes, 
	std::string databaseOutput, 
	AuthorData &authordata, 
	std::string url)
{
	std::map<std::string, std::vector<std::string>> receivedHashes = {};

	std::map<std::pair<std::string, std::string>, int> projects;
	std::map<std::string, int> dbAuthors;
	std::map<std::string, std::vector<std::string>> dbProjects;
	std::map<std::string, std::vector<std::string>> authorIdToName;

	// Seperate the response we got into its individual entries.
	std::vector<std::string> dbentries = utils::split(databaseOutput, ENTRY_DELIMITER);
	parseDatabaseHashes(dbentries, receivedHashes, projects, dbAuthors);
	
	getDatabaseAuthorAndProjectData(projects, dbAuthors, dbProjects, authorIdToName);

	// Author data.
	std::map<std::string, std::vector<HashData*>> transformedList;
	std::map<HashData, std::vector<std::string>> authors;
	NetworkUtils::transformHashList(hashes, transformedList);
	NetworkUtils::getAuthors(authors, transformedList, authordata);

	auto report = printMatches::setupOutputReport(url);

	int matches = 0;
	std::map<std::string, int> authorCopiedForm;
	std::map<std::string, int> authorsCopied;
	// Add all hashes we got localy to a map.
	for (int i = 0; i < hashes.size(); i++)
	{
		if (receivedHashes.count(hashes[i].hash) > 0)
		{
			matches++;
			printMatch(
				hashes[i], 
				receivedHashes, 
				authors, 
				authorCopiedForm, 
				authorsCopied, 
				dbProjects, 
				authorIdToName,
				report
			);
		}
	}

	print::writelineToFile("\n\n", report);

	printSummary(
		authorCopiedForm, 
		authorsCopied, 
		matches, 
		hashes.size(), 
		dbProjects, 
		authorIdToName, 
		projects,
		report
	);
	report.close();
}

void printMatches::parseDatabaseHashes(
	std::vector<std::string>& dbentries,
	std::map<std::string, std::vector<std::string>>& receivedHashes,
	std::map<std::pair<std::string, std::string>, int> &projects,
	std::map<std::string, int> &dbAuthors)
{
	for (std::string entry : dbentries)
	{
		if (entry == "")
		{
			continue;
		}
		std::vector<std::string> entrySplitted = utils::split(entry, INNER_DELIMITER);

		receivedHashes[entrySplitted[0]] = entrySplitted;
		if (entrySplitted.size() < 10)
		{
			continue;
		}
		for (int i = 10; i < 10 + std::stoi(entrySplitted[9]); i++)
		{
			dbAuthors[entrySplitted[i]]++;
		}
		projects[std::pair<std::string, std::string>(entrySplitted[1], entrySplitted[2])]++;
	}
}

void printMatches::getDatabaseAuthorAndProjectData(
	std::map<std::pair<std::string, std::string>, int>& projects,
	std::map<std::string, int> &dbAuthors,
	std::map<std::string, std::vector<std::string>>& dbProjects,
	std::map<std::string, std::vector<std::string>>& authorIdToName)
{
	// Database requests.
	std::vector<std::string> authorEntries =
		utils::split(DatabaseRequests::getAuthor(dbAuthors), ENTRY_DELIMITER);
	std::vector<std::string> projectEntries =
		utils::split(DatabaseRequests::getProjectData(projects), ENTRY_DELIMITER);

	// Getting the project data out of it.
	for (int i = 0; i < projectEntries.size(); i++)
	{
		auto splitted = utils::split(projectEntries[i], INNER_DELIMITER);
		if (splitted.size() == 1)
		{
			continue;
		}
		dbProjects[splitted[0]] = splitted;
	}
	// Getting the author data out of it.
	for (int i = 0; i < authorEntries.size(); i++)
	{
		auto splitted = utils::split(authorEntries[i], INNER_DELIMITER);
		if (splitted.size() == 1)
		{
			continue;
		}
		authorIdToName[splitted[2]] = splitted;
	}
}

void printMatches::printMatch(
	HashData hash, 
	std::map<std::string, std::vector<std::string>>& receivedHashes, 
	std::map<HashData, std::vector<std::string>>& authors, 
	std::map<std::string, int>& authorCopiedForm, 
	std::map<std::string, int>& authorsCopied,
	std::map<std::string, std::vector<std::string>>& dbProjects,
	std::map<std::string, std::vector<std::string>>& authorIdToName,
	std::ofstream &report
) 
{
	std::vector<std::string> dbEntry = receivedHashes[hash.hash];
	print::printAndWriteToFile("\n" + hash.functionName + " in file " + hash.fileName + " line "
		+ std::to_string(hash.lineNumber) + " was found in our database: ", report);
	print::printAndWriteToFile("Function " + dbEntry[6] + " in project " + dbProjects[dbEntry[1]][4]
		+ " in file " + dbEntry[7] + " line " + dbEntry[8], report);
	print::printAndWriteToFile("Authors of local function: ", report);
	for (std::string s : authors[hash])
	{
		utils::replace(s, INNER_DELIMITER, '\t');
		print::printAndWriteToFile(s, report);
		authorsCopied[s]++;
	}
	print::printAndWriteToFile("Authors of function found in database: ", report);
	
	for (int i = 10; i < 10 + std::stoi(dbEntry[9]); i++)
	{
		if (authorIdToName.count(dbEntry[i]) > 0)
		{
			authorCopiedForm[dbEntry[i]]++;
			print::printAndWriteToFile(
				"\t" + authorIdToName[dbEntry[i]][0] + '\t' + authorIdToName[dbEntry[i]][1], 
				report);
		}
	}
}

void printMatches::printSummary(std::map<std::string, int> &authorCopiedForm,
	std::map<std::string, int> &authorsCopied,
	int matches, int methods,
	std::map<std::string, std::vector<std::string>>& dbProjects,
	std::map<std::string, std::vector<std::string>>& authorIdToName,
	std::map<std::pair<std::string, std::string>, int> &projects,
	std::ofstream &report)
{
	print::printAndWriteToFile("\nSummary:", report);
	print::printAndWriteToFile("Methods in checked project: " + std::to_string(methods), report);

	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << ((float)matches * 100 / methods);
	print::printAndWriteToFile("Matches: " + std::to_string(matches) + " (" + stream.str() + "%)", report);
	print::printAndWriteToFile("Projects found in database:", report);

	for (const auto& x : projects)
	{
		print::printAndWriteToFile(
			"\t" + dbProjects[x.first.first][3] + ": " + std::to_string(x.second)
			+ " (" + dbProjects[x.first.first][4] + ")", 
			report
		);
	}

	print::printAndWriteToFile("\nLocal authors present in matches: ", report);
	for (auto const& x : authorsCopied)
	{
		print::printAndWriteToFile(x.first + ": " + std::to_string(x.second), report);
	}

	print::printAndWriteToFile("\nAuthors present in database matches: ", report);
	for (auto const& x : authorCopiedForm)
	{
		print::printAndWriteToFile(
			'\t' + authorIdToName[x.first][0] + "\t" + authorIdToName[x.first][1] + ": " + std::to_string(x.second), 
			report
		);
	}
}

std::ofstream printMatches::setupOutputReport(std::string url)
{
	auto projectName = utils::split(url, '/').back();

	std::ofstream report;
	auto filename = "reports/" + projectName + ".txt";

	std::filesystem::create_directory("reports");

	report.open(filename, std::ios::trunc);

	auto header = "Matched the project at \"" + url + "\" against the database.";
	print::writelineToFile(header, report);
	print::writelineToFile(std::string(header.length(), '_') + '\n', report);

	return report;
}

#pragma endregion Matches
