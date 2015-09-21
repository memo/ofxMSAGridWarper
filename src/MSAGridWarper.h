/*
 *  GridWarper.h
 *  BlazeMapper 2.0
 *
 *  Created by Mehmet Akten on 03/03/2010.
 *  Copyright 2010 MSA Visuals Ltd.. All rights reserved.
 *
 */

#pragma once

#include "MSACore.h"
#include "ofxFBOTexture.h"
#include "ofxXmlSettings.h"

namespace MSA {
	
	class GridPoint {
	public:
		float x, y;
		float u, v;
		
		void drawTexture(ofPoint &texSize) {
			glTexCoord2f(u * texSize.x, v * texSize.y);
			glVertex2f(x * texSize.x, y * texSize.y);
			//		ofCircle(x, y, 5);
		}
	};
	
	class GridWarper {
	public:
		ofxFBOTexture fbo;
		ofRectangle	drawRect;
		ofPoint texSize;
		
		void setup(int nx, int ny);
		void reset();
		void allocate(int nx, int ny, int w, int h, int internalGlDataType = GL_RGBA, int numSamples = 0);
		void begin();
		void end();
		
		void clear();
		void clear(float r, float g, float b, float a);
		
		
		void draw(int x, int y, int w, int h);
		void drawPoints();
		
		void checkMouse(float mouseX, float mouseY);
		
		
		int activePointIndex;
		void movePoint(float dx, float dy) {
			if(activePointIndex>=0) {
				printf("moving point %i by %f %f\n", activePointIndex, dx, dy);
				GridPoint &p = points[activePointIndex];
				p.u += dx * 1.0f/fbo.getWidth();
				p.v += dy * 1.0f/fbo.getHeight();
			}
		}
		
		void incActivePoint() {
			printf("incActivePoint");
			activePointIndex++;
			if(activePointIndex >= points.size()) activePointIndex = 0;
		}
		
		void decActivePoint() {
			printf("decActivePoint");
			activePointIndex--;
			if(activePointIndex < 0) activePointIndex = points.size()-1;
		}
		
		void loadXML(ofxXmlSettings &xml);
		void saveXML(ofxXmlSettings &xml);
		
		
	protected:
		int nx, ny;
		float inx, iny;
		vector<GridPoint> points;
	};
	
}