#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <Windows.h>
#include <thread>
#include "func.h"
#include "pch.h"

void MoveDirectory(char* src_dir, char* dst_dir);
void FileScan(string base_dir, vector<string>* distBuf);
int MoveSingleFile(string src_file, string dist_file);
void MoveFileList(vector<string>* src_paths, string src_dir, string dist_dir);

extern "C" DLL_API void MoveDirectory(char* src_dir, char* dst_dir)
{
	vector<string>* buf = new vector<string>;
	FileScan(src_dir, buf);
	MoveFileList(buf, src_dir, dst_dir);
	return;
}

void FileScan(string base_dir, vector<string>* distBuf)
{
	intptr_t handle;
	_finddata_t* data = new _finddata_t;
	int layer_file_count = 0;
	handle = _findfirst((base_dir + "\\*.*").c_str(), data);
	if (handle == -1)
	{
		distBuf->push_back(base_dir);
		layer_file_count++;
	}
	else
	{
		layer_file_count++;
		while (_findnext(handle, data) != -1)
		{
			if (strcmp(data->name, ".") == 0 || strcmp(data->name, "..") == 0)
			{
				layer_file_count++;
				continue; //跳过 . 和 ..
			}
			else if (data->attrib == _A_SUBDIR)
			{
				layer_file_count++;
				FileScan(base_dir + "\\" + data->name, distBuf);
			}
			else
			{
				layer_file_count++;
				distBuf->push_back(base_dir + "\\" + data->name);
			}
		}
		if (layer_file_count <= 2)
		{
			distBuf->push_back(base_dir);
		}
		_findclose(handle);
	}
	delete data;
	return;
}

int MoveSingleFile(string src_file, string dist_file)
{
	char tempDirPath[MAX_PATH] = { 0 };
	for (unsigned int i = 0; i < dist_file.size(); i++)
	{
		tempDirPath[i] = dist_file[i];
		if (tempDirPath[i] == '\\' || tempDirPath[i] == '/')
		{
			if (_access(tempDirPath, 0) == -1)
			{
				int ret = _mkdir(tempDirPath);
				if (ret == -1) return ret;
			}
		}
	}
	if (MoveFile(src_file.c_str(), dist_file.c_str()) == 0)
	{
		return _mkdir(tempDirPath); //空文件夹仍然创建
	}
	return 1;
}

void MoveFileList(vector<string>* src_paths, string src_dir, string dist_dir)
{
	for (int i = 0; i < src_paths->size(); i++)
	{
		string temp = (*src_paths)[i];
		MoveSingleFile(temp, replace_all_distinct((*src_paths)[i], src_dir, dist_dir));
	}
	return;
}
