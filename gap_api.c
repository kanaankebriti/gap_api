#define _CRT_SECURE_NO_WARNINGS

#include "gap_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
//#include <cjson/cJSON.h>

#ifdef _DEBUG
#pragma comment(lib, "libcurl-d.lib")
#else
#pragma comment(lib, "libcurl.lib")
#endif

const char baseURL[] = "https://api.gap.im/";
const char method[] = "sendMessage";
const char* chat_id;
const char* token;

void gapInit(const char* this_chat_id, const char* this_token)
{
	chat_id = this_chat_id;
	token = this_token;
}

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
	char* http_query;				// http query string
	CURL* curl_handler;				// libcurl handler
	CURLcode curl_return;			// libcurl http return code
	struct curl_slist* list = NULL;

	baseURL_len = strlen(baseURL);
	method_len = strlen(method);
	url_cmd_len = (baseURL_len + method_len);
	url_cmd = (char*)malloc(url_cmd_len + 1);	// allocate memory for final url. null character considered.
	strcpy(url_cmd, baseURL);					// copy base url to final url
	strcat(url_cmd, method);					// append method string to final url

	list = curl_slist_append(list,
		"token: ");

	http_query = "";
	curl_handler = curl_easy_init();
	curl_easy_setopt(curl_handler, CURLOPT_URL, "https://api.gap.im/sendMessage");
	curl_easy_setopt(curl_handler, CURLOPT_HEADER, false);
	curl_easy_setopt(curl_handler, CURLOPT_POST, 1);
	curl_easy_setopt(curl_handler, CURLOPT_POSTFIELDS, http_query);
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
