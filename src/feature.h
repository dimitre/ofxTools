// using namespace std;
// #include "ofxMicroUI.h"
// #include "ofxMicroUISoftware.h"

struct microFeature {
public:

	ofxMicroUI * ui = NULL;
	ofxMicroUI * ui2 = NULL;
	ofxMicroUISoftware * soft = NULL;
    string name = "";
    
	virtual void begin() {
		cout << "begin in primitive feature" << endl;
	};
	virtual void end() {
		cout << "end in primitive feature" << endl;
	};
	virtual void setup() {
		cout << "setup in primitive feature" << endl;
	};
	virtual void update() {
		cout << "update in primitive feature" << endl;
	};
	virtual void uiEvents(ofxMicroUI::element & e) {
		cout << "uiEvents in primitive feature" << endl;
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

	microFeature(ofxMicroUI * _ui) : ui(_ui) {
        internalSetup();
		setup();
	}

	microFeature(ofxMicroUI * _ui, ofxMicroUISoftware * _soft) : ui(_ui), soft(_soft) {
        internalSetup();
		setup();
	}

	microFeature(ofxMicroUI * _ui, ofxMicroUI * _ui2, ofxMicroUISoftware * _soft) : ui(_ui), ui2(_ui2), soft(_soft) {
        internalSetup();
		setup();
	}
    
    microFeature(ofxMicroUI * _ui, ofxMicroUI * _ui2, ofxMicroUISoftware * _soft, string n) :
    ui(_ui), ui2(_ui2), soft(_soft), name(n) {
        internalSetup();
        setup();
    }
};

