#include "ReqHeadAnalyser.hpp"
#include "include_resources.hpp"

int main()
{
	std::cout << "\t\t\tHost validator test: \n";
	std::vector<std::string> hosts = 
	{
		"wewe.wew.w",
		"127.0.0.1",
		"www.lox-net.com",
		"www.lox-net.net-flix.f.us",
		"wewe",
		"wewe.",
		"w.e.w..e",
		"",
		"w ewe.wew.w",
		"wewe.wew.w!",
		"-wewe.wew.w",
		"wewe-.-wew.w",
	};
	for (int i = 0; i < hosts.size(); i++)
	{
		bool res = HeaderAnalyser::isValidHost(hosts[i]);
		std::cout << "\033[35mRes: " << res << "\033[0m, host : " << hosts[i] << "\n";
	}
	
	std::cout << "\n\n\t\t\tCharset validator test: \n";
	std::vector<std::string> chets = 
	{
		"*",
		"utf-8;q=0.01",
		"utf-8;q=0.001",
		"*, *, *;q=0.22",
		"iso-8859-1",
		"utf-8, iso-8859-1;q=0.5",
		"utf-8, iso-8859-1;q=0.5, *;q=0.1",
		"*;q=0.22, *;q=0.2221",
		"iso-8859-, *;q=0.22",
		"iso-88, *;q=0.22, 59-",
		"iso-8859-;q=1",
		"",
	};
	for (int i = 0; i < chets.size(); i++)
	{
		qualityMap	ch = HeaderAnalyser::parseAcceptionLine(chets[i]);
		std::cout << "\033[32m |valid ? \033[34m " << ch.first << "\033[0m| str: " << chets[i] << std::endl;
		for (std::map<std::string, int>::iterator it = ch.second.begin(); it != ch.second.end(); it++)
			std::cout << "\t\tCharset: " << it->first << ", Q: " << it->second << std::endl;
	}

	std::cout << "\n\n\t\t\tUserAgent validator test: \n";
	std::vector<std::string> users = 
	{
		"Opera/9.80 (Android 2.3.3; Linux; Opera Mobi/ADR-1111101157; U; es-ES) Presto/2.9.201 Version/11.50",
		"Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS 7.5; Trident/5.0; IEMobile/9.0)",
		"Mozilla/5.0 (Windows Phone 10.0; Android 6.0.1; Xbox; Xbox One) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Mobile Safari/537.36 Edge/16.16299",
		"Mozilla/5.0 (Linux; Android 4.4.2); Nexus 5 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/33.0.1750.117 Mobile Safari/537.36 OPR/20.0.1396.72047",
		"Mozilla/5.0 (Android; Mobile; rv:13.0) Gecko/13.0 Firefox/13.0",
		"Mozilla/5.0 (Linux; U; Android 4.0.3; de-ch; HTC Sensation Build/IML74K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30",
		"asdasd/555",
		"",
		"as/w -s",
		"/",
		"/s/s/s",
	};
	for (int i = 0; i < users.size(); i++)
	{
		bool res = HeaderAnalyser::isValidUserAgent(users[i]);
		std::cout << "\033[35mRes: " << res << "\033[0m\n";
	}
	

	std::cout << "\n\n\t\t\tBase64Encoder validator test: \n";
	std::vector<std::string>	msgs = 
	{
		"Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.",
		"A",
		"AB",
		"ABC",
	};
	std::string s = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";
	for (int i = 0; i < msgs.size(); i++)
	{
		std::string mssg = HeaderAnalyser::base64encode(msgs[i]);
		std::cout << mssg  << std::endl;
	}
	
	std::cout << "\n\n\t\t\tBase64Decoder validator test: \n";
	std::vector<std::string>	mmsgs = 
	{
		"TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=",
		"QQ==",
		"QUI=",
		"QUJD",
	};
	for (int i = 0; i < mmsgs.size(); i++)
	{
		std::string msssg = HeaderAnalyser::base64decode(mmsgs[i]);
		std::cout << msssg  << std::endl;
	}

}
