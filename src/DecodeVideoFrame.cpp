#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include "DynamsoftBarcodeReader.h"
using namespace cv;
using namespace std;
using std::cerr; 
using std::endl;
using std::cin;

void ToHexString(unsigned char* pSrc, int iLen, char* pDest)
{
	const char HEXCHARS[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	int i;
	char* ptr = pDest;

	for (i = 0; i < iLen; ++i)
	{
		sprintf_s(ptr, 4, "%c%c ", HEXCHARS[(pSrc[i] & 0xF0) >> 4], HEXCHARS[(pSrc[i] & 0x0F) >> 0]);
		ptr += 3;
	}
}


void textResultcallback(int frameId, TextResultArray *pResults, void * pUser)
{
	char * pszTemp = NULL;
	char * pszTemp1 = NULL;
	char * pszTemp2 = NULL;
	pszTemp = (char*)malloc(4096);

	sprintf_s(pszTemp, 4096, "\r\n\r\nframe = %d,Total barcode(s) found: %d.\n", frameId, pResults->resultsCount);
	printf(pszTemp);
	for (int iIndex = 0; iIndex < pResults->resultsCount; iIndex++)
	{
		sprintf_s(pszTemp, 4096, "Barcode %d:\r\n", iIndex + 1);
		printf(pszTemp);
		if (pResults->results[iIndex]->barcodeFormat != 0)
		{
			sprintf_s(pszTemp, 4096, "    Type: %s\r\n", pResults->results[iIndex]->barcodeFormatString);
		}
		else
		{
			sprintf_s(pszTemp, 4096, "    Type: %s\r\n", pResults->results[iIndex]->barcodeFormatString_2);
		}
		printf(pszTemp);
		sprintf_s(pszTemp, 4096, "    Value: %s\r\n", pResults->results[iIndex]->barcodeText);
		printf(pszTemp);

		pszTemp1 = (char*)malloc(pResults->results[iIndex]->barcodeBytesLength * 3 + 1);
		pszTemp2 = (char*)malloc(pResults->results[iIndex]->barcodeBytesLength*3 + 100);
		ToHexString(pResults->results[iIndex]->barcodeBytes, pResults->results[iIndex]->barcodeBytesLength, pszTemp1);
		sprintf_s(pszTemp2, pResults->results[iIndex]->barcodeBytesLength*3 + 100, "    Hex Data: %s\r\n", pszTemp1);
		printf(pszTemp2);
		free(pszTemp1);
		free(pszTemp2);
	}
	free(pszTemp);

	CBarcodeReader::FreeTextResults(&pResults);
}

void errorcb(int frameId, int errorCode, void * pUser)
{
	char * pszTemp = NULL;
	pszTemp = (char*)malloc(4096);
	printf("frame = %d errorcode = %d\n", frameId, errorCode);
	if (errorCode != DBR_OK && errorCode != DBRERR_LICENSE_EXPIRED && errorCode != DBRERR_QR_LICENSE_INVALID && errorCode != DBRERR_MAXICODE_LICENSE_INVALID && errorCode != DBRERR_GS1_COMPOSITE_LICENSE_INVALID && 
		errorCode != DBRERR_1D_LICENSE_INVALID && errorCode != DBRERR_PDF417_LICENSE_INVALID && errorCode != DBRERR_DATAMATRIX_LICENSE_INVALID && errorCode != DBRERR_AZTEC_LICENSE_INVALID && 
		errorCode != DBRERR_GS1_DATABAR_LICENSE_INVALID && errorCode != DBRERR_POSTALCODE_LICENSE_INVALID && errorCode != DBRERR_DOTCODE_LICENSE_INVALID && errorCode != DBRERR_DPM_LICENSE_INVALID && errorCode != DBRERR_IRT_LICENSE_INVALID)
	{
		sprintf_s(pszTemp, 4096, "Failed to read barcode: %s\r\n", CBarcodeReader::GetErrorString(errorCode));
		printf(pszTemp);
		free(pszTemp);
		return;
	}
	free(pszTemp);
}


int main()
{
	//getchar();
	Mat frame;
	cout << "Opening camera..." << endl;
	VideoCapture capture(0); // open the first camera
	if (!capture.isOpened())
	{
		cerr << "ERROR: Can't initialize camera capture" << endl;
		return 1;
	}
	 
	int iRet = -1;
	CBarcodeReader reader;
	iRet = reader.InitLicense("Put your license key here");
	if (iRet != 0)
	{
		printf("%s\n", CBarcodeReader::GetErrorString(iRet));
		return -1;
	}
	//Best coverage settings
	//iRet = reader.InitRuntimeSettingsWithString("{\"ImageParameter\":{\"Name\":\"BestCoverage\",\"DeblurLevel\":9,\"ExpectedBarcodesCount\":512,\"ScaleDownThreshold\":100000,\"LocalizationModes\":[{\"Mode\":\"LM_CONNECTED_BLOCKS\"},{\"Mode\":\"LM_SCAN_DIRECTLY\"},{\"Mode\":\"LM_STATISTICS\"},{\"Mode\":\"LM_LINES\"},{\"Mode\":\"LM_STATISTICS_MARKS\"}],\"GrayscaleTransformationModes\":[{\"Mode\":\"GTM_ORIGINAL\"},{\"Mode\":\"GTM_INVERTED\"}]}}",CM_OVERWRITE);
	//Best speed settings
	//iRet = reader.InitRuntimeSettingsWithString("{\"ImageParameter\":{\"Name\":\"BestSpeed\",\"DeblurLevel\":3,\"ExpectedBarcodesCount\":512,\"LocalizationModes\":[{\"Mode\":\"LM_SCAN_DIRECTLY\"}],\"TextFilterModes\":[{\"MinImageDimension\":262144,\"Mode\":\"TFM_GENERAL_CONTOUR\"}]}}", CM_OVERWRITE);
	//Balance settings
	iRet = reader.InitRuntimeSettingsWithString("{\"ImageParameter\":{\"Name\":\"Balance\",\"DeblurLevel\":5,\"ExpectedBarcodesCount\":512,\"LocalizationModes\":[{\"Mode\":\"LM_CONNECTED_BLOCKS\"},{\"Mode\":\"LM_STATISTICS\"}]}}", CM_OVERWRITE);	
		
	if (iRet != 0)
	{
		printf("%s\n", CBarcodeReader::GetErrorString(iRet));
		return -1;
	}
	iRet = reader.SetTextResultCallback(textResultcallback,NULL);
	if (iRet != 0)
	{
		printf("%s\n", CBarcodeReader::GetErrorString(iRet));
		return -1;
	}
	iRet = reader.SetErrorCallback(errorcb, NULL);
	if (iRet != 0)
	{
		printf("%s\n", CBarcodeReader::GetErrorString(iRet));
		return -1;
	}
	FrameDecodingParameters parameters;
	iRet = reader.InitFrameDecodingParameters(&parameters);
	if (iRet != 0)
	{
		printf("%s\n", CBarcodeReader::GetErrorString(iRet));
		return -1;
	}

	capture >> frame;
	parameters.width = capture.get(CAP_PROP_FRAME_WIDTH);
	parameters.height = capture.get(CAP_PROP_FRAME_HEIGHT);
	parameters.maxQueueLength = 30;
	parameters.maxResultQueueLength = 30;
	parameters.stride = frame.step.p[0];
	parameters.imagePixelFormat = IPF_RGB_888;
	parameters.region.regionMeasuredByPercentage = 1;
	parameters.region.regionTop = 0;
	parameters.region.regionBottom = 100;
	parameters.region.regionLeft = 0;
	parameters.region.regionRight = 100;
	parameters.threshold =0.01;
	parameters.fps = 0;

	/*int width = capture.get(CAP_PROP_FRAME_WIDTH);
	int height = capture.get(CAP_PROP_FRAME_HEIGHT);
	int format = capture.get(CAP_PROP_FORMAT);
	int t = frame.type();
	int depth = frame.depth();
	int chnnel = frame.channels();*/
	
	iRet = reader.StartFrameDecodingEx(parameters, "");
	//iRet = reader.StartFrameDecoding(30, 30, width, height, frame.step.p[0], IPF_RGB_888, "");
	if (iRet != 0)
	{
		printf("%s\n", CBarcodeReader::GetErrorString(iRet));
		return -1;
	}
	
	for(;;)
	{
		capture >> frame; // read the next frame from camera
		if (frame.empty())
		{
			cerr << "ERROR: Can't grab camera frame." << endl;
			break;
		}
		reader.AppendFrame(frame.data);
		
		imshow("Frame", frame);
		int key = waitKey(1);
		if (key == 27/*ESC*/)
			break;
	}
	reader.StopFrameDecoding();
	
	return 0;
}
