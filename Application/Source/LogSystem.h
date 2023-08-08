#pragma once 
#include <string>
#include <fstream>
#include <iostream>
enum class ErrorType{Info,Warning,Error,FatalError};
namespace Core{
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
		std::ofstream file("C:/Repos/Game/bin/windows/x86_64/Application/Log.txt", std::ios::app);

		switch (type) {
			case ErrorType::Info:{
				file << "Info: ";
				std::cout << "Info: ";

				break;
			}
			case ErrorType::Warning : {
				file << "Warning: ";
				std::cout << "Warning: ";

				break;

			}
			case ErrorType::FatalError : {
				file << "FetalError: ";
				std::cout << "FetalError: ";


				break;
			}
		    case ErrorType::Error : {
				file << "Error: ";
				std::cout << "Error: ";

				break;
			}
			default: {
				   break;
			   }

		};

		LogRec(file, args...);
		file.close();
	}


}