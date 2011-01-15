#include <stdlib.h>
#include <stdio.h>

#include "Path.h"
#include "Image.h"
#include "Polygon.h"
#include "Region.h"
#include "PointSequence.h"
#include "Voronoi.h"

void	makePotato(Path* dest, float cx, float cy, float r);

class IntCmp : public MComparator<int> {
public:
	IntCmp(void) {};
	virtual ~IntCmp(void) {};
	virtual int	compare(int const &a, int const &b) const {return a-b;};
};

/*int main(int nbArgs, char* argv[]) {
	// make a path with a bunch of round shapes
	Path*	pat = new Path();
	for (int i=0;i<20;i++) {
		int		ix = rand() % 500;
		int		iy = rand() % 500;
		int		ir = 50 + rand() % 50;
		makePotato(pat, ix, iy, ir);
	}

	// remove self-intersections
	Polygon*	pol = new Polygon();
	pol->setSource(pat,true);
	pat->fill(pol, ldexpf(1, -rounding_power));
	pol->uncross(winding_evenodd, true);
	Path*	res = pol->givePath();
	res->dump();

	// draw to png
	Image	imA(500,500);
	pol->toImage(&imA);
	imA.blit();
	imA.writeToFile("testA.png");

	// make a square
	Path*		clip = new Path();
	clip->moveto(MPoint<float>(100,100));
	clip->lineto(MPoint<float>(400,100));
	clip->lineto(MPoint<float>(400,400));
	clip->lineto(MPoint<float>(100,400));
	clip->close();

	// draw to png
	Image	imB(500,500);
	clip->fill(&imB, ldexpf(1, -rounding_power));
	imB.blit();
	imB.writeToFile("testB.png");

	// compute intersection
	Polygon*	inters = new Polygon();
	inters->setSource(res, true);
	res->fill(inters, ldexpf(1, -rounding_power));
	inters->setSource(clip, false);
	clip->fill(inters, ldexpf(1, -rounding_power));
	inters->uncross(winding_AND, false);

	// and draw intersection to png
	Image	imR(500,500);
	inters->toImage(&imR);
	imR.blit();
	imR.writeToFile("testR.png");

	delete pat;
	delete res;
	delete pol;
	delete clip;
	delete inters;
	return 0;
}*/

/*int main(int nbArgs, char* argv[]) {
	// make a path with a bunch of round shapes
	Path*	pat = new Path();
	for (int i=0;i<10;i++) {
		int		ix = rand() % 500;
		int		iy = rand() % 500;
		int		ir = 50 + rand() % 50;
		makePotato(pat, ix, iy, ir);
	}
	pat->moveto(MPoint<float>(200,100));
	pat->arcto(MPoint<float>(250,150), MPoint<float>(200,200), false);
	pat->arcto(MPoint<float>(200,150), MPoint<float>(200,100), true);
	pat->close();

	pat->moveto(MPoint<float>(300,200));
	pat->curveto(MPoint<float>(300,150), MPoint<float>(400,250), MPoint<float>(400,200));
	pat->close();

	Path* opat = pat->offset(-5,0.1);
	Path* ipat = pat->offset(2,0.1);

	// remove self-intersections
	Polygon*	pol = new Polygon();
	pol->setSource(opat,true);
	opat->fill(pol, ldexpf(1, -rounding_power));
	pol->setSource(ipat,false);
	ipat->fill(pol, ldexpf(1, -rounding_power));
	pol->uncross(winding_DIFF, true);
	Path*	res = pol->givePath();
	res->dump();

	// draw to png
	Image	imA(500,500);
	pol->toImage(&imA);
	imA.blit();
	imA.writeToFile("testA.png");


	delete pat;
	delete opat;
	delete ipat;
	delete pol;
	delete res;
	return 0;
}*/

