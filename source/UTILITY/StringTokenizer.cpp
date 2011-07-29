/*
 * StringTokenizer.cpp - Methods for StringTokenizer class.
 * 
 * Author: Patrick O'Leary
 * Created: October 22, 2007
 * Copyright: 2007
 */
#include <cstdlib>
#include <iostream>

#include <UTILITY/StringTokenizer.h>

/*
 * StringTokenizer - Constructor for class StringTokenizer.
 * 
 * parameter _tokenString - std::string&
 * parameter _deliminator - std::string&
 */
StringTokenizer::StringTokenizer(const std::string& _tokenString, const std::string& _deliminator) {
	if ((_tokenString.length() == 0) || (_deliminator.length() == 0))
		return;
	tokenString = _tokenString;
	deliminator = _deliminator;
	/* Remove sequential deliminators */
	std::string::size_type currentPosition = 0;
	while (true) {
		if ((currentPosition = tokenString.find(deliminator, currentPosition)) != std::string::npos) {
			currentPosition += deliminator.length();
			while (tokenString.find(deliminator, currentPosition) == currentPosition) {
				tokenString.erase(currentPosition, deliminator.length());
			}
		} else
			break;
	}
	/* Trim leading deliminator */
	if (tokenString.find(deliminator, 0) == 0) {
		tokenString.erase(0, deliminator.length());
	}
	/* Trim ending deliminatoriter */
	currentPosition = 0;
	if ((currentPosition = tokenString.rfind(deliminator)) != std::string::npos) {
		if (currentPosition != (tokenString.length() - deliminator.length()))
			return;
		tokenString.erase(tokenString.length() - deliminator.length(), deliminator.length());
	}
} // end StringTokenizer()

/*
 * ~StringTokenizer - Deconstructor for StringTokenizer.
 */
StringTokenizer::~StringTokenizer() {
} // end ~StringTokenizer()

/*
 * filterNextToken - Return next token with the filter string erased from it.
 * 
 * parameter filterString - const std::string&
 * return - std::string
 */
std::string StringTokenizer::filterNextToken(const std::string& filterString) {
	std::string temporaryString = nextToken();
	std::string::size_type currentPosition = 0;
	while ((currentPosition = temporaryString.find(filterString, currentPosition)) != std::string::npos) {
		temporaryString.erase(currentPosition, filterString.length());
	}
	return temporaryString;
} // end filterNextToken()

/*
 * getNumberOfTokens - Return the number of tokens in the string.
 * 
 * return - int
 */
int StringTokenizer::getNumberOfTokens() {
	if (tokenString.length() > 0) {
		int numberOfTokens = 0;
		std::string::size_type currentPosition = 0;
		std::string::size_type previousPosition = 0;
		while (true) {
			if ((currentPosition = tokenString.find(deliminator, currentPosition)) != std::string::npos) {
				numberOfTokens++;
				previousPosition = currentPosition;
				currentPosition += deliminator.length();
			} else
				break;
		}
		return ++numberOfTokens;
	} else {
		return 0;
	}
} // end getNumberOfTokens()

/*
 * hasMoreTokens - determines if there are more tokens in the token string.
 * 
 * return - bool
 */
bool StringTokenizer::hasMoreTokens() {
	return (tokenString.length() > 0);
} // end hasMoreTokens()

/*
 * nextToken - Return next token as a string.
 * 
 * return - std::string
 */
std::string StringTokenizer::nextToken() {
	if (tokenString.length() == 0)
		return "";
	std::string temporaryString = "";
	std::string::size_type position = tokenString.find(deliminator, 0);
	if (position != std::string::npos) {
		temporaryString = tokenString.substr(0, position);
		tokenString = tokenString.substr(position+deliminator.length(), tokenString.length()-position);
	} else {
		temporaryString = tokenString.substr(0, tokenString.length());
		tokenString = "";
	}
	return temporaryString;
} // end nextToken()

/*
 * nextToken - Return next token given a different deliminator.
 * 
 * parameter _otherDeliminator - const std::string&
 * return - std::string
 */
std::string StringTokenizer::nextToken(const std::string& _otherDeliminator) {
	if (tokenString.length() == 0)
		return "";
	std::string temporaryString = "";
	std::string::size_type position = tokenString.find(_otherDeliminator, 0);
	if (position != std::string::npos) {
		temporaryString = tokenString.substr(0, position);
		tokenString = tokenString.substr(position+_otherDeliminator.length(), tokenString.length()-position);
	} else {
		temporaryString = tokenString.substr(0, tokenString.length());
		tokenString = "";
	}
	return temporaryString;
} // end nextToken()

/*
 * nextDoubleToken - Return next token as a double (long float).
 * 
 * return - double
 */
double StringTokenizer::nextDoubleToken() {
	return atof(nextToken().c_str());
} // end nextDoubleToken()

/*
 * nextFloatToken - Return next token as a float.
 * 
 * return - float
 */
float StringTokenizer::nextFloatToken() {
	return float(atof(nextToken().c_str()));
} // end nextFloatToken()

/*
 * nextIntToken - Return next token as an int.
 * 
 * return - int
 */
int StringTokenizer::nextIntToken() {
	return atoi(nextToken().c_str());
} // end nextIntToken()

/*
 * remainingString - Return the remaining string.
 * 
 * return - std::string
 */
std::string StringTokenizer::remainingString() {
	return tokenString;
} // end remainingString()
