#include "iostream"
#include "cstdio"
#include "vector"
#include "curl/curl.h"
#include "cstdlib"
#include "cstring"
#include "map"
#include "unistd.h"

#define DEBUG//RELEASE
#define LOGIN_URL "https://netaccess.iitm.ac.in/account/login"
#define LOGIN_SUCCESS_URL "https://netaccess.iitm.ac.in/account/index"
#define APPROVE_URL "https://netaccess.iitm.ac.in/account/approve"
#define LOGOUT_URL "https://netaccess.iitm.ac.in/account/approve"

using namespace std;
typedef std::map<std::string, std::string> UrlParams;

void debugmsg(string msg)
{
	#ifdef DEBUG
	cout<<"> "<<msg<<endl;
	#endif
}

void debugmsg(char* msg)
{
	#ifdef DEBUG
	cout<<"> "<<msg<<endl;
	#endif
}

void errorcheck(CURLcode result)
{
	#ifdef DEBUG
	if (result != CURLE_OK)
	{
		cerr<<"> "<<curl_easy_strerror(result)<<endl;
		exit(-1);
	}
	#endif
}

void readinput(string output, string& store)
{
	cout<<output;
	std::getline(std::cin, store);;
}

void readpassword(string output, string& store)
{
	char* password;
	password = getpass(output.c_str());
	store = string(password);
}

string params_string(UrlParams const &params)
{
	//http://stackoverflow.com/questions/19978947/libcurl-c-adding-http-get-parameters
	if (params.empty()) 
		return "";
	UrlParams::const_iterator pb = params.cbegin(), pe= params.cend();
	std::string data= pb-> first+ "="+ pb-> second;
	++pb;
	if(pb == pe) 
		return data;
	for(; pb!= pe; ++ pb)
		data+= "&"+ pb-> first+ "="+ pb-> second;
	return data;
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

int main()
{
	CURL* curl;
	CURLcode result;
	string rollno, password, loginbody, approvebody;
	
	readinput("Roll No. : ", rollno);
	readpassword("Password : ", password);

	loginbody = params_string({
		{"userLogin", rollno},
		{"userPassword", password},
		{"submit", ""}
	});
	approvebody = params_string({
		{"duration", "2"},
		{"approveBtn", ""}
	});

	curl = curl_easy_init();
	if (curl)
	{
		//Get initial session state
		curl_easy_setopt(curl, CURLOPT_URL, LOGIN_URL);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "");
		result = curl_easy_perform(curl);
		errorcheck(result);

		//Post login credentials
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, 	loginbody.c_str());
		result = curl_easy_perform(curl);
		errorcheck(result);

		//Check login validity
		char* redirecturl;
		curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redirecturl);
		debugmsg("redirected to " + string(redirecturl));
		string redirectstring(redirecturl);
		if (redirectstring.compare(LOGIN_SUCCESS_URL) != 0)
		{
			cout<<"Login Error"<<endl;
			exit(-1);
		}

		//Post approve data
		curl_easy_setopt(curl, CURLOPT_URL, APPROVE_URL);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, approvebody.c_str());
		debugmsg(string("sending approve request"));
		result = curl_easy_perform(curl);
		errorcheck(result);
		cout<<"Approved for one day"<<endl;
	}

	curl_easy_cleanup(curl);
	return 0;
}
