// AudioConvertisseurM4A.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>
#include <windowsx.h>
#include <wingdi.h>
#include <Winuser.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <shlobj.h>

#pragma comment(lib,"comctl32.lib")
#pragma comment (lib,"shlwapi.lib")

char buffer[1024];
char sortie[256];
char buffer1[MAX_PATH];
char Source[260];
BROWSEINFO bi;
ITEMIDLIST *il;
WIN32_FIND_DATA fd;

int scanner(char * source);
int ExtrapolerNomCible(char* source);
int SelectionDossier();
bool EnumerateFiles(LPTSTR lptszSearchPath);
int ParsingCMD2FFMepg(char* source, char* cible);

int main(int argc,char* argv[])
{
	SetConsoleTitle("Convertisseur Audio M4A");
	std::cout << argv[0] << "\n";
	if (argc > 1) {
		std::cout << argv[1] << "\n";
		}
	else {
		std::cout << "Aucune entree specifiee\nPassage au mode Assistant\n";
		SelectionDossier();
	}
}
int scanner(char * source) {
	SetCurrentDirectory(source);
	BOOL fFinished = FALSE;
	int x = 0;
	char tmp[200];
	HANDLE hSearch = FindFirstFile("*.mp3", &fd);
	if (hSearch == INVALID_HANDLE_VALUE) {
		printf("Analyse terminée de\n%s", source);
		return x;
	}
	while (!fFinished) {
		x++;
		if (GetFileAttributes(fd.cFileName) != FILE_ATTRIBUTE_DIRECTORY) {
			PathAddBackslash(source);
			sprintf_s(buffer, "%s%s\n%s", source, fd.cFileName, buffer);
		}
		if (x > 1) {
			sprintf_s(buffer1, "%d fichiers", x);
		}
		else {
			sprintf_s(buffer1, "%d fichier", x);
		}
		if (!FindNextFile(hSearch, &fd)) {
			if (GetLastError() == ERROR_NO_MORE_FILES) {
				printf("AudioConvertisseurM4A C++ a trouvé %s dans le dossier %s et les a ajoutés", buffer1, source);
				fFinished = TRUE;
			}
			else {
				printf("Impossible de trouver le fichier suivant");
				return -1;
			}
		}
	}
	return FindClose(hSearch);
}
int ExtrapolerNomCible(char* source)
{
	wsprintf(sortie, "%s",source);
	PathRenameExtension(sortie, ".mp3");
	ParsingCMD2FFMepg(source, sortie);
	return 0;
}
int SelectionDossier()
{
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T("Dossier à traiter");
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0) {
		if (SHGetPathFromIDList(pidl, Source)) {
			printf("Traitement de %s",Source);
		}
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))	{
			imalloc->Free(pidl);
			imalloc->Release();
		}
	}	
	return EnumerateFiles(Source);
}
bool EnumerateFiles( LPTSTR lptszSearchPath){
WIN32_FIND_DATA	FindData;
	HANDLE		hFiles;
	UINT		nFileCount = 0;
	BOOL		bRet = FALSE;
	SetCurrentDirectory(lptszSearchPath);
	if((hFiles = FindFirstFile("*.*", &FindData)) != NULL){
		for(; FindNextFile(hFiles, &FindData); nFileCount++)
			ExtrapolerNomCible(FindData.cFileName);
		bRet = TRUE;
	}
	return bRet;
}
int ParsingCMD2FFMepg(char* source, char* cible)
{
	wsprintf(buffer,"Convertisseur Audio MP3 [%s]",sortie);
	SetConsoleTitle(buffer);
	char path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYMUSIC, NULL, 0, path)))
	{
		wsprintf(buffer, "ffmpeg -i \"%s\\%s\" -c:a libmp3lame -b:a 128k -ar 44100 -ac 2 -map_metadata 0 \"%s\\o_%s\"", Source, source, path, cible);
	}
	else
	{
		wsprintf(buffer, "ffmpeg -i \"%s\\%s\" -c:a libmp3lame -b:a 128k -ar 44100 -ac 2 -map_metadata 0 \"%s\\o_%s\"", Source,source, Source,cible);
	}
	system(buffer);
	return DeleteFile(source);
}