/*
 *  GridWarper.cpp
 *  BlazeMapper 2.0
 *
 *  Created by Mehmet Akten on 03/03/2010.
 *  Copyright 2010 MSA Visuals Ltd.. All rights reserved.
 *
 */

#include "MSAGridWarper.h"

namespace MSA {
	
	void GridWarper::setup(int nx, int ny) {
		printf("GridWarper::setup %i, %i\n", nx, ny);
		this->nx = nx;
		this->ny = ny;
		inx = 1.0f/(nx-1);
		iny = 1.0f/(ny-1);
		
		points.clear();
		for(int j=0; j<ny; j++) {
			for(int i=0; i<nx; i++) {
				GridPoint p;
				p.x = i * inx;// * fbo.getWidth();
				p.y = j * iny;// * fbo.getHeight();
				p.u = i * inx;// * fbo.getWidth();
				p.v = 1 - j * iny;// * fbo.getHeight();
				points.push_back(p);
				printf("creating point: %i, %i   %f, %f, %f, %f\n", i, j, p.u, p.v, p.x, p.y);
			}
		}
		
		activePointIndex = 0;
	}
	
	void GridWarper::reset() {
		setup(nx, ny);
	}
	
	
	void GridWarper::allocate(int nx, int ny, int w, int h, int internalGlDataType, int numSamples) {
		fbo.allocate(w, h, internalGlDataType, numSamples);
		texSize = ofPoint(fbo.getWidth(), fbo.getHeight());
		setup(nx, ny);
	}
	
	void GridWarper::begin() {
		fbo.begin();
	}
	
	void GridWarper::end() {
		fbo.end(); 
	}
	
	
	void GridWarper::clear() {
		fbo.clear();
	}
	
	void GridWarper::clear(float r, float g, float b, float a) {
		fbo.clear(r, g, b, a);
	}
	
	
	void GridWarper::draw(int x, int y, int w, int h) {
		drawRect.x = x;
		drawRect.y = y;
		drawRect.width = w;
		drawRect.height = h;
		
		glColor3f(1, 1, 1);
		fbo.bind();
		glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(w / fbo.getWidth(), h / fbo.getHeight(), 1);
		for(int j=0; j<ny-1; j++) {
			for(int i=0; i<nx-1; i++) {
				int index = j * nx + i;
				glBegin(GL_QUADS);
				points[index].drawTexture(texSize);
				points[index+1].drawTexture(texSize);
				points[index+nx+1].drawTexture(texSize);
				points[index+nx].drawTexture(texSize);
				glEnd();
			}
		}
		fbo.unbind();
		glPopMatrix();
	}
	
	
	void GridWarper::drawPoints() {
		int x = drawRect.x;
		int y = drawRect.y;
		int w = drawRect.width;
		int h = drawRect.height;
		
		glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(w / fbo.getWidth(), h / fbo.getHeight(), 1);
		glLineWidth(3);
		
		for(int i=0; i<points.size(); i++) {
			if(activePointIndex == i) {
				ofFill();
				glColor3f(1, 0, 0);
				ofCircle(points[i].x * fbo.getWidth(), points[i].y * fbo.getHeight(), 20);
				
				ofNoFill();
				glColor3f(0, 0, 0);
				ofCircle(points[i].x * fbo.getWidth(), points[i].y * fbo.getHeight(), 20);
			} else {
				ofFill();
				glColor3f(0, 1, 0);
				ofCircle(points[i].x * fbo.getWidth(), points[i].y * fbo.getHeight(), 10);
				
				ofNoFill();
				glColor3f(0, 0, 0);
				ofCircle(points[i].x * fbo.getWidth(), points[i].y * fbo.getHeight(), 10);
			}
		}
		ofFill();
		glPopMatrix();
	}
	
	
	void GridWarper::checkMouse(float mouseX, float mouseY) {
		activePointIndex = -1;
		for(int i=0; i<points.size(); i++) {
			GridPoint &p = points[i];
			//			GLdouble world_x = p.x;
			//			GLdouble world_y = p.y;
			//			GLdouble world_z = 0;
			//			
			//			GLdouble screen_x, screen_y, screen_z;
			//			
			//			GLdouble ModelView[16];
			//			GLdouble Projection[16];
			//			GLint Viewport[4];
			//			
			//			glGetDoublev(GL_MODELVIEW_MATRIX, ModelView);
			//			glGetDoublev(GL_PROJECTION_MATRIX, Projection);
			//			glGetIntegerv(GL_VIEWPORT, Viewport);
			//			
			//			gluProject(world_x, world_y, world_z, ModelView, Projection, Viewport, &screen_x, &screen_y, &screen_z);	
			//			
			//			printf("screen pos %f, %f mouse pos %f %f %s\n", screen_x, screen_y, mouseX, mouseY, i/3 == activePointIndex ? " *** " : "");
			//			if(ofDistSquared(mouseX, mouseY, screen_x, screen_y) < 100) {
			//				activePointIndex = i/3;
			//			}
			
			float px = ofMap(p.x, 0, 1, drawRect.x, drawRect.x + drawRect.width);
			float py = ofMap(p.y, 0, 1, drawRect.y, drawRect.y + drawRect.height);
			
			if(ofDistSquared(mouseX, mouseY, px, py) < 100) {
				activePointIndex = i;
			}
		}
	}
	
	
	void GridWarper::loadXML(ofxXmlSettings &xml) {
		printf("GridWarper::loadXML\n");
		xml.pushTag("grid");
		int numGridX = xml.getValue("numGridX", 11);
		int numGridY = xml.getValue("numGridY", 11);
		
		printf("read grid xy: %i, %i\n", numGridX, numGridY);
		
		if(numGridX==0) numGridX = 11;
		if(numGridY==0) numGridY = 11;
		
		allocate(numGridX, numGridY, outputManager.getWidth(), outputManager.getHeight(), GL_RGB, 1);
		clear(0, 0, 0, 0);
		
		for(int i = 0; i < nx * ny; i++) {
			points[i].u = xml.getValue("pt:x", 0, i) / 1000.0f;
			points[i].v = xml.getValue("pt:y", 0, i) / 1000.0f;
			printf("reading point %i %f, %f\n", i, points[i].u, points[i].v);
		}
		xml.popTag();
	}
	
	void GridWarper::saveXML(ofxXmlSettings &xml) {
		printf("GridWarper::saveXML\n");
		xml.addTag("grid");
		xml.pushTag("grid");
		xml.setValue("numGridX", nx);
		xml.setValue("numGridY", ny);
		for(int i = 0; i < nx * ny; i++) {
			xml.setValue("pt:x", points[i].u * 1000, i);
			xml.setValue("pt:y", points[i].v * 1000, i);
			printf("writing point %i %f, %f\n", i, points[i].u, points[i].v);
		}
		xml.popTag();
	}
	
}