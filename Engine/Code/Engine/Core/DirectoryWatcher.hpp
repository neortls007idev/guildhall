#pragma once

class DirectoryWatcher
{
public:
	DirectoryWatcher();
	~DirectoryWatcher();

	void Initialize();
	void Terminate();
	//void AddDirectory( LPCTSTR wszDirectory , BOOL bWatchSubtree , DWORD dwNotifyFilter , DWORD dwBufferSize = 16384 );


private:

};
