#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::ComponentModel;
using namespace Microsoft::Win32::SafeHandles;

namespace CppCliDll {

	//入出力データを入れる.NETのクラス
	public ref class InData
	{
	public:
		int Data1;
	};

	public ref class OutData
	{
	public:
		String^ Data;
	};



	public ref class Volume
	{
	public:
		OutData^ VolumeCommand(InData^ inputData);

	private:
		void DeviceIoControl(SafeHandle^ hDevice, UInt32 iOControlCode, IntPtr pInBuffer, UInt32 inBufferSize, IntPtr pOutBuffer, UInt32 outBufferSize, UInt32 % bytesReturned, [Out] bool% isExistMoreData);
	};
}
