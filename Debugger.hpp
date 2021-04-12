#pragma once
#include "include_resources.hpp"

class Message
{
public:
	typedef enum
	{
		verbose,
		request,
		reqread,
		warning,
		error,
		fatal,
		sys,
		reset
	} LogLevel;
	static	const	std::string	resetColor;
	static	const	std::string	coutStyleColor;
	Message() {};
	Message(const char *s, LogLevel lvl = verbose);
	Message(const std::string &s, LogLevel lvl = verbose);
	void			setLvl(LogLevel lvl);
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
			_msgPrefix = "#[Delim]#\t";
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
	const std::string		&getMsgColor() const;
	const std::string		&getMsg() const;
	const std::string		&getMsgPrefix() const;
	LogLevel				getMsgLvl() const;
	~Message() {};
	protected:
	std::string		_msg;
	LogLevel		_msgLvl;
	std::string		_msgColor;
	std::string		_msgPrefix;
};

class Logger
{
	typedef Message::LogLevel	LogLevel;
private:
	int						_isFile;
	LogLevel				_lvl;
	// LogLevel				_lastMsgLvl;
	std::ostream			*_stream;
	time_t					_startLogTime;
	Message					_msg;
	class LoggerException : public std::exception
	{
		public:
			virtual const char	*what() const throw() {return ("LoggerError");};
	};
public:
	static const std::string 	endl;
	Logger();
	Logger(std::ostream &file, LogLevel lvl = Message::verbose);
	Logger(std::string &fileName, LogLevel lvl = Message::verbose);
	~Logger();
	void			setLogLevel(LogLevel newLvl);
	Logger	&operator<<(const Message &o);
	Logger	&operator<<(const LogLevel &lvl);
	Logger	&operator<<(const char *s);
	template <class T>
	Logger &operator<<(T o)
	{
		*_stream << o;
		return (*this);
	}
};

void				printLog(Logger *_webLogger, std::string msg, int addInfo, Message::LogLevel lvl);
