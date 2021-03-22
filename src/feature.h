// using namespace std;
// #include "ofxMicroUI.h"
// #include "ofxMicroUISoftware.h"

struct microFeature {
public:

	ofxMicroUISoftware * soft = NULL;
	string name = "";
	ofxMicroUI * ui = NULL;
	ofxMicroUI * ui2 = NULL;
    
    bool * use = NULL;
    
	virtual void begin() {
		cout << "begin in primitive feature" << endl;
	};
	virtual void end() {
		cout << "end in primitive feature" << endl;
	};
	virtual void setup() {
		cout << "setup in primitive feature " << name << endl;
	};
	virtual void update() {
		cout << "update in primitive feature" << endl;
	};
    virtual void draw() {
        cout << "draw in primitive feature" << endl;
    };
    
    
	virtual void uiEvents(ofxMicroUI::element & e) {
//		cout << "uiEvents in primitive feature" << endl;
	}
    virtual void uiEventMaster(string & s) {
        if (s == "setup") {
//            cout << "|||||||||||||||| FEATURE " << name << " uiEventMaster : " << s <<  endl;
            setup();
        }
    }

    virtual bool isOk() {
        return use == NULL || *use;
    }
    
    void internalSetup() {
//        name = ui->uiName;
//        cout << "new feature with the name " << name << endl;
		if (ui != NULL) {
			ofAddListener(ui->uiEvent, this, &microFeature::uiEvents);
		}
        if (soft != NULL) {
            ofAddListener(soft->_ui->uiEventMaster, this, &microFeature::uiEventMaster);
        }
        if (name == "" && ui != NULL) {
            name = ui->uiName;
        }
    }
	
	bool isSetup = false;
	virtual void checkSetup() {
		if (!isSetup) {
			setup();
			isSetup = true;
		}
	}
	
	microFeature(ofxMicroUISoftware * _soft) : soft(_soft) {
        internalSetup();
	}
    
    microFeature(ofxMicroUISoftware * _soft, string n, bool * u = NULL) : soft(_soft), name(n), use(u) {
        internalSetup();
    }

	microFeature(string n, ofxMicroUI * _ui, bool * u = NULL) : name(n), ui(_ui), use(u) {
        internalSetup();
	}

	microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui) : soft(_soft), ui(_ui) {
        internalSetup();
	}

	microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui, ofxMicroUI * _ui2) : soft(_soft), ui(_ui), ui2(_ui2) {
        internalSetup();
	}
    
    microFeature(ofxMicroUISoftware * _soft, string n, ofxMicroUI * _ui, ofxMicroUI * _ui2 = NULL) :
    soft(_soft), name(n), ui(_ui), ui2(_ui2) {
        internalSetup();
    }
};





struct featurePolar : virtual public microFeature {
	using microFeature::microFeature;
	static float r2x (float a, float m) { return m * cos(ofDegToRad(a)); }
	static float r2y (float a, float m) { return m * sin(ofDegToRad(a)); }

	static float c2a (float x, float y) { return ofRadToDeg(atan2(y,x)); }
	static float c2m (float x, float y) { return sqrt(pow(x,2)+pow(y,2)); }
};



struct microFeatureBase : virtual public microFeature {
	using microFeature::microFeature;	
	void setup() override {}
	void begin() override {}
	void end() override {}
	
};
