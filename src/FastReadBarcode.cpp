// BarcodeReaderDemo.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <conio.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "DynamsoftBarcodeReader.h"
using namespace std;

bool GetExeDirectory(string& strPath)
{
	char path[512] = { 0 };
#if defined(_WIN32) || defined(_WIN64)
	GetModuleFileName(NULL, path, sizeof(path));
	strPath = path;

	size_t pos = strPath.find_last_of('\\');
	strPath = strPath.substr(0, pos);
	return true;
#elif defined(__linux__)
	readlink("/proc/self/exe", path, sizeof(path));
	strPath = path;

	size_t pos = strPath.find_last_of('\\');
	strPath = strPath.substr(0, pos);
	return true;
#elif defined(__APPLE__) && defined(TARGET_OS_MAC)
#endif
	return false;
}

/**
 * Initialize runtimesettings by using a template file
 *
 * @param reader  the instance of CBarcodeReader
 * @return true if an error occurred
 */
bool InitialRuntimeSettingsWithFile(CBarcodeReader& reader)
{
	char errorMessage[256] = { 0 };
	int iRet = DBR_OK;
	string file;
	GetExeDirectory(file);
	size_t pos = file.find_last_of('/');
	file = file.substr(0, pos + 1) + "resource/FastReadBarcode.json";

	iRet = reader.InitRuntimeSettingsWithFile(file.c_str(), ConflictMode::CM_OVERWRITE, errorMessage, 256);
	if (iRet != DBR_OK)
		return true;
	return false;
}

