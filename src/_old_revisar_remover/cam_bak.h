#include "polar.h"

#ifndef	HASAUDIO
float beat = 0.0;
#endif

ofLight luzinha3d;
ofMaterial material3d;

//	float pointsPerMeter = 100.0;
const float pointsPerMeter = 1.0;
//ofCamera camera3d;
ofEasyCam camera3d;
glm::vec3 cameraLook3d;
glm::vec3 cameraLookUp = { 0.f, 1.0f, 0.0f };


ofNode lookNode;

ofxMicroUI *uiCam = &u.uis["cam"];
ofxMicroUI *uiLuz = &u.uis["luz"];
ofxMicroUI *uiMaterial = &u.uis["material"];


// precisa vir antes do loadMaster pra poder fazer os presets antes.
void setupCam_3d() {
	ofAddListener(uiCam->uiEvent, this, &ofApp::dmtrCamUIEvent_3d);
	// uiCam->createFromLines(s);
//	uiLuz->createFromLines(s);
//	uiMaterial->createFromLines(s);
	
//	luzinha3d.setPointLight();

//	camera3d.enableInertia();

}

struct luzUI {
public:
	ofxMicroUI * ui = NULL;
	ofLight luz;
	glm::vec3 pos;
	
	luzUI(ofxMicroUI * _ui) : ui(_ui) {
		luz.setAreaLight(10, 10);
	}
	void begin() {
		if (ui->pBool["on"]) {
			luz.setAttenuation(
				ui->pEasy["lightAttenuationConstant"],
				ui->pEasy["lightAttenuationLinear"],
				ui->pEasy["lightAttenuationQuadratic"]
			);
			
			
			
			pos = glm::vec3(
				ui->pEasy["luzX"],
				ui->pEasy["luzY"],
				ui->pEasy["luzZ"]
			);
			
			ofSetColor(ui->pColor["diffuseColorLuz"]);
			ofDrawBox(pos, 1.0);
			
			luz.setPosition(pos);
			
			luz.setDiffuseColor( ui->pColor["diffuseColorLuz"]);
			luz.setSpecularColor( ui->pColor["specularColorLuz"]);
			luz.setAmbientColor( ui->pColor["ambientColorLuz"]);
			luz.enable();



		}
	}
	
	void end() {
		if (ui->pBool["on"]) {
			luz.disable();
		}
	}
};

luzUI luzes[3] = {
	luzUI(&u.uis["luz0"]),
	luzUI(&u.uis["luz1"]),
	luzUI(&u.uis["luz2"])
};

void beginLuzes() {
	for (auto & l : luzes) {
		l.begin();
	}
}

void endLuzes() {
	for (auto & l : luzes) {
		l.end();
	}
}
	
//--------------------------------------------------------------
void beginIlumina_3d(){
	if (uiLuz->pBool["Lighting"]) {
		ofEnableLighting();
		
		if (uiLuz->pBool["on"]) {
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
		
		beginLuzes();
	}
}

//--------------------------------------------------------------
void endIlumina_3d(){
	if (uiLuz->pBool["Lighting"]) {
		luzinha3d.disable();
		ofDisableLighting();
		ofDisableSeparateSpecularLight();
		
		endLuzes();
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
		
		cameraLook3d = glm::vec3(uiCam->pEasy["lookX"] * pointsPerMeter,
							   uiCam->pEasy["lookY"] * pointsPerMeter,
							   uiCam->pEasy["lookZ"] * pointsPerMeter);
		
		lookNode.setPosition(cameraLook3d);
		
//		cameraLookPosition3d = glm::vec3(uiCam->pEasy["lookX"] * pointsPerMeter,
//									   (uiCam->pEasy["lookY"]+100) * pointsPerMeter,
//									   uiCam->pEasy["lookZ"] * pointsPerMeter);
		
		camera3d.lookAt(lookNode, cameraLookUp);
		//camera3d.lookAt( cameraLook3d, cameraLookPosition3d );
	}

	camera3d.setFov(uiCam->pEasy["cameraFov"]);
	camera3d.begin();
	ofPushMatrix();
	
	float rotX = uiCam->pEasy["accelX"];
	float rotY = uiCam->pEasy["accelY"];
	float rotZ = uiCam->pEasy["accelZ"];
	// float rotation = ofGetFrameNum()/9.0f;
	
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
void beginCamPos_3d(glm::vec3 camPos, glm::vec3 lookAtPos) {
	
	if (uiCam->pBool["disableDepthTest"]) {
		ofDisableDepthTest();
	} else {
		ofEnableDepthTest();
	}
	
	float cameraX = uiCam->pEasy["cameraX"] * pointsPerMeter;
	float cameraZ = uiCam->pEasy["cameraZ"] * pointsPerMeter;
	
	camera3d.setNearClip(1.0);
	camera3d.setFarClip(160 * pointsPerMeter);
	
	// 1.70, a altura de um adulto em p�
	
	if (uiCam->pBool["cameraPolar"]) {
		cameraX = r2x(uiCam->pEasy["cameraAngle"], uiCam->pEasy["cameraDist"] * pointsPerMeter);
		cameraZ = r2y(uiCam->pEasy["cameraAngle"], uiCam->pEasy["cameraDist"] * pointsPerMeter);
	}
	glm::vec3 lookAtOrientation = lookAtPos;
	lookAtOrientation.z += 3*pointsPerMeter;

	camera3d.setPosition(camPos);
	camera3d.lookAt( lookAtPos, lookAtOrientation );
	camera3d.rollDeg(uiCam->pFloat["roll"]);
	
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

////--------------------------------------------------------------
//void beginCamera3d() {
//	ofEnableDepthTest();
//	beginCam_3d();
//}
//
////--------------------------------------------------------------
//void endCamera3d() {
//	endCam_3d();
//	ofDisableDepthTest();
//}

//--------------------------------------------------------------
void dmtrCamUIEvent_3d(ofxMicroUI::element & e) {
	if (!e._settings->presetIsLoading) {

//			cout << e.name << endl;
		if (e.name == "resetLook") {
			uiCam->getSlider("lookX")->set(0.0);
//			uiCam->getSlider("lookY")->set(1.6);
			uiCam->getSlider("lookY")->set(0.0);
			uiCam->getSlider("lookZ")->set(0.0);
		}
		
		else if (e.name == "resetAutoRotate") {
			uiCam->pFloat["rotX_accum"] = uiCam->pFloat["rotY_accum"] = uiCam->pFloat["rotZ_accum"] = 0;
			//uiCam->pEasy["rotX_accum"] = uiCam->pEasy["rotY_accum"] = uiCam->pEasy["rotZ_accum"] = 0;
		}

		else if (e.name == "resetCam") {
			uiCam->pFloat["rotX_accum"] = uiCam->pFloat["rotY_accum"] = uiCam->pFloat["rotZ_accum"] = 0;
			//uiCam->pEasy["rotX_accum"] = uiCam->pEasy["rotY_accum"] = uiCam->pEasy["rotZ_accum"] = 0;

			uiCam->getSlider("rotCamX")->set(0);
			uiCam->getSlider("rotCamY")->set(0);
			uiCam->getSlider("rotCamZ")->set(0);

			uiCam->getSlider("rotCamXAuto")->set(0);
			uiCam->getSlider("rotCamYAuto")->set(0);
			uiCam->getSlider("rotCamZAuto")->set(0);
		}
	}
		
	
	if (e.name == "ortho") {
		if (*e.b) {
			camera3d.enableOrtho();
		} else {
			camera3d.disableOrtho();
		}
	}
	
	else if (e.name == "shadeFlat") {
		if (*e.b) {
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
