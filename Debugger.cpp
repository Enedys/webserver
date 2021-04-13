#include "Debugger.hpp"

/* Message Class */

const std::string	Message::resetColor = "\033[0m";
const std::string	Message::coutStyleColor = "\033[36m";
Message::Message(const char *s, LogLevel lvl) {setLvl(lvl); setMsg(s);}
Message::Message(const std::string &s, LogLevel lvl) {setLvl(lvl); setMsg(s);}
void					Message::setLvl(LogLevel lvl){_msgLvl = lvl;}
const std::string		&Message::getMsgColor() const {return (_msgColor);}
const std::string		&Message::getMsg() const {return (_msg);}
const std::string		&Message::getMsgPrefix() const {return (_msgPrefix);}
Message::LogLevel		Message::getMsgLvl() const {return (_msgLvl);}

/* Logger Class */

const std::string	Logger::endl = "\n";
Logger::Logger() : _isFile(0), _lvl(Message::verbose), _stream(&std::cerr)
{_startLogTime = time(NULL);}

Logger::Logger(std::ostream &file, LogLevel lvl) : _isFile(0), _lvl(lvl), _stream(&file)
{_startLogTime = time(NULL);}
Logger::Logger(std::string &fileName, LogLevel lvl) try :
	_isFile(1), _lvl(lvl)
{
	_stream = new std::ofstream(fileName.c_str(), std::ios::trunc);
	if (!static_cast<std::fstream *>(_stream)->is_open() ||\
		static_cast<std::fstream *>(_stream)->fail())
		throw LoggerException();
	_startLogTime = time(NULL);
}
catch (const std::exception &e)
{
	std::cerr << e.what() << std::endl;
}

Logger::~Logger()
{
	if (_isFile)
		static_cast<std::fstream *>(_stream)->close();
}

void		Logger::setLogLevel(LogLevel newLvl) {_lvl = newLvl; }

Logger		&Logger::operator<<(const Message &o)
{
	if (o.getMsgLvl() >= _lvl)
		*_stream << time(NULL) - _startLogTime << "\t" << o.getMsgColor()\
		<< o.getMsgPrefix() << o.resetColor << o.getMsg();
	return (*this);
}

Logger		&Logger::operator<<(const LogLevel &lvl)
{
	_msg.setLvl(lvl);
	return (*this);
}

Logger		&Logger::operator<<(const char *s)
{
	if (s == Logger::endl)
	{
		*_stream << std::endl;
		return (*this);
	}
	_msg.setMsg(s);
	Logger::operator<<(_msg);
	return (*this);
}
