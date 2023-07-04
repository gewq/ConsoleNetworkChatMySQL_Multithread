#include "FileIsEmpty_Exception.h"


FileIsEmpty_Exception::FileIsEmpty_Exception(
  const std::string& fileName): std::exception(),
	errorMessage_("Error: File '" + fileName +
                "' is empty")
{
}



const char* FileIsEmpty_Exception::what() const noexcept
{
	return errorMessage_.c_str();
}