#pragma once 
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>

enum class ErrorType{Info,Warning,Error,FatalError};
namespace Core{
	const std::string COUTRED{"\033[31m"};
	const std::string COUTGREEN{"\033[32m"};
	const std::string COUTYELLOW{"\033[33m"};
	const std::string COUTWHITE{"\033[0m"};

	static std::mutex LogMutex{};

    void EmptyLogFile();
    
	template<typename T>
	 void LogRec(std::ofstream& file, T t) {
		file << t <<"\n";
		std::cout << t << "\n";


	}
	template<typename T, typename... ARGS>
	  void LogRec(std::ofstream& file, T first, ARGS...args) {

		file << first << " ";
		std::cout << first << " ";

		LogRec(file,args...);
	}
	template<typename... ARGS>
	 void Log(ErrorType type, ARGS ...args) {
		std::lock_guard<std::mutex> lock(LogMutex);

		std::ofstream file("C:/Repos/Game/bin/windows/x86_64/Application/Log.txt", std::ios::app);
		switch (type) {
			case ErrorType::Info:{
				file << "Info: ";
				std::cout <<COUTGREEN<< "Info: ";

				break;
			}
			case ErrorType::Warning : {
				file << "Warning: ";
				std::cout <<COUTYELLOW<< "Warning: ";

				break;

			}
			case ErrorType::FatalError : {
				file << "FetalError: ";
				std::cout << COUTRED<<"FetalError: ";


				break;
			}
		    case ErrorType::Error : {
				file << "Error: ";
				std::cout <<COUTRED<< "Error: ";
				break;
			}
			default: {
				   break;
			   }

		};

		LogRec(file, args...);
		file.close();
		std::cout <<COUTWHITE;

	}
	template<typename... ARGS>
	 void Log(ARGS ...args) {
		Log(ErrorType::Info,args...);
	 }
	


}
#ifdef DEBUG
#define PRINTDEBUG(x,...) Core::Log(x,__VA_ARGS__)
#else
#define PRINTDEBUG(x,...)
#endif 