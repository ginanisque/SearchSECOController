#pragma once
#include <string>
#include <vector>

struct HashData;
class NetworkHandler;

class DatabaseRequests
{
public:
	static std::string uploadHashes(std::vector<HashData> hashes);
	static std::string findMatches(std::vector<HashData> hashes);
private:
	static std::string execRequest(std::string request, char* rawData, int dataSize);
	static NetworkHandler* startConnection();
};

