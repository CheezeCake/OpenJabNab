#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "speakerReco.h"

// Binaries
const std::string speakerReco::sfbcepPath = "./sfbcep";
const std::string speakerReco::normFeatPath = "./NormFeat";
const std::string speakerReco::ivExtractorPath = "./IvExtractor";
const std::string speakerReco::ivTestPath = "./IvTest";

// Directories
const std::string speakerReco::prmDir = "../resources/alize/prm/";
const std::string speakerReco::ndxDir = "../resources/alize/ndx/";

// Config files
const std::string speakerReco::normFeatCfgPath = "../resources/alize/cfg/NormFeat.cfg";
const std::string speakerReco::ivExtractorCfgPath = "../resources/alize/cfg/ivExtractor_fast.cfg";
const std::string speakerReco::ivTestCfgPath = "../resources/alize/cfg/ivTest_Cosine.cfg";

// Others
const std::string speakerReco::resultFilePath = "../resources/alize/res/scores_Cosine.txt";
const std::string speakerReco::timeDataFile = "time.txt";

std::string speakerReco::basename(std::string original)
{
	int slashPos = original.rfind('/');
	return original.substr(slashPos + 1, original.rfind('.') - slashPos - 1);
}

void speakerReco::writeTime(std::string what, double time)
{
	std::ofstream timeData(timeDataFile.c_str(), std::ios_base::app | std::ios_base::out);
	if(!timeData)
	{
		std::cerr << "Unable to open " + timeDataFile << std::endl;
		return;
	}
	timeData << time << " SECONDS TO " << what << '\n';
}

int speakerReco::setUpParamFile(std::string sphFile, bool isSph = false)
{
	pid_t pid = fork();
	int status = 1;
	std::string name = basename(sphFile);
	if(pid == 0)
	{
		if(isSph)
			execl(sfbcepPath.c_str(), "sfbcep", "-F", "SPHERE", "-p", "19", "-e", "-D", "-A", "-k", "0", sphFile.c_str(), (prmDir + name + ".prm").c_str(), NULL);
		else
			execl(sfbcepPath.c_str(), "sfbcep", "-p", "19", "-e", "-D", "-A", "-k", "0", sphFile.c_str(), (prmDir + name + ".prm").c_str(), NULL);

		std::cerr << "Error exec sfbcep\n";
		exit(1);
	}
	else if(pid > 0)
		waitpid(pid, &status, 0);
	else
		std::cerr << "Fork failed for sfbcep" << std::endl;

	return status;
}

int speakerReco::normalizeParam(std::string paramFile)
{
	pid_t pid = fork();
	int status = 1;
	if(pid == 0)
	{
		execl(normFeatPath.c_str(), "NormFeat", "--config", normFeatCfgPath.c_str(), "--inputFeatureFilename", paramFile.c_str(), "--featureFilesPath", prmDir.c_str(), NULL);
		std::cerr << "Error exec NormFeat\n";
		exit(1);
	}
	else if(pid > 0)
		waitpid(pid, &status, 0);
	else
		std::cerr << "Fork failed for NormFeat" << std::endl;

	return status;
}

int speakerReco::createIvector(std::string targetIndex) // list of files to create a vector for
{
	pid_t pid = fork();
	int status = 1;
	//time_t start = time(NULL);
	if(pid == 0)
	{
		std::cout << ivExtractorPath << " " << "--config " << ivExtractorCfgPath << " --targetIdList " << targetIndex << " --featureFilesPath " << prmDir << std::endl;
		execl(ivExtractorPath.c_str(), "IvExtractor", "--config", ivExtractorCfgPath.c_str(), "--targetIdList", targetIndex.c_str(), "--featureFilesPath", prmDir.c_str(), NULL);
		std::cerr << "Error exec IvExtractor\n";
		exit(1);
	}
	else if(pid > 0)
	{
		waitpid(pid, &status, 0);
		//writeTime("CREATE VECTORS FROM " + targetIndex, difftime(time(NULL), start));
	}
	else
		std::cerr << "Fork failed for IvExtractor" << std::endl;

	return status;
}

int speakerReco::test(std::string testIndex)
{
	pid_t pid = fork();
	int status = 1;
	if(pid == 0)
	{
		std::cout << ivTestPath << " --config " << ivTestCfgPath << " --ndxFilename " << testIndex << std::endl;
		execl(ivTestPath.c_str(), "IvTest", "--config", ivTestCfgPath.c_str(), "--ndxFilename", testIndex.c_str(), NULL);
		std::cerr << "Error exec IvTest\n";
		exit(1);
	}
	else if(pid > 0)
		waitpid(pid, &status, 0);
	else
		std::cerr << "Fork failed for IvTest" << std::endl;

	return status;
}

