#include "polar.h"

#ifndef	HASAUDIO
float beat = 0.0;
#endif

ofLight luzinha3d;
ofMaterial material3d;

//ofCamera camera3d;
ofEasyCam camera3d;

//ofxDmtrUI3 *uiCam = ui;
//ofxDmtrUI3 *uiLuz = ui;
//ofxDmtrUI3 *uiMaterial = ui;

ofxMicroUI *uiCam = &u.uis["cam"];
ofxMicroUI *uiLuz = &u.uis["uiLuz"];
ofxMicroUI *uiMaterial = &u.uis["uiMaterial"];

ofVec3f cameraLook3d, cameraLookPosition3d;

ofNode lookNode;

// precisa vir antes do loadMaster pra poder fazer os presets antes.
void setupCam_3d() {
	ofAddListener(uiCam->uiEvent, this, &ofApp::dmtrCamUIEvent_3d);
	
	
string s =
R"(beginTemplate	camControl3d
tag	camNeue
#bool	ortho	0
bang	resetLook	0
float	lookX	-30 30 0
float	lookY	-1 20 0
float	lookZ	-30 30 0
float	cameraX	-20 20 0
float	cameraY	-6 20 0
float	cameraZ	-20 20 0
bool	cameraPolar	1
float	cameraDist	0 50 23
float	cameraAngle	-180 180 -45
float	cameraFov	10 180 35
#radio	cameraFov_shortcut	12 22 35 50 70 100 150
	
bang	resetCam
bang	resetAutoRotate
float	rotCamX	-180 180 0
float	rotCamY	-180 180 0	audio
float	rotCamZ	-180 180 0
float	rotCamXAuto	-1.5 1.5 0
float	rotCamYAuto	-1.5 1.5 0
float	rotCamZAuto	-1.5 1.5 0
bool	rotCam	1
tag
endTemplate
#=============================
beginTemplate	luzControl
tag	light
toggle	LUZ	0 1 0
float	luzX	-2000 2000 0	beat
float	luzY	-2000 2000 0	beat
float	luzZ	-2000 2000 0	beat
bool	shadeFlat	0
###
float	lightAttenuationConstant	0 2 1
float	lightAttenuationLinear	0 .0002 0
float	lightAttenuationQuadratic	0 .000001 0
###
float	diffuseColorLuz	0 255 255
#float	diffuseColorLuzAlpha	0 255 100
float	specularColorLuz	0 255 255
#float	specularColorLuzAlpha	0 255 100
float	ambientColorLuz	0 255 255
#=============================
#color	specularColor
#color	diffuseColor
float	areaLight	0 2000 1000
float	materialShininess	0 100 20
bool	enableSeparateSpecularLight	1
tag
#bool	shadeFlat	0
endTemplate
#=============================
beginTemplate	materialControl
toggle	material	0 1 0
#label	MATERIAL
float	specularColorMaterial	0 255 255
float	specularColorMaterialAlpha	0 255 100
float	diffuseColorMaterial	0 255 255
float	emissiveColorMaterial	0 255 255
float	ambientColorMaterial	0 255 255
endTemplate)";

	
	//uiCam->createFromLines(s);
//	uiLuz->createFromLines(s);
//	uiMaterial->createFromLines(s);
	luzinha3d.setPointLight();
//	camera3d.enableInertia();

}
	
//--------------------------------------------------------------
void beginIlumina_3d(){
	if (uiLuz->pBool["LUZ"]) {
		ofEnableLighting();
		//cout << "luz" << endl;
		
		luzinha3d.setAreaLight(uiLuz->pEasy["areaLight"],uiLuz->pEasy["areaLight"]);

		luzinha3d.setAttenuation(
							uiLuz->pEasy["lightAttenuationConstant"],
							uiLuz->pEasy["lightAttenuationLinear"],
							uiLuz->pEasy["lightAttenuationQuadratic"]);
		
		luzinha3d.setPosition(
							uiLuz->pEasy["luzX"] + uiLuz->pEasy["luzXBeat"] * beat,
							uiLuz->pEasy["luzY"] + uiLuz->pEasy["luzYBeat"] * beat,
							uiLuz->pEasy["luzZ"] + uiLuz->pEasy["luzZBeat"] * beat);

		luzinha3d.setDiffuseColor( ofColor(uiLuz->pEasy["diffuseColorLuz"]));
		luzinha3d.setSpecularColor( ofColor(uiLuz->pEasy["specularColorLuz"]));
		luzinha3d.setAmbientColor( ofColor(uiLuz->pEasy["ambientColorLuz"]));
		luzinha3d.enable();
	}
}

//--------------------------------------------------------------
void endIlumina_3d(){
	if (uiLuz->pBool["LUZ"]) {
		luzinha3d.disable();
		ofDisableLighting();
		ofDisableSeparateSpecularLight();
	}
}

//--------------------------------------------------------------
void beginMaterial_3d() {
	if (uiMaterial->pBool["material"]) {
		material3d.setShininess( uiMaterial->pEasy["materialShininess"] );
		float sp = uiMaterial->pEasy["specularColorMaterial"];
		float spAlpha = uiMaterial->pEasy["specularColorMaterialAlpha"];
		material3d.setSpecularColor(ofColor(sp,spAlpha));
		material3d.setDiffuseColor (ofColor(uiMaterial->pEasy["diffuseColorMaterial"]));
		material3d.setEmissiveColor(ofColor(uiMaterial->pEasy["emissiveColorMaterial"]));
		material3d.setAmbientColor (ofColor(uiMaterial->pEasy["ambientColorMaterial"]));
		material3d.begin();
	}
}

//--------------------------------------------------------------
void endMaterial_3d() {
	if (uiMaterial->pBool["material"]) {
		material3d.end();
	}
}

//--------------------------------------------------------------
void beginCam_3d(float eyeDistance = 0) {
	
	if (uiCam->pBool["disableDepthTest"]) {
		ofDisableDepthTest();
	} else {
		ofEnableDepthTest();
	}

	float pointsPerMeter = 100.0;
	float cameraX = uiCam->pEasy["cameraX"] * pointsPerMeter;
	float cameraZ = uiCam->pEasy["cameraZ"] * pointsPerMeter;
	
	camera3d.setNearClip(1.0);
	camera3d.setFarClip(160 * pointsPerMeter);

	// 1.70, a altura de um adulto em p�

	if (uiCam->pBool["cameraPolar"]) {
		float a = uiCam->pEasy["cameraAngle"] + 90.0;
		float d = uiCam->pEasy["cameraDist"] * pointsPerMeter;
		cameraX = r2x(a, d);
		cameraZ = r2y(a, d);

		cameraX += lookNode.getPosition().x;
		cameraZ += lookNode.getPosition().z;
	}
	
	cameraX += eyeDistance;
	
	if (!u.pBool["mouseCamera"]) {
		camera3d.setPosition(cameraX,
						   uiCam->pEasy["cameraY"] * pointsPerMeter,
						   cameraZ);
		
		cameraLook3d = ofVec3f(uiCam->pEasy["lookX"] * pointsPerMeter,
							   uiCam->pEasy["lookY"] * pointsPerMeter,
							   uiCam->pEasy["lookZ"] * pointsPerMeter);
		
		lookNode.setPosition(cameraLook3d);
		
//		cameraLookPosition3d = ofVec3f(uiCam->pEasy["lookX"] * pointsPerMeter,
//									   (uiCam->pEasy["lookY"]+100) * pointsPerMeter,
//									   uiCam->pEasy["lookZ"] * pointsPerMeter);
		
		cameraLookPosition3d = {0.f, -2*pointsPerMeter, 0.0f};
		
		camera3d.lookAt(lookNode);
		//camera3d.lookAt( cameraLook3d, cameraLookPosition3d );
	}




	camera3d.setFov(uiCam->pEasy["cameraFov"]);
	camera3d.begin();
	ofPushMatrix();
	
	
	
	float rotX = uiCam->pEasy["accelX"];
	float rotY = uiCam->pEasy["accelY"];
	float rotZ = uiCam->pEasy["accelZ"];
	float rotation = ofGetFrameNum()/9.0f;
	
	if (uiCam->pBool["rotCam"]) {
		uiCam->pFloat["rotX_accum"] += uiCam->pFloat["rotCamXAuto"];
		uiCam->pFloat["rotY_accum"] += uiCam->pFloat["rotCamYAuto"];
		uiCam->pFloat["rotZ_accum"] += uiCam->pFloat["rotCamZAuto"];
	}

	ofRotateXDeg(rotX + uiCam->pEasy["rotCamX"] + uiCam->pFloat["rotX_accum"]);
	ofRotateYDeg(rotY + uiCam->pEasy["rotCamY"] + uiCam->pFloat["rotY_accum"]);
	ofRotateZDeg(rotZ + uiCam->pEasy["rotCamZ"] + uiCam->pFloat["rotZ_accum"]);
}