void ToHexString(unsigned char* pSrc, int iLen, char* pDest)
{
	const char HEXCHARS[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	int i;
	char* ptr = pDest;

	for (i = 0; i < iLen; ++i)
	{
#if defined(_WIN32) || defined(_WIN64)
		sprintf_s(ptr, 4, "%c%c ", HEXCHARS[(pSrc[i] & 0xF0) >> 4], HEXCHARS[(pSrc[i] & 0x0F) >> 0]);
#else
		snprintf(ptr, 4, "%c%c ", HEXCHARS[(pSrc[i] & 0xF0) >> 4], HEXCHARS[(pSrc[i] & 0x0F) >> 0]);
#endif
		ptr += 3;
	}
}

bool GetImagePath(char* pImagePath)
{
	char pszBuffer[512] = { 0 };
	bool bExit = false;
	size_t iLen = 0;
	FILE* fp = NULL;
	while (1)
	{
		printf("\r\nStep 1: Input your QR code image file's full path:\r\n>> ");
#if defined(_WIN32) || defined(_WIN64)
		gets_s(pszBuffer, 512);
#else
		fgets(pszBuffer, 512, stdin);
		strtok(pszBuffer, "\n");
#endif
		iLen = strlen(pszBuffer);
		if (iLen > 0)
		{
			if (strlen(pszBuffer) == 1 && (pszBuffer[0] == 'q' || pszBuffer[0] == 'Q'))
			{
				bExit = true;
				break;
			}

			memset(pImagePath, 0, 512);
			if ((pszBuffer[0] == '\"' && pszBuffer[iLen - 2] == '\"') || (pszBuffer[0] == '\'' && pszBuffer[iLen - 2] == '\''))
				memcpy(pImagePath, &pszBuffer[1], iLen - 3);
			else
				memcpy(pImagePath, pszBuffer, iLen);

#if defined(_WIN32) || defined(_WIN64)
			int err = fopen_s(&fp, pImagePath, "rb");
			if (err == 0)
			{
				fclose(fp);
				break;
			}
#else
			fp = fopen(pImagePath, "rb");
			if (fp != NULL)
			{
				break;
			}
#endif
		}
		printf("Please input a valid path.\r\n");
	}
	return bExit;
}

void OutputResult(CBarcodeReader& reader, int errorcode, float time)
{
	char* pszTemp = NULL;
	char* pszTemp1 = NULL;
	char* pszTemp2 = NULL;
	int iRet = errorcode;
	pszTemp = (char*)malloc(4096);
	if (iRet != DBR_OK && iRet != DBRERR_MAXICODE_LICENSE_INVALID && iRet != DBRERR_AZTEC_LICENSE_INVALID && iRet != DBRERR_LICENSE_EXPIRED && iRet != DBRERR_QR_LICENSE_INVALID && iRet != DBRERR_GS1_COMPOSITE_LICENSE_INVALID &&
		iRet != DBRERR_1D_LICENSE_INVALID && iRet != DBRERR_PDF417_LICENSE_INVALID && iRet != DBRERR_DATAMATRIX_LICENSE_INVALID && iRet != DBRERR_GS1_DATABAR_LICENSE_INVALID && iRet != DBRERR_PATCHCODE_LICENSE_INVALID &&
		iRet != DBRERR_POSTALCODE_LICENSE_INVALID && iRet != DBRERR_DOTCODE_LICENSE_INVALID && iRet != DBRERR_DPM_LICENSE_INVALID && iRet != DBRERR_IRT_LICENSE_INVALID)
	{
#if defined(_WIN32) || defined(_WIN64)
		sprintf_s(pszTemp, 4096, "Failed to read barcode: %s\r\n", DBR_GetErrorString(iRet));
#else
		snprintf(pszTemp, 4096, "Failed to read barcode: %s\r\n", DBR_GetErrorString(iRet));
#endif
		printf(pszTemp);
		free(pszTemp);
		return;
	}

	TextResultArray* paryResult = NULL;
	reader.GetAllTextResults(&paryResult);

	if (paryResult->resultsCount == 0)
	{
#if defined(_WIN32) || defined(_WIN64)
		sprintf_s(pszTemp, 4096, "No barcode found. Total time spent: %.3f seconds.\r\n", time);
#else
		snprintf(pszTemp, 4096, "No barcode found. Total time spent: %.3f seconds.\r\n", time);
#endif
		printf(pszTemp);
		free(pszTemp);
		CBarcodeReader::FreeTextResults(&paryResult);
		return;
	}

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(pszTemp, 4096, "Total barcode(s) found: %d. Total time spent: %.3f seconds\r\n\r\n", paryResult->resultsCount, time);
#else
	snprintf(pszTemp, 4096, "Total barcode(s) found: %d. Total time spent: %.3f seconds\r\n\r\n", paryResult->resultsCount, time);
#endif	
	printf(pszTemp);
	for (int iIndex = 0; iIndex < paryResult->resultsCount; iIndex++)
	{
#if defined(_WIN32) || defined(_WIN64)
		sprintf_s(pszTemp, 4096, "Barcode %d:\r\n", iIndex + 1);
#else
		snprintf(pszTemp, 4096, "Barcode %d:\r\n", iIndex + 1);
#endif
		printf(pszTemp);
		if (paryResult->results[iIndex]->barcodeFormat != 0)
		{
#if defined(_WIN32) || defined(_WIN64)
			sprintf_s(pszTemp, 4096, "    Type: %s\r\n", paryResult->results[iIndex]->barcodeFormatString);
#else
			snprintf(pszTemp, 4096, "    Type: %s\r\n", paryResult->results[iIndex]->barcodeFormatString);
#endif	
		}
		else
		{
#if defined(_WIN32) || defined(_WIN64)
			sprintf_s(pszTemp, 4096, "    Type: %s\r\n", paryResult->results[iIndex]->barcodeFormatString_2);
#else
			snprintf(pszTemp, 4096, "    Type: %s\r\n", paryResult->results[iIndex]->barcodeFormatString_2);
#endif
		}

		printf(pszTemp);
#if defined(_WIN32) || defined(_WIN64)
		sprintf_s(pszTemp, 4096, "    Value: %s\r\n", paryResult->results[iIndex]->barcodeText);
#else
		snprintf(pszTemp, 4096, "    Value: %s\r\n", paryResult->results[iIndex]->barcodeText);
#endif
		printf(pszTemp);

		pszTemp1 = (char*)malloc(paryResult->results[iIndex]->barcodeBytesLength * 3 + 1);
		pszTemp2 = (char*)malloc(paryResult->results[iIndex]->barcodeBytesLength * 3 + 100);
		ToHexString(paryResult->results[iIndex]->barcodeBytes, paryResult->results[iIndex]->barcodeBytesLength, pszTemp1);
#if defined(_WIN32) || defined(_WIN64)
		sprintf_s(pszTemp2, paryResult->results[iIndex]->barcodeBytesLength * 3 + 100, "    Hex Data: %s\r\n", pszTemp1);
#else
		snprintf(pszTemp2, paryResult->results[iIndex]->barcodeBytesLength * 3 + 100, "    Hex Data: %s\r\n", pszTemp1);
#endif
		printf(pszTemp2);
		free(pszTemp1);
		free(pszTemp2);
	}

	free(pszTemp);
	CBarcodeReader::FreeTextResults(&paryResult);
}

int main(int argc, const char* argv[])
{
#if defined(_WIN32) || defined(_WIN64)
	unsigned _int64 ullTimeBegin = 0;
	unsigned _int64 ullTimeEnd = 0;
#else
	struct timeval ullTimeBegin, ullTimeEnd;
#endif

	printf("*************************************************\r\n");
	printf("Welcome to Dynamsoft Barcode Reader Demo\r\n");
	printf("*************************************************\r\n");
	printf("Hints: Please input 'Q' or 'q' to quit the application.\r\n");

	CBarcodeReader reader;
	reader.InitLicense("Put your license key here");

	int iRet = -1;
	bool bExit = false;
	char pszImageFile[512] = { 0 };
	const char* pszTemplateName = NULL;

	while (1)
	{
		bExit = GetImagePath(pszImageFile);
		if (bExit)
			break;
		
		//Initialize runtimesettings by using a template file
		bExit = InitialRuntimeSettingsWithFile(reader);
		if (bExit)
			break;

#if defined(_WIN32) || defined(_WIN64)
		ullTimeBegin = GetTickCount();
		iRet = reader.DecodeFile(pszImageFile, pszTemplateName);
		ullTimeEnd = GetTickCount();
		OutputResult(reader, iRet, (((float)(ullTimeEnd - ullTimeBegin)) / 1000));
#else
		gettimeofday(&ullTimeBegin, NULL);
		iRet = reader.DecodeFile(pszImageFile, pszTemplateName);
		float fCostTime = (float)((ullTimeEnd.tv_sec * 1000 * 1000 + ullTimeEnd.tv_usec) - (ullTimeBegin.tv_sec * 1000 * 1000 + ullTimeBegin.tv_usec)) / (1000 * 1000);
		OutputResult(reader, iRet, fCostTime);
#endif

	}

	return 0;
}
