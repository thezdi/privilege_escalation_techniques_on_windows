#pragma once
#include <windows.h>
#include <filesystem>

class Driver {
public:
	bool install_driver();
	void uninstall_driver();
private:
	bool create_driver_file();
	void delete_driver_file();
	void cleanup();
	static std::filesystem::path driver_path;
	static byte RTCore64[];
	static unsigned int RTCore64_Size;
	//static unsigned char RTCore32[];
};