#include "DllAlgo.h"

DllAlgo::DllAlgo(): algo(nullptr)
{
}

bool DllAlgo::LoadDll(const string& dllPath)
{
	path = string(dllPath);

	// Load dynamic library
	HINSTANCE hDll = LoadLibraryA(path.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll)
	{
		MainLogger.logFile << "Could not load the dynamic library " << path << endl;
		return false;
	}

	GetAlgorithmFuncType GetAlgorithmFunc = (GetAlgorithmFuncType)GetProcAddress(hDll, "GetAlgorithm");
	if (!GetAlgorithmFunc)
	{
		MainLogger.logFile << "could not load function GetAlgorithm()" << std::endl;
		return false;
	}

	handle = { hDll, GetAlgorithmFunc };
	algo = GetAlgorithmFunc();

	if(algo == nullptr)
	{
		MainLogger.logFile << "Object is nullptr after init" << endl;
		return false;
	}
	MainLogger.logFile << "Load dll successfully for " << path << endl;
	return true;
}