//--------------------------------------------------------------
void beginCamPos_3d(ofVec3f camPos, ofVec3f lookAtPos) {
	
	if (uiCam->pBool["disableDepthTest"]) {
		ofDisableDepthTest();
	} else {
		ofEnableDepthTest();
	}
	
	float pointsPerMeter = 100.0;
	float cameraX = uiCam->pEasy["cameraX"] * pointsPerMeter;
	float cameraZ = uiCam->pEasy["cameraZ"] * pointsPerMeter;
	
	camera3d.setNearClip(1.0);
	camera3d.setFarClip(160 * pointsPerMeter);
	
	// 1.70, a altura de um adulto em p�
	
	if (uiCam->pBool["cameraPolar"]) {
		cameraX = r2x(uiCam->pEasy["cameraAngle"], uiCam->pEasy["cameraDist"] * pointsPerMeter);
		cameraZ = r2y(uiCam->pEasy["cameraAngle"], uiCam->pEasy["cameraDist"] * pointsPerMeter);
	}
	ofVec3f lookAtOrientation = lookAtPos;
	lookAtOrientation.z += 3*pointsPerMeter;

	camera3d.setPosition(camPos);
	camera3d.lookAt( lookAtPos, lookAtOrientation );
	camera3d.roll(uiCam->pFloat["roll"]);
	
	camera3d.setFov(uiCam->pEasy["cameraFov"]);
	camera3d.begin();
	ofPushMatrix();
}

//--------------------------------------------------------------
void endCam_3d() {
	ofPopMatrix();
	camera3d.end();
	ofDisableDepthTest();
}

//--------------------------------------------------------------
void beginCamera3d() {
	ofEnableDepthTest();
	beginCam_3d();
}

//--------------------------------------------------------------
void endCamera3d() {
	endCam_3d();
	ofDisableDepthTest();
}

//--------------------------------------------------------------
void dmtrCamUIEvent_3d(ofxMicroUI::element & e) {
	
	if (e.tag == "camNeue") {
		
		if (e.name == "resetLook") {
			//cout << "resetLook!" << endl;
			// TODO
//			uiCam->getSlider("lookX")->set(0.0);
//			uiCam->getSlider("lookY")->set(1.6);
//			uiCam->getSlider("lookZ")->set(0.0);
		}
		
		else if (e.name == "ortho") {
			if (e.b) {
				camera3d.enableOrtho();
			} else {
				camera3d.disableOrtho();
			}
		}
		
		else if (e.name == "resetAutoRotate") {
			uiCam->pFloat["rotX_accum"] = uiCam->pFloat["rotY_accum"] = uiCam->pFloat["rotZ_accum"] = 0;
			//uiCam->pEasy["rotX_accum"] = uiCam->pEasy["rotY_accum"] = uiCam->pEasy["rotZ_accum"] = 0;
		}

		else if (e.name == "resetCam") {
			//cout << "reset cam" << endl;
			uiCam->pFloat["rotX_accum"] = uiCam->pFloat["rotY_accum"] = uiCam->pFloat["rotZ_accum"] = 0;
			//uiCam->pEasy["rotX_accum"] = uiCam->pEasy["rotY_accum"] = uiCam->pEasy["rotZ_accum"] = 0;
	
			// TODO
//			uiCam->getElement("rotCamX")->set(float(0.0));
//			uiCam->getElement("rotCamY")->set(float(0.0));
//			uiCam->getElement("rotCamZ")->set(float(0.0));

//			uiCam->getElement("rotCamXAuto")->set(float(0.0));
//			uiCam->getElement("rotCamYAuto")->set(float(0.0));
//			uiCam->getElement("rotCamZAuto")->set(float(0.0));
		}

		else if (e.name == "shadeFlat") {
			if (e.b) {
				glShadeModel(GL_FLAT);
				glMatrixMode(GL_MODELVIEW);
				
			} else {
				glShadeModel(GL_SMOOTH);
				glMatrixMode(GL_MODELVIEW);
			}
		}
		
		if (e.tag == "light") {
			if (uiCam->pBool["enableSeparatapaecularLight"]) {
				ofEnableSeparateSpecularLight();
			} else {
				ofDisableSeparateSpecularLight();
			}
		}
	}
//	if (ofIsStringInString(e.name, "saveLoad")) {
//		vector <string> partes = ofSplitString(e.name, "_");
//		if (partes[2] == "0") {
//			uiCam->save("uiCam" + partes[1] + ".xml");
//		} else {
//			uiCam->load("uiCam" + partes[1] + ".xml");
//		}
//		//cout << e.name << endl;
//	}
}
