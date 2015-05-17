#include <iostream>
#include <vector>

class speakerReco
{
	private:
		// Binaries
		static const std::string sfbcepPath;
		static const std::string normFeatPath;
		static const std::string ivExtractorPath;
		static const std::string ivTestPath;
		static const std::string trainTargetPath;
		static const std::string computeTestPath;

		// Directories
		static const std::string prmDir;
		static const std::string ndxDir;

		// Config files
		static const std::string normFeatCfgPath;
		static const std::string ivExtractorCfgPath;
		static const std::string ivTestCfgPath;
		static const std::string trainTargetCfgPath;
		static const std::string computeTestCfgPath;

		// Others
		static const std::string resultFilePath;
		static const std::string resultGMMFilePath;
		static const std::string worldFilePath;
		static const std::string timeDataFile;

		// Functions
		static std::string basename(std::string original);
		static void writeTime(std::string what, double time);
		static int setUpParamFile(std::string sphFile, bool isSph);
		static int normalizeParam(std::string paramFile);
		static int createIvector(std::string targetIndex);
		static int generateGMMModel(std::string targetIndex);
		static int test(std::string testIndex);
		static int testGMM(std::string testIndex);
		static std::string analyseResults(std::string resultFile, std::string tested);
		static void generateIndex(std::string indexPath, std::string name, std::vector<std::string> params);
		static void generateTestIndex(std::string modelsIndexPath, std::string recordsIndexPath, std::string testIndexPath);
		static void addModel(std::string modelsFile, std::string name, std::string vectorName);

	public:

		static void registerModel(std::string name, std::vector<std::string> audioFilesPaths);
		static void registerModelGMM(std::string name, std::vector<std::string> audioFilesPaths);
		static std::string identify(std::vector<std::string> audioFilesPaths);
		static std::string identifyGMM(std::vector<std::string> audioFilesPaths);
		static std::string unUnderscore(std::string original);
		static std::string underscore(std::string original);
};
