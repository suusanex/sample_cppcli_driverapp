#include "stdafx.h"

#include "CppCliDll.h"

//デバイスドライバとのやり取りに使用する、構造体等の定義

struct VOLUME_IOCTL_RESULT
{
	int DataStringOffset;
};

struct IOCTL_EXECUTE
{
	int Size;
	int Data1;
};

const int IOCTL_RESULT_MAXSIZE = 65535;

#define IOCTL_VOLUMECOMMAND 		CTL_CODE(FILE_DEVICE_UNKNOWN, 0xF0A, METHOD_BUFFERED, FILE_ANY_ACCESS)


using namespace CppCliDll;


void Volume::DeviceIoControl(SafeHandle^ hDevice, UInt32 iOControlCode, IntPtr pInBuffer, UInt32 inBufferSize, IntPtr pOutBuffer, UInt32 outBufferSize, UInt32 % bytesReturned, [Out] bool% isExistMoreData)
{
	//Win32APIのDeviceIoControlを呼び出し。エラー時は.NETの例外を投げる。
	isExistMoreData = false;

	DWORD bytesReturnedBuf;
	auto bRet = ::DeviceIoControl(hDevice->DangerousGetHandle().ToPointer(), iOControlCode, pInBuffer.ToPointer(), inBufferSize, pOutBuffer.ToPointer(), outBufferSize, &bytesReturnedBuf, NULL);
	bytesReturned = bytesReturnedBuf;

	
	//動作を見せるため、ここでメッセージボックスを表示
	MessageBoxW(NULL, L"DeviceIoControlを送信しました（C++/CLIのDLLからメッセージを出しています）", L"成功", MB_OK);


	if (!bRet) {
		auto Win32Err = GetLastError();
		if (Win32Err = ERROR_MORE_DATA) {
			isExistMoreData = true;
		}
		else {
			throw gcnew Win32Exception(String::Format("DeviceIoControl Fail, Win32Err=0x{0:X}, iOControlCode={1:d}, hDevice=0x{2:X}", GetLastError(), iOControlCode, hDevice->DangerousGetHandle()));
		}
	}

}


OutData ^ Volume::VolumeCommand(InData ^ inputData)
{
	//入力バッファ作成
	std::vector<BYTE> inBuf(sizeof(IOCTL_EXECUTE));

	//.NETのクラスで与えられた入力パラメータを、構造体にセット
	auto pCommonHead = reinterpret_cast<IOCTL_EXECUTE*>(inBuf.data());
	pCommonHead->Size = inBuf.size();
	pCommonHead->Data1 = inputData->Data1;

	//出力バッファ作成
	std::vector<BYTE> outBuf(IOCTL_RESULT_MAXSIZE);

	{
		//対象のデバイスドライバにあたるハンドルを開く
		CStringW OpenName(L"\\\\.\\C:");
		auto hDriverN = CreateFileW(OpenName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hDriverN == INVALID_HANDLE_VALUE) {
			auto Win32Err = GetLastError();
			throw gcnew Exception(String::Format("Driver Open Fail, Win32Err=0x{0:X}", Win32Err));
		}

		SafeHandle^ hDriver = gcnew SafeFileHandle(IntPtr(hDriverN), true);

		//DeviceIoControlを送信
		UInt32 bytesReturned = 0;
		bool isExistMoreData = false;
		DeviceIoControl(hDriver, IOCTL_VOLUMECOMMAND, IntPtr(inBuf.data()), inBuf.size(), IntPtr(outBuf.data()), outBuf.size(), bytesReturned, isExistMoreData);

	}


	//出力バッファの構造体を読み取り
	auto pOutCommonHead = reinterpret_cast<VOLUME_IOCTL_RESULT*>(outBuf.data());

	//.NETのクラスを作成して、構造体の内容を設定。戻り値へ返す。
	//C++なのでC#とは違い、オフセットを使ったポインタでの読み取りなども簡単
	auto pDataString = reinterpret_cast<const wchar_t*>(outBuf.data() + pOutCommonHead->DataStringOffset);

	auto outData = gcnew OutData();
	outData->Data = gcnew String(pDataString);


	return outData;

}
