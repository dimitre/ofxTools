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
	virtual void uiEvents(ofxMicroUI::element & e) {
		cout << "uiEvents in primitive feature" << endl;
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
		setup();
	}
	
	microFeature(ofxMicroUISoftware * _soft, string n) : soft(_soft), name(n) {
        internalSetup();
		setup();
	}
    
    microFeature(ofxMicroUISoftware * _soft, string n, bool * u = NULL) : soft(_soft), name(n), use(u) {
        internalSetup();
        setup();
    }

	microFeature(ofxMicroUI * _ui, bool * u = NULL) : ui(_ui), use(u) {
        internalSetup();
		setup();
	}

	microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui) : soft(_soft), ui(_ui) {
        internalSetup();
		setup();
	}

	microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui, ofxMicroUI * _ui2) : soft(_soft), ui(_ui), ui2(_ui2) {
        internalSetup();
		setup();
	}
    
    microFeature(ofxMicroUISoftware * _soft, string n, ofxMicroUI * _ui, ofxMicroUI * _ui2 = NULL) :
    soft(_soft), name(n), ui(_ui), ui2(_ui2) {
        internalSetup();
        setup();
    }
};

