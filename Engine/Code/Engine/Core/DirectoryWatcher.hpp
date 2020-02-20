#pragma once

class DirectoryWatcher
{
public:
	DirectoryWatcher();
	~DirectoryWatcher();

	void Initialize();
	void Terminate();
	//void AddDirectoryToWatchList( LPCTSTR directory , BOOL watchSubtree , DWORD notificationFilters , DWORD bufferSize = 16384 );

private:

};
