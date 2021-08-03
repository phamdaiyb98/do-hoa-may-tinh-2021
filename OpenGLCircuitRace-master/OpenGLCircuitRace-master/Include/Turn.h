/*
	Racing game project - Turn class
	Represents a turn as a ciruit part
	2019
	Nicolas Bouchard, Timoth�e Guy, Timoth�e Laurent
*/
// Class de quay xe
#ifndef ____TURN____
#define ____TURN____

#include "Position.h"
#include "TrackPart.h"

using namespace std;

class Turn : public TrackPart {

protected:
	vector<Position> tires;

	float softness;
	float angle;

	bool direction; // true : left, false : right

	/* Compute vertices */
	virtual void computeVertices(void);

	/* Bounding boxes generator */
	virtual void generateBoundingBoxes(float offset);

public:
	/* Constructors */
	Turn(void);

	Turn(float width, float softness, float angle, bool direction, Position pos);

	Turn(Turn *p1);

	/* Destructor */
	~Turn(void);

	/* Getters */
	float getSoftness(void);
	float getAngle(void);
	bool getDirection(void);

	/* Setters */
	void setSoftness(float softness);
	void setAngle(float angle);
	void setDirection(bool direction);

	/* Drawer */
	void draw(void);

	/* Util function to draw a cylindre */
	static void mySolidCylindre(double hauteur, double rayon, int ns);
};

#endif // Turn
