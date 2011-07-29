/*
 * ComparableOcNode.h - Class for sorting OcNodes.
 * 
 * Author: Patrick O'Leary
 * Created: February 20, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef COMPARABLEOCNODE_H_
#define COMPARABLEOCNODE_H_

#include <string>

// begin Forward Declarations
class OcNode;
// end Forward Declarations
class ComparableOcNode {
public:
	static bool greater;
	explicit ComparableOcNode(int _which = 0, float _value = 0.0, OcNode * _ocNode = NULL);
	ComparableOcNode(const ComparableOcNode & rhs);
	~ComparableOcNode(void);
	OcNode * getOcNode(void) const;
	void setOcNode(OcNode * _ocNode);
	float getValue(void) const;
	void setValue(float _value);
	int getWhich(void) const;
	void setWhich(int _which);
	void minimum(void);
	void maximum(void);
	const ComparableOcNode & operator=(const ComparableOcNode & rhs);
	bool operator<(const ComparableOcNode & rhs) const;
	std::string toString(void);
private:
	int which;
	float value;
	OcNode * ocNode;
};

#endif /*COMPARABLEOCNODE_H_*/
