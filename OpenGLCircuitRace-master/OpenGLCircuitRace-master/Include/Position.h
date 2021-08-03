/*
	Racing game project - Position

	Represents a position in a 3D world

	2019
	Nicolas Bouchard, Timothee Guy, Timothee Laurent
*/
// Class mieu ta vi tri trong the gioi 3D
#ifndef ____POSITION____
#define ____POSITION____

class Position {

public:
	float x, y, z, angle;

public:
	/* Constructors */
	Position(void);

	Position(float cx, float cy, float cz);

	Position(float cx, float cy, float cz, float cangle);

	Position(Position *p);

	/* Destructor */
	~Position(void);
};

#endif