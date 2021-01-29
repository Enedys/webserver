#pragma once
#include "includes.hpp"

typedef enum
{
	verbose,
	request,
	reqread,
	warning,
	error,
	fatal,
	sys,
	reset,
} LogLevel;

class Message
{
private:
protected:
	std::string		_msg;
	LogLevel		_msgLvl;
	std::string		_msgColor;
	std::string		_msgPrefix;
public:
	static	const	std::string	resetColor;
	static	const	std::string	coutStyleColor;
	Message() {};
	Message(const char *s, LogLevel lvl = verbose) {setLvl(lvl); setMsg(s);};
	Message(const std::string &s, LogLevel lvl = verbose) {setLvl(lvl); setMsg(s);};
	

	void			setLvl(LogLevel lvl)
	{
		_msgLvl = lvl;
	}

	template <class T>
	Message			&setMsg(T s)
	{
		_msg = s;
		switch (_msgLvl)
		{
		case sys:
			_msgColor = "\033[32;1m";
			_msgPrefix = "#[System]#\t";
			break;
		case fatal:
			_msgColor = "\033[31;1m";
			_msgPrefix = "#[Fatal]#\t";
			break;
		case error:
			_msgColor = "\033[35;1m";
			_msgPrefix = "#[Error]#\t";
			break;
		case warning:
			_msgColor = "\033[33;1m";
			_msgPrefix = "#[Warning]#\t";
			break;
		case verbose:
			_msgColor = "\033[32m";
			_msgPrefix = "#[Event]#\t";
			break;
		case request:
			_msgColor = "\033[32m";
			_msgPrefix = "#[Request]#\t";
			break;
		case reqread:
			_msgColor = "\033[32m";
			_msgPrefix = "#[ReadReq]#\t";
			break;
		default:
			_msgPrefix = "";
			_msgColor = "\033[34;1m";
			break;
		}
		return (*this);
	};
	const std::string		&getMsgColor() const {return (_msgColor);};
	const std::string		&getMsg() const {return (_msg);};
	const std::string		&getMsgPrefix() const {return (_msgPrefix);};
	const LogLevel			getMsgLvl() const {return (_msgLvl);};
	~Message() {};
};
const std::string	Message::resetColor = "\033[0m";
const std::string	Message::coutStyleColor = "\033[36m";


class Logger
{
private:
	int						_isFile;
	LogLevel				_lvl;
	LogLevel				_lastMsgLvl;
	std::ostream			*_stream;
	time_t					_startLogTime;
	Message					_msg;
	class LoggerException : std::exception
	{
		private:
			std::string		_error;
			LoggerException();
		public:
			LoggerException(const char *error) : _error(error) {};
			virtual const char	*what() const throw() {return _error.c_str();};
	};
public:
	static const std::string 	endl;
	Logger() : _lvl(verbose), _stream(&std::cerr), _isFile(0)
	{_startLogTime = time(NULL);};
	Logger(std::ostream &file, LogLevel lvl = verbose) : _lvl(lvl), _stream(&file), _isFile(0)
	{_startLogTime = time(NULL);};
	Logger(std::string &fileName, LogLevel lvl = verbose) try :
	_lvl(lvl), _isFile(1)
	{
		_stream = new std::ofstream(fileName.c_str(), std::ios::trunc);
		if (!static_cast<std::fstream *>(_stream)->is_open() ||\
			static_cast<std::fstream *>(_stream)->fail())
			throw LoggerException("Can not create log-file.");
		_startLogTime = time(NULL);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	~Logger()
	{
		if (_isFile)
			static_cast<std::fstream *>(_stream)->close();
	}
	void			setLogLevel(LogLevel newLvl) {_lvl = newLvl;};

	Logger	&operator<<(const Message &o)
	{
		if (o.getMsgLvl() >= _lvl)
			*_stream << time(NULL) - _startLogTime << "\t" << o.getMsgColor()\
			<< o.getMsgPrefix() << o.resetColor << o.getMsg();
		return (*this);
	};
	Logger	&operator<<(const LogLevel &lvl)
	{
		_msg.setLvl(lvl);
		return (*this);
	}

	Logger	&operator<<(const char *s)
	{
		if (s == Logger::endl)
		{
			*_stream << std::endl;
			return (*this);
		}
		_msg.setMsg(s);
		Logger::operator<<(_msg);
		return (*this);
	};

	template <class T>
	Logger &operator<<(T o)
	{
		*_stream << o;
		return (*this);
	}
};
const std::string	Logger::endl = "\n";
