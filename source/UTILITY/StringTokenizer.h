/*
 * StringTokenizer.h - Class StringTokenizer used to break-up strings of data that have already been created from 
 * tokens and have also been separated by delimiters.
 * 
 * Author: Patrick O'Leary
 * Created: October 22, 2007
 * Copyright: 2007
 */
#ifndef STRINGTOKENIZER_H_
#define STRINGTOKENIZER_H_

#include <string>

class StringTokenizer {
public:
	StringTokenizer(const std::string& _tokenString, const std::string& _deliminator);
	~StringTokenizer();
	std::string filterNextToken(const std::string& filterString);
	int getNumberOfTokens();
	bool hasMoreTokens();
	std::string nextToken();
	std::string nextToken(const std::string& deliminator);
	double nextDoubleToken();
	float nextFloatToken();
	int nextIntToken();
	std::string remainingString();
private:
	std::string tokenString;
	std::string deliminator;
};

#endif /*STRINGTOKENIZER_H_*/
