shared_ptr<ofAppBaseWindow> newWindow;
vector <shared_ptr<ofAppBaseWindow> > allWindows;


//========================================================================
int main( ){
	#ifdef WINDOWWIDTH
	int w = WINDOWWIDTH;
	#else
	int w = 1920;
	#endif

	#ifdef WINDOWHEIGHT
	int h = WINDOWHEIGHT;
	#else
	int h = 1140;
	#endif

	// WINDOW GUI
	float offset = 80;
	ofGLFWWindowSettings settings;
	
	//
//    settings.setGLVersion(4, 1);
	
	settings.setSize(w, h);
	//settings.setPosition(ofVec2f(offset,offset));
	//settings.setGLVersion(3, 2);

	settings.windowMode = OF_WINDOW;
	auto guiWindow = ofCreateWindow(settings);
	//auto guiApp = make_shared<ofGuiApp>();
	auto mainApp = make_shared<ofApp>();

	CGDisplayCount displayCount;
	CGDirectDisplayID displays[32];
	CGGetActiveDisplayList(32, displays, &displayCount);
	int numDisplays = displayCount;

	for (int a=1; a<numDisplays; a++) {
		int whichDisplay = a;
		int displayHeight = CGDisplayPixelsHigh (displays[whichDisplay]);
		int displayWidth = CGDisplayPixelsWide (displays[whichDisplay]);
		CGRect displayBounds = CGDisplayBounds (displays[whichDisplay]);
		glm::vec2 pos = glm::vec2(displayBounds.origin.x, displayBounds.origin.y);
		settings.setSize(displayWidth, displayHeight);
		settings.setPosition(pos);
		settings.windowMode = OF_FULLSCREEN;
		settings.shareContextWith = guiWindow;
		
//		ofApp->wins.emplace_back(glm::vec2(displayWidth, displayHeight), pos);
		
		newWindow = ofCreateWindow(settings);

		if (a==1) {
			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow1);
		}
		else if (a==2) {
            ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow2);
		}
        else if (a==3) {
            ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow3);
        }
		else if (a==4) {
			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow4);
		}
//		else if (a==5) {
//			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow5);
//		}
//		else if (a==6) {
//			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow6);
//		}


		allWindows.push_back(move(newWindow));
	}

	ofRunApp(guiWindow, mainApp);
	ofRunMainLoop();

}