std::string speakerReco::analyseResults(std::string resultFile, std::string tested)
{
	std::ifstream file(resultFile.c_str());

	std::string testedFile;
	std::string model;
	float score;

	float bestScore = 0;
	std::string bestModel;

	std::string line;
	if(!file)
	{
		std::cerr << "Unable to open " + resultFile << std::endl;
		return "";
	}
	while(!file.eof())
	{
		file >> model; // get rid or the leading M
		file >> model;
		file >> testedFile; // get rid of the 1
		file >> testedFile;
		file >> score;
		if(testedFile == tested && score > bestScore)
		{
			bestScore = score;
			bestModel = model;
		}
	}
	file.close();

	if(bestScore != 0)
		return bestModel;

	return "";
}

// Creates single line modelsIndex or recordsIndex file to set up model or record named "name"
// with "params" prm
void speakerReco::generateIndex(std::string indexPath, std::string name, std::vector<std::string> params)
{
	if(params.size() == 0)
		return;

	std::ofstream index(indexPath.c_str());
	if(!index)
	{
		std::cerr << "Unable to open " + indexPath << std::endl;
		return;
	}
	index << name;
	int size = params.size();
	for(int i = 0; i < size; i++)
	{
		index << ' ' << params[i];
	}
}

void speakerReco::generateTestIndex(std::string modelsIndexPath, std::string recordsIndexPath, std::string testIndexPath)
{
	std::string modelsString = "";
	std::ifstream modelsIndex(modelsIndexPath.c_str());

	if(!modelsIndex)
	{
		std::cerr << "Unable to open " + modelsIndexPath << std::endl;
		return;
	}
	std::string line;
	while(getline(modelsIndex, line))
	{
		modelsString += ' ' + line.substr(0, line.find(' '));
	}
	modelsIndex.close();

	std::ifstream recordsIndex(recordsIndexPath.c_str());
	std::ofstream testIndex(testIndexPath.c_str());

	if(!recordsIndex || !testIndex)
	{
		std::cerr << "Unable to open " + recordsIndexPath << " or " << testIndexPath << std::endl;
		return;
	}
	while(getline(recordsIndex, line))
	{
		testIndex << line.substr(0, line.find(' '));
		testIndex << modelsString;
		testIndex << '\n';
	}
	recordsIndex.close();
	testIndex.close();
}

void speakerReco::addModel(std::string modelsFile, std::string name, std::string vectorName)
{
	std::ofstream models(modelsFile.c_str(), std::ios_base::app | std::ios_base::out);
	if(!models)
	{
		std::cerr << "Unable to open " + modelsFile << std::endl;
		return;
	}
	models << name << ' ' << vectorName << '\n';
}

void speakerReco::registerModel(std::string name, std::vector<std::string> audioFilesPaths)
{
	//time_t start = time(NULL);
	int size = audioFilesPaths.size();
	if(size == 0)
		return;

	std::vector<std::string> basenames;

	for(int i = 0; i < size; i++)
	{
		std::string file = audioFilesPaths[i];
		setUpParamFile(file);
		normalizeParam(basename(file));
		basenames.push_back(basename(file));
	}
	generateIndex(ndxDir + "temp.ndx", name, basenames);
	addModel(ndxDir + "models.ndx", name, name);
	createIvector(ndxDir + "temp.ndx");
	//writeTime("REGISTER MODEL", difftime(time(NULL), start));
}

std::string speakerReco::identify(std::vector<std::string> audioFilesPaths)
{
	int size = audioFilesPaths.size();
	if(size == 0)
		return NULL;

	std::vector<std::string> basenames;

	for(int i = 0; i < size; i++)
	{
		std::string file = audioFilesPaths[i];
		setUpParamFile(file);
		normalizeParam(basename(file));
		basenames.push_back(basename(file));
	}
	generateIndex(ndxDir + "temp.ndx", "temp", basenames);
	createIvector(ndxDir + "temp.ndx");
	generateTestIndex(ndxDir + "models.ndx", ndxDir + "temp.ndx", ndxDir + "test.ndx");
	test(ndxDir + "test.ndx");
	return analyseResults(resultFilePath, "temp");
}

std::string speakerReco::unUnderscore(std::string original)
{
	size_t under = original.find('_');
	while(under != std::string::npos)
	{
		original.replace(under, 1, " ");
		under = original.find('_');
	}
	return original;
}

std::string speakerReco::underscore(std::string original)
{
	size_t space = original.find(' ');
	while(space != std::string::npos)
	{
		original.replace(space, 1, "_");
		space = original.find(' ');
	}
	return original;
}
