/*
This program has been developed by students from the bachelor Computer Science at
Utrecht University within the Software Project course.
� Copyright Utrecht University (Department of Information and Computing Sciences)
*/
#pragma once
#include <vector>
#include <string>

#include "Parser.h"
#include "../spider-revisited/Spider-Revisited/CodeBlock.h"


class NetworkUtils
{
public:
	/// <summary>
	/// Converts a list of Hashes into a big string that can be send to the database.
	/// The individual parts components of each entry will be seperated by a null character.
	/// The entries will be seperated by a new line.
	/// The first entry will be the header that you give this function.
	/// </summary>
	/// 
	/// <param name="data">The hashes you want to put in the string.</param>
	/// 
	/// <param name="size">This is basically a second return value. 
	/// The size of the string that is returned will be put in here. 
	/// The initial value that you give it will be ignored.</param>
	/// 
	/// <param name="header">The header that you want infront of the string. 
	/// Even if the header is empty, it will still put an end line in the beginning.</param>
	/// 
	/// <returns>A char pointer to string with all the data in it.</returns>
	static char* getAllDataFromHashes(std::vector<HashData> data, int& size, std::string header, AuthorData& authors);

	/// <summary>
	/// This function will only copy the actual hash into a string.
	/// The hashes will be seperated by a new line
	/// </summary>
	/// 
	/// <param name="data">The hashes you want to put in the string.</param>
	/// 
	/// <param name="size">This is basically a second return value. 
	/// The size of the string that is returned will be put in here. 
	/// The initial value that you give it will be ignored.</param>
	/// 
	/// <returns>A char pointer to string with all the data in it.</returns>
	static char* getHashDataFromHashes(std::vector<HashData> data, int& size);

	/// <summary>
	/// Will put all the strings that you pass it into a single string seperated by null chars.
	/// </summary>
	static std::string generateHeader(std::vector<std::string> components);
private:
	/// <summary>
	/// Adds a string to a char* buffer.
	/// </summary>
	static void addStringToBuffer(char* buffer, int& pos, std::string adding);

	/// <summary>
	/// Adds multiple strings to a char* buffer.
	/// </summary>
	static void addStringsToBuffer(char* buffer, int& pos, std::vector<std::string> adding);

	/// <summary>
	/// Adds a hash and its authors to a buffer.
	/// </summary>
	static void addHashDataToBuffer(char* buffer, int& pos, HashData& hd, std::map<HashData, std::vector<std::string>>& authors);

	/// <summary>
	/// Transforms a list of hashes into a map. This map has as key a string
	/// corresponding to the filename that is found in hashdata.
	/// The value is a list of all hashes that are in that file.
	/// This list will be sorted by linenumber.
	/// </summary>
	/// <param name="hashes">The original hashes.</param>
	/// <param name="output">The new map.</param>
	static void transformHashList(std::vector<HashData>& hashes, std::map<std::string, std::vector<HashData*>>& output);

	/// <summary>
	/// Will match the inputlist rawData with inputlist hashes to give where they overlap.
	/// The output of this function is both the size that the author data will take up
	/// in when send to the database and the actuall strings that we are going to send.
	/// </summary>
	/// <param name="authors">The output map. Will give for each Hash in hashes a list of strings
	///		which contain who worked on the given function.</param>
	/// <param name="hashes">The function we want to find the corresponding authors for.</param>
	/// <param name="rawData">The raw author data which we are going to match with the hashes.</param>
	/// <returns>The size of all the author strings.</returns>
	static int getAuthors(std::map<HashData, std::vector<std::string>>& authors, 
		std::map<std::string, std::vector<HashData*>>& hashes, AuthorData& rawData);

};

