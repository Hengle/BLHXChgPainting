#include <Windows.h>
#include <cstdio>

#pragma comment(lib, "AssetsTools.lib")

#include "AssetsTools\AssetsFileReader.h"
#include "AssetsTools\AssetsBundleFileFormat.h"
#include "AssetsTools\AssetBundleFileTable.h"

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
) {
	return TRUE;
}
// D:\work\aaa\xiaotiane.png

struct file {
	std::auto_ptr<char> buf;
	size_t length;
	int last;
	bool open(const char* fn) {
		FILE* fp;
		fopen_s(&fp, fn, "rb");
		if (!fp) {
			return false;
		}
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		buf.reset(new char[length]);
		fseek(fp, 0, SEEK_SET);
		fread(buf.get(), 1, length, fp);
		fclose(fp);
		return true;
	}
	operator LPARAM() const {
		return LPARAM(this);
	}

	QWORD static _cdecl reader(QWORD pos, QWORD count, void *pBuf, LPARAM par) {
		file* fp = (file*)par;
		int _pos = int(pos);
		if (_pos < 0) {
			_pos = fp->last;
		}
		count = (_pos + count) > fp->length ? fp->length - _pos : count;
		memcpy(pBuf, fp->buf.get() + _pos, count);
		fp->last = _pos + count;
 		return count;
	}
};

QWORD _cdecl writer(QWORD pos, QWORD count, const void *pBuf, LPARAM par) {
	FILE* fp = (FILE*)par;
	fseek(fp, pos, SEEK_SET);
	return fwrite(pBuf, 1, count, fp);
}

void _cdecl verifyLog(char *message) {
	OutputDebugStringA(message);
}

bool GetImageInfo(const char* fn, int& bufSize) {
	printf("enter GetImageInfo\n");
	AssetsBundleFile bf;
	AssetBundleAsset asset;
	FILE *fo;
	fn = "D:\\from_nox\\Nox_share\\Other\\files\\AssetBundles\\painting\\aikesaite_enc_tex";
	file infile;
	infile.open(fn);
	fopen_s(&fo, "temp", "wb");
	auto ret = bf.Read(file::reader, infile, verifyLog, true);
	AssetsFileReader reader;
	AssetsBundleDirectoryInfo06 dirInfo;
	if (bf.bundleHeader6.fileVersion == 6) {
		LPARAM p;
		reader = bf.MakeAssetsFileReader(file::reader, &p, &dirInfo);
		if (bf.IsAssetsFile(reader, p, &dirInfo)) {
			asset.ReadBundleFile();
		}
	}
	//auto ret = bf.Unpack(file::reader, infile, writer, LPARAM(fo));
	//bufSize = 1025051;
	return true;
}

bool LoadImageFromBundle(const char* fn, void* buf) {
	FILE* fp;
	fopen_s(&fp, "D:\\work\\aaa\\xiaotiane.png", "rb");
	if (fp) {
		fread(buf, 1, 1025051, fp);
		fclose(fp);
		return true;
	}
	return false;
}

bool ReplaceImageFile(const char* fn, const char* png) {
	return false;
}
