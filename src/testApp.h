#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		ofMesh baseMesh, mesh;
		ofSpherePrimitive sphere;
		ofLight light;		//Light
		ofSoundPlayer sound;	//Sound sample
		ofTexture texture;
		ofMaterial material;
		ofShader shader;
		ofCamera camera;
		void updateShape( ofMesh &mesh );
		void moveCamera( ofVec3f cameraPos, ofVec3f cameraTarget, ofVec3f cameraPosGoal, ofVec3f cameraTargetGoal );
		void keyReleased(int key);
		
};
void setNormals( ofMesh &mesh );
ofMesh createGeoSphere(int stacks, int slices, int radius);
