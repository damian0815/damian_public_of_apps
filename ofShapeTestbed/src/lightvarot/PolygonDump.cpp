#include <stdlib.h>
#include <stdio.h>

#include "VarotPolygon.h"

void VarotPolygon::toFile(const char* fileName) {
	FILE *fp = fopen(fileName, "wb");
	if (fp == NULL)
		return;
	
	fprintf(fp, "<?xml version=\"1.0\"?>\n");
	fprintf(fp, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\">\n");

	/*
	 * a circle + text for each point
	 */
	fprintf(fp, "<g style=\"fill-opacity:0.7;\">\n");
	for (int i=0;i<_pts.nb();i++) {
		fprintf(fp, "  <text x=\"%fmm\" y=\"%fmm\" font-size=\"3\" font-family=\"Sans\" fill=\"green\">%i</text>\n", _pts[i]._x, _pts[i]._y+1,i);
		fprintf(fp, "  <circle cx=\"%fmm\" cy=\"%fmm\" r=\".1mm\" style=\"fill:red; stroke:black; stroke-width:0.02mm\" />\n", _pts[i]._x, _pts[i]._y);
	}
	fprintf(fp, "</g>\n");

	/*
	 * a line + text for each edge
	 */
	fprintf(fp, "<g style=\" fill-opacity:1;\">\n");
	for (int i=0;i<_edges.nb();i++) {
		float sx = _pts[_edges[i]._st]._x;
		float sy = _pts[_edges[i]._st]._y;
		float ex = _pts[_edges[i]._en]._x;
		float ey = _pts[_edges[i]._en]._y;
		//fprintf(fp, "  <text x=\"%fmm\" y=\"%fmm\" font-size=\"3\" font-family=\"Sans\" fill=\"blue\">%i(%i)</text>\n", 0.5*(sx+ex), 0.5*(sy+ey),i,_edges[i]._st);
		fprintf(fp, "  <text x=\"%fmm\" y=\"%fmm\" font-size=\"3\" font-family=\"Sans\" fill=\"blue\">%i</text>\n", 0.5*(sx+ex), 0.5*(sy+ey),i);
		fprintf(fp, "  <line x1=\"%fmm\" y1=\"%fmm\" x2=\"%fmm\" y2=\"%fmm\" style=\"fill:none; stroke:black; stroke-width:0.02mm\"/>\n", sx,sy,ex,ey);
	}
	fprintf(fp, "</g>\n");
	
	fprintf(fp, "</svg>\n");
	
	fclose(fp);
}
