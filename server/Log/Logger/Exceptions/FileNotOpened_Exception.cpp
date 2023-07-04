#include "FileNotOpened_Exception.h"


FileNotOpened_Exception::FileNotOpened_Exception(
  const std::string& fileName): std::exception(),
	errorMessage_("Error: File '" + fileName +
                "' not opened")
{
}



const char* FileNotOpened_Exception::what() const noexcept
{
	return errorMessage_.c_str();
}