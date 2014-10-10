/*
 KEYS
 1-3: different colored materials
 4-6: different camera views
 a-d: different movements
 */

#include "testApp.h"

const int N = 256;		//Number of bands in spectrum
float spectrum[ N ];	//Smoothed spectrum values
int bandFreq = 30, bandLength = 50, bandHeight1 = 90, bandHeight2 = 70;
float pikeFreq = 1, pikeLength = 1, pikeHeight1 = 1, pikeHeight2 = 1;
int movementMode = 2;
ofVec3f cameraPos = ofVec3f(-300, 280, 800), cameraTarget = ofVec3f(-40, 0, 300);
ofVec3f cameraPosGoal = cameraPos, cameraTargetGoal = cameraTarget;

//--------------------------------------------------------------
void testApp::setup(){

	//load sound
	sound.loadSound( "moon.mp3" );
	sound.setLoop( true );
	sound.play();

	//Set spectrum values to 0
	for (int i=0; i<N; i++) {
		spectrum[i] = 0.0f;
	}
	light.enable();				//Enable lighting
	
	ofSetFrameRate( 60 );       //Set the rate of screen redrawing
	
    //make an identical baseMesh to record the start position of all vertices
	baseMesh = createGeoSphere(80,80, 220);
	mesh = createGeoSphere(80,80, 220);
	setNormals( mesh );
    
	//light settings
    ofColor ambientColor = ofColor(25, 30, 80);
    ofSetGlobalAmbientColor( ofColor(0, 0, 0) );
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetSmoothLighting(true);
    light.setAmbientColor(ambientColor);
    
	//material settings
    material.setShininess(50);
    material.setSpecularColor(ofColor(0, 0, 0 ));
    material.setEmissiveColor(ofColor(0, 0, 0));
    material.setDiffuseColor(ofColor(100, 100, 100));
    material.setAmbientColor(ofColor(0, 0, 0));
	
	//camera settings
    camera.setPosition(cameraPos);
    camera.lookAt(cameraTarget);
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofSoundUpdate();
	float *val = ofSoundGetSpectrum( N );
	for ( int i=0; i<N; i++ ) {
		spectrum[i] *= 0.97;	//Slow decreasing
		spectrum[i] = max( spectrum[i], val[i] );
	}
    
	//get desired numbers from different freq
	pikeFreq += (ofMap( spectrum[ bandFreq ], 0, 1.8, 5, 2000, false )-pikeFreq)*.01;
	pikeLength += (ofMap( spectrum[ bandLength ], 0, 0.05, 1, 46, true )-pikeLength)*.01;
	pikeHeight1 += (ofMap( spectrum[ bandHeight1 ], 0, 0.05, 5, 240, true )-pikeHeight1)*.1;
	pikeHeight2 += (ofMap( spectrum[ bandHeight2 ], 0, 0.1, 5, 120, true )-pikeHeight2)*.1;
	
	updateShape( mesh );
    
	//move current camera toward to the goal
	cameraTarget += (cameraTargetGoal-cameraTarget)*0.02;
	cameraPos += (cameraPosGoal-cameraPos)*0.02;
	camera.setPosition(cameraPos);
    camera.lookAt(cameraTarget);
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //display the mesh and keep rotating
	ofEnableDepthTest();
	ofBackground(ofColor(0, 0, 0));

	float time = ofGetElapsedTimef();
	float angle = time*10;
	camera.begin();
	ofPushMatrix();
	ofTranslate( 0, 0, 0 );
	ofRotate( angle, 1, 0, 1 );
	material.begin();
	mesh.draw();
	material.end();
	
	ofPopMatrix();
	ofVec3f centerPoint = mesh.getCentroid();
	
	camera.end();
}

//---------------------

float noiseBase = 0, sinBase = 0;
void testApp::updateShape( ofMesh &mesh ) {
	float time = ofGetElapsedTimef();
    //go through all the vertices and manipulate them
	for( int k=0; k<mesh.getNumVertices(); k++ ) {
		int circleCount = floor(k/50);
		ofPoint v1 = baseMesh.getVertex(k);
		ofPoint v2 = mesh.getVertex(k);
		ofVec3f v1Normalized = v1.getNormalized();
		int a = (int) (pikeFreq+pikeHeight1*0.05+pikeHeight2*0.02), b = (int) (pikeLength-pikeHeight1*0.03);
		noiseBase += 0.01 + pikeHeight2*0.03 + pikeHeight1*0.01;
        
        //different manipulation according to the movement mode
		switch ( movementMode ) {
			case 1: {
				float baseHeight = sin(k*.002+time*.1)-0.5;//ofClamp( a-500, 0, 300);
				if( k%a <= b ) {
					float h = pikeHeight1 + pikeHeight2;
					v1.x += (ofNoise( 0.2, noiseBase, k)*h*baseHeight)*v1Normalized.x;
					v1.y += (ofNoise( 0.3, noiseBase, k)*h*baseHeight)*v1Normalized.y;
					v1.z += (ofNoise( 0.4, noiseBase, k)*h*baseHeight)*v1Normalized.z;
				}
				break;
			}
			case 2: {
					
				if( k%a <= b ) {
					float h = pikeHeight1 + pikeHeight2;
					v1.x -= ofNoise( 0.2, time, k)*v1Normalized.x*h;
					v1.y -= ofNoise( 0.3, time, k)*v1Normalized.y*h;
					v1.z = ofNoise( 0.4, time, k)*v1Normalized.z;
				}
				break;
			}
			case 3: {
				int currentCircle = floor( k/80 );
				sinBase += ofClamp((pikeHeight2 + pikeHeight1)*0.000005, 0, 0.00002) + 0.00001;
				float adj = sin((currentCircle)*.3+sinBase)+0.1;
				if( k%a <= b ) {
					float h = (pikeHeight1 + pikeHeight2)*0.8;
					int nx = v1Normalized.x/abs(v1Normalized.x);
					int ny = v1Normalized.y/abs(v1Normalized.y);
					int nz = v1Normalized.z/abs(v1Normalized.z);
					v1.x += ofNoise( 0.2, noiseBase, k*.8)*h*v1Normalized.x*adj;
					v1.y += ofNoise( 0.3, noiseBase, k*.8)*h*v1Normalized.y*adj;
					v1.z += ofNoise( 0.4, noiseBase, k*.8)*h*v1Normalized.z*adj;
				}

				break;
			}
			case 4: {
				int currentCircle = floor( k/80 );
				int countInline = k%80;
				int rangeN = (int) abs(sin(pikeLength*.2)*10);
				int range = (int) sin( currentCircle*0.005 )*rangeN;
				int division = rangeN+range+1;
				int reminder = countInline%division;
				float length = (rangeN+range+1)*abs(sin(pikeFreq*.1))+1;
				float height = (pikeHeight2 + pikeHeight1)*.5;
				float wave = (sin( currentCircle*(1-(pikeLength*.15)) )+0.5)*ofClamp(height, 0, 80)*sin(reminder*(-180/division));
				if( reminder < length ) {
					v1.x += v1Normalized.x*wave;
					v1.y += v1Normalized.y*wave;
					v1.z += v1Normalized.z*wave;
				} else {
					v1.x += ofNoise( 0.2, noiseBase, k*.8)*v1Normalized.x*height*.3;
					v1.y += ofNoise( 0.3, noiseBase, k*.8)*v1Normalized.y*height*.3;
					v1.z += ofNoise( 0.4, noiseBase, k*.8)*v1Normalized.z*height*.3;
				}
			}
		}
		
		ofPoint diff = (v1-v2)*.1;
		v2 += diff;
		mesh.setVertex(k, v2);
	}
}

//---------------------

ofMesh createGeoSphere(int stacks, int slices, int radius) {
    ofMesh mesh;

    //add vertices
    mesh.addVertex(ofVec3f(0,0,1*radius));

    ofFloatColor color( 0.2, 0.6, 0.6);
    for (int i=1; i<stacks; i++) {
        double phi = PI * double(i)/stacks;
        double cosPhi = cos(phi);
        double sinPhi = sin(phi);
        for (int j=0; j<slices; j++) {
            double theta = TWO_PI * double(j)/slices;
			mesh.addColor( color );
            mesh.addVertex(ofVec3f(cos(theta)*sinPhi*radius, sin(theta)*sinPhi*radius, cosPhi*radius));
            
        }
    }
    mesh.addVertex(ofVec3f(0,0,-1*radius));
    
    //top row triangle fan
    for (int j=1; j<slices; j++) {
        mesh.addTriangle(0,j,j+1);
    }
    mesh.addTriangle(0,slices,1);
    
    //triangle strips
    for (int i=0; i < stacks-2; i++) {
        int top = i*slices + 1;
        int bottom = (i+1)*slices + 1;
        
        for (int j=0; j<slices-1; j++) {
            mesh.addTriangle(bottom+j, bottom+j+1, top+j+1);
            mesh.addTriangle(bottom+j, top+j+1, top+j);
        }
        mesh.addTriangle(bottom+slices-1, bottom, top);
        mesh.addTriangle(bottom+slices-1, top, top+slices-1);
    }
    
    //bottom row triangle fan
    int last = mesh.getNumVertices()-1;
    for (int j=last-1; j>last-slices; j--) {
        mesh.addTriangle(last,j,j-1);
    }
    mesh.addTriangle(last,last-slices,last-1);
    
    return mesh;
}

