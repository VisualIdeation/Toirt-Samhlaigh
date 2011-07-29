/*
 * ComparableBrick.h - Class for sorting Bricks.
 * 
 * Author: Patrick O'Leary
 * Created: February 20, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef COMPARABLEBRICK_H_
#define COMPARABLEBRICK_H_

#include <string>

// begin Forward Declarations
class Brick;
// end Forward Declarations
class ComparableBrick {
public:
	static bool greater;
	explicit ComparableBrick(int _which = 0, float _value = 0.0, Brick * _brick = NULL);
	ComparableBrick(const ComparableBrick & rhs);
	~ComparableBrick(void);
	Brick* getBrick(void); // May mutate the brick
	void setBrick(Brick * _brick);
	float getValue(void) const;
	void setValue(float _value);
	int getWhich(void) const;
	void setWhich(int _which);
	void minimum(void);
	void maximum(void);
	const ComparableBrick & operator=(const ComparableBrick & rhs);
	bool operator<(const ComparableBrick & rhs) const;
	std::string toString(void);
private:
	int which;
	float value;
	Brick* brick;
};

#endif /*COMPARABLEBRICK_H_*/
