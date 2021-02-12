struct feature {
public:
	ofxMicroUI * _ui = NULL;
	virtual void begin() {};
	virtual void end() {};
	virtual void setup() {};
	feature();
	feature(ofxMicroUI * ui) : _ui(ui) {
		// setup();
	}
};

struct featureCam : public feature {
public:
//	using feature::feature;
//	featureCam(ofxMicroUI * ui) : _ui(ui) { }
	
	featureCam(ofxMicroUI * ui) : feature(ui) { }
	

//	ofEasyCam camera3d;
//	glm::vec3 cameraLook3d = { 0.0f, 0.0f, 0.0f };
//	glm::vec3 cameraLookUp = { 0.f, -1.0f, 0.0f };
//	glm::vec3 cameraLookUp2 = { 0.f, 1.0f, 0.0f };
//	float pointsPerMeter = 100.0;
//	ofNode lookNode;
	
//	void setup() override {
//		camera3d.setNearClip(0.01);
//		camera3d.setFarClip(160 * pointsPerMeter);
//	}
	
//	void begin() override {
//		if (_ui->pBool["disableDepthTest"]) {
//			ofDisableDepthTest();
//		} else {
//			ofEnableDepthTest();
//		}
//		float cameraX = _ui->pEasy["cameraX"];
//		float cameraZ = _ui->pEasy["cameraZ"];
//
//		// 1.70, a altura de um adulto em p�
//		if (_ui->pBool["cameraPolar"]) {
//			float a = _ui->pEasy["cameraAngle"] + 90.0;
//			float d = _ui->pEasy["cameraDist"] * pointsPerMeter;
//			cameraX = r2x(a, d);
//			cameraZ = r2y(a, d);
//			cameraX += lookNode.getPosition().x;
//			cameraZ += lookNode.getPosition().z;
//		}
//
//
//		camera3d.setFov(_ui->pEasy["cameraFov"]);
//		camera3d.begin();
//		ofPushMatrix();
//
//		float rotX = _ui->pEasy["accelX"];
//		float rotY = _ui->pEasy["accelY"];
//		float rotZ = _ui->pEasy["accelZ"];
//
//		if (_ui->pBool["rotCam"]) {
//			_ui->pFloat["rotX_accum"] += _ui->pFloat["rotCamXAuto"];
//			_ui->pFloat["rotY_accum"] += _ui->pFloat["rotCamYAuto"];
//			_ui->pFloat["rotZ_accum"] += _ui->pFloat["rotCamZAuto"];
//		}
//
//		ofRotateXDeg(rotX + _ui->pEasy["rotCamX"] + _ui->pFloat["rotX_accum"]);
//		ofRotateYDeg(rotY + _ui->pEasy["rotCamY"] + _ui->pFloat["rotY_accum"]);
//		ofRotateZDeg(rotZ + _ui->pEasy["rotCamZ"] + _ui->pFloat["rotZ_accum"]);
//	}
};

//featureCam cam(&u.uis["cam"]);

ofxMicroUI * uiCam = &u.uis["cam"];
feature ft = feature(&u.uis["cam"]);
featureCam cam = featureCam(&u.uis["cam"]);
