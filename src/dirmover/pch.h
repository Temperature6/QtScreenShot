#pragma once
#define  DLL_API __declspec(dllexport)
extern "C"
{
	DLL_API void MoveDirectory(char* src_dir, char* dst_dir);
}