//---------------------

void testApp::moveCamera( ofVec3f cameraPos, ofVec3f cameraTarget, ofVec3f cameraPosGoal, ofVec3f cameraTargetGoal ) {

	cameraTarget += (cameraTargetGoal-cameraTarget)*0.1;
	cameraPos = (cameraPosGoal);
	camera.setPosition(cameraPos);
    camera.lookAt(cameraTarget);
	
}

//---------------------

void setNormals( ofMesh &mesh ){
    
	//The number of the vertices
	int nV = mesh.getNumVertices();
    
	//The number of the triangles
	int nT = mesh.getNumIndices() / 3;
    
	vector<ofPoint> norm( nV ); //Array for the normals
    
	//Scan all the triangles. For each triangle add its
	//normal to norm's vectors of triangle's vertices
	for (int t=0; t<nT; t++) {
        
		//Get indices of the triangle t
		int i1 = mesh.getIndex( 3 * t );
		int i2 = mesh.getIndex( 3 * t + 1 );
		int i3 = mesh.getIndex( 3 * t + 2 );
        
		//Get vertices of the triangle
		const ofPoint &v1 = mesh.getVertex( i1 );
		const ofPoint &v2 = mesh.getVertex( i2 );
		const ofPoint &v3 = mesh.getVertex( i3 );
        
		//Compute the triangle's normal
		ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
        
		//Accumulate it to norm array for i1, i2, i3
		norm[ i1 ] += dir;
		norm[ i2 ] += dir;
		norm[ i3 ] += dir;
	}
    
	//Normalize the normal's length
	for (int i=0; i<nV; i++) {
		norm[i].normalize();
	}
    
	//Set the normals to mesh
	mesh.clearNormals();
	mesh.addNormals( norm );
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
	if( key == 49 ) {
		material.setSpecularColor(ofColor(10, 0, 50, 255));
		material.setEmissiveColor(ofColor(40, 0, 10, 100));
		material.setDiffuseColor(ofColor(0, 0, 0));
		material.setAmbientColor(ofColor(30, 20, 50));

	} else if( key == 50) {
		material.setSpecularColor(ofColor(30, 0, 30, 15));
		material.setEmissiveColor(ofColor(90, 30, 30, 100));
		material.setDiffuseColor(ofColor(90, 90, 90));
		material.setAmbientColor(ofColor(50, 40, 40));
	} else if( key == 51 ) {
		material.setSpecularColor(ofColor(0, 0, 0 ));
		material.setEmissiveColor(ofColor(0, 0, 0));
		material.setDiffuseColor(ofColor(100, 100, 100));
		material.setAmbientColor(ofColor(0, 0, 0));
	} else if( key == 52 ) {
		cameraPosGoal = ofVec3f(-300, 280, 800);
		cameraTargetGoal = ofVec3f(-40, 0, 300);
		
	} else if( key == 53 ) {
		cameraPosGoal = ofVec3f(-200, 300, 400);
		cameraTargetGoal = ofVec3f(230, -260, 50);
	} else if( key == 54 ) {
		cameraPosGoal = ofVec3f(0, 0, 600);
		cameraTargetGoal = ofVec3f(0, 0, 100);
	}

	if( key == 97 ) {
		movementMode = 1;
	} else if( key == 98 ) {
		movementMode = 2;
	} else if( key == 99 ) {
		movementMode = 3;
	} else if( key == 100 ) {
		movementMode = 4;
	}
	ofLog() << "the number is " << key;
}