int main(int nbArgs, char* argv[]) {
	PointSequence	seq; // sequence of point we're going to approximate
	Path*	pbt = new Path(); // store the exact point sequence, for comparison

	double	px = 200, py = 200; // position
	double	vx = 10, vy = 0;    // speed
	double	dx = 0, dy = 0;     // destination
	for (int i=0;i<=100;i++) {
		MPoint<float>	npt(px, py);
		// add new point to sequence and control path
		seq.addPoint(npt);
		if ( i == 0 ) pbt->moveto(npt); else pbt->lineto(npt);

		// move point to make 'as if' it was attracted by something
		if ( i - 20 * (i/20) == 0 ) {
			// every 20 points, make a new destination to 'attract' the position
			int	ix = (rand()%400);
			int	iy = (rand()%400);
			dx = ((double)ix);
			dy = ((double)iy);
		}
		double	rx = dx - px;
		double  ry = dy - py;
		// behave like a dot attracted to the current destination point
		// by a spring, with some friction
		vx = 0.9 * vx + 0.05 * rx;
		vy = 0.9 * vy + 0.05 * ry;
		px += vx;
		py += vy;
	}
	pbt->close();

	// make simplified curve
	Path*	pat = seq.simplify(3.0, true);
	pat->close();

	// remove self-intersections and get shapes
	Polygon*	pal = new Polygon();
	pal->setSource(pat,true);
	pat->fill(pal, ldexpf(1, -rounding_power));
	pal->uncross(winding_evenodd, true);
	Path* resa = pal->givePath();

	Polygon*	pbl = new Polygon();
	pbl->setSource(pbt,true);
	pbt->fill(pbl, ldexpf(1, -rounding_power));
	pbl->uncross(winding_evenodd, true);
	Path* resb = pbl->givePath();

	// make difference between original and simplified
	Polygon*	pol = new Polygon();
	pol->setSource(resa,true);
	resa->fill(pol, ldexpf(1, -rounding_power));
	pol->setSource(resb,false);
	resb->fill(pol, ldexpf(1, -rounding_power));
	pol->uncross(winding_XOR, false);

	// draw to png
	Image	imA(400,400);
	pol->toImage(&imA);
	imA.blit();
	imA.writeToFile("testA.png");

	delete pat;
	delete pbt;
	delete pal;
	delete pbl;
	delete resa;
	delete resb;
	delete pol;
	return 0;
}


/*int main(int nbArgs, char* argv[]) {
	Voronoi vor;
	// vor will be a voronoi graph of points: fill it with a bunch of random pointes
	for (int i=0;i<20;i++) {
		int ix = rand() % 300 + 50;
		int iy = rand() % 300 + 50;
		vor.addPoint(MPoint<double>(ix, iy));
	}
	// compute graph
	// the result is a set of closed paths representing the regions
	Path*	res = vor.graph();
	// inset to make the faces of the graph disjoint (otherwise the image will be full black)
	Path*	ins = res->offset(1.0, 0.1);

    // draw the path: transform to polygon, remove self intersections, and raster
	Polygon*	pol = new Polygon();
	pol->setSource(ins,true);
	ins->fill(pol, ldexpf(1, -rounding_power));
	pol->uncross(winding_positive, false);

	// draw to png
	Image	imA(400,400);
	pol->toImage(&imA);
	imA.blit();
	imA.writeToFile("testA.png");

	delete res;
	delete ins;
	delete pol;
	return 0;
}*/

void	makePotato(Path* dest, float cx, float cy, float r) {
	int		iang = rand() % 1000;
	float	ang = 2 * M_PI * 0.001 * ((float)iang);
	float	co = cosf(ang);
	float	si = sinf(ang);
	MPoint<float>	cur(cx + r * co, cy + r * si);
	dest->moveto(cur);
	for (int i=0;i<4;i++) {
		int		ipr = rand()%100;
		float	pr = 0.01 * ((float) ipr);
		float	nco = -si;
		float	nsi = co;
		pr = 1 + 0.5 * (pr - 0.5);
		if ( i == 3 ) pr = 1;
		MPoint<float>	ncur(cx + pr * r * nco, cy + pr * r * nsi);
		MPoint<float>	p1(cur._x  + 0.333 * r * nco, cur._y  + 0.333 * r * nsi);
		MPoint<float>	p2(ncur._x + 0.333 * r * co , ncur._y + 0.333 * r * si);
		dest->curveto(p1,p2,ncur);
		co = nco;
		si = nsi;
		cur = ncur;
	}
	dest->close();
}
