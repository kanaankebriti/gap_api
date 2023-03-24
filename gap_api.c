#define _CRT_SECURE_NO_WARNINGS

#include "gap_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
//#include <cjson/cJSON.h>

/* linker hinters */
#ifdef _DEBUG
#pragma comment(lib, "libcurl-d.lib")
#else
#pragma comment(lib, "libcurl.lib")
#endif

/* global constants */
static const char baseURL[] = "https://api.gap.im/";
static const char* method[] = { "sendMessage","upload","sendAction","editMessage","deleteMessage","answerCallback" };
static const char* type[] = { "text","radio","select","textarea","checkbox","submit" };
const char* chat_id;
const char* token;

// http_query structure
struct query
{
	char chat_id[21];	// constant length. e.g. "chat_id=%2B9123456789"
	char* data;			// e.g. "data=message"
	// TODO: variable length with malloc should be considered.
	char type[9 + 5];	// from global type array. maximum length considered. strlen("type=") = 5
}typedef query;

bool http_build_query(query* this_querry, unsigned char type_index, char* message)
{
	//http_query = "chat_id=%2B989380839938&data=test&type=text";

	/* check for presence of chat_id and token */
	// TODO: check for their correctness
	if (chat_id == NULL || token == NULL)
	{
		fprintf(stderr, "[ERROR] gapInit hasn't been called or has been set incorrectly!\n");
		return false;
	}

	/* chat_id */
	strcpy(this_querry->chat_id, "chat_id=%2B");	// '+' replaced with '%2B'.
	strcat(this_querry->chat_id, chat_id + 1);	// skip '+' charachter.

	/* type */
	strcpy(this_querry->type, "type=");
	strcat(this_querry->type, type[type_index]);

	/* data */
	this_querry->data = (char*)malloc(strlen(message) + 5);	// strlen("data=") = 5
	strcpy(this_querry->data, "data=");
	strcat(this_querry->data, message);

	return true;
}

/// <summary>
/// form http querry from query object
/// </summary>
/// <param name="this_querry"></param>
/// <returns>on success returns query string.</returns>
char* get_query_str(query* this_querry)
{
	char* http_query_str;

	// allocation of memory for char array
	// len(query* this_querry) = 21 + 9 + length of message + null
	http_query_str = (char*)malloc(31 + strlen(this_querry->data));

	strcpy(http_query_str, this_querry->chat_id);
	strcat(http_query_str, "&");
	strcat(http_query_str, this_querry->data);
	strcat(http_query_str, "&");
	strcat(http_query_str, this_querry->type);

	return http_query_str;
}

/// <summary>
/// forms and return token string from pre-initialized token variable.
/// </summary>
/// <returns>on success returns token string.</returns>
char* get_token_str()
{
	char* token_str;
	token_str = (char*)malloc(strlen(token) + 8);	// strlen(token) + strlen("token: ") + null;

	strcpy(token_str, "token: ");
	strcat(token_str, token);

	return token_str;
}

void gapInit(const char* this_chat_id, const char* this_token)
{
	chat_id = this_chat_id;
	token = this_token;
}

/*
* ┌─────────┬──────┬──────────┐
* │  name   │ type │ required │
* ├─────────┼──────┼──────────┤
* │ chat_id │ int  │   yes    │
* ├─────────┼──────┼──────────┤
* │  type   │ int  │   yes    │
* ├─────────┼──────┼──────────┤
* │  data   │ int  │   yes    │
* └─────────┴──────┴──────────┘
*/
bool sendText(const char* message)
{
	/* check for presence of chat_id and token */
	// TODO: check for their correctness
	if (chat_id == NULL || token == NULL)
	{
		fprintf(stderr, "[ERROR] gapInit hasn't been called or has been set incorrectly!\n");
		return false;
	}

	/* define variables */
	unsigned int baseURL_len, method_len, url_cmd_len;
	char* url_cmd;					// final command url string
	query http_query;				// http query object
	CURL* curl_handler;				// libcurl handler
	CURLcode curl_return;			// libcurl http return code
	struct curl_slist* list = NULL;

	baseURL_len = strlen(baseURL);
	method_len = strlen(method);
	url_cmd_len = (baseURL_len + method_len);
	url_cmd = (char*)malloc(url_cmd_len + 1);	// allocate memory for final url. null character considered.
	// TODO: use safe alternatives
	strcpy(url_cmd, baseURL);					// copy base url to final url
	strcat(url_cmd, method);					// append method string to final url

	list = curl_slist_append(list, get_token_str());
	http_build_query(&http_query, TYPETEXT, message);

	curl_handler = curl_easy_init();
	curl_easy_setopt(curl_handler, CURLOPT_URL, "https://api.gap.im/sendMessage");
	curl_easy_setopt(curl_handler, CURLOPT_HEADER, false);
	curl_easy_setopt(curl_handler, CURLOPT_POST, 1);
	curl_easy_setopt(curl_handler, CURLOPT_POSTFIELDS, get_query_str(&http_query));
	curl_easy_setopt(curl_handler, CURLOPT_HTTPHEADER, list);
	
	/* Perform the request, res will get the return code */
	curl_return = curl_easy_perform(curl_handler);

	/* memory cleanup */
	free(url_cmd);
	curl_slist_free_all(list);
	curl_easy_cleanup(curl_handler);

	/* check for return value from curl */
	if (curl_return != CURLE_OK)
	{
		fprintf(stderr, "[ERROR] cURL failed: %s\n", curl_easy_strerror(curl_return));
		return false;
	}
	else
		return true;
}
