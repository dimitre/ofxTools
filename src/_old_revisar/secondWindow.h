//enum ofTargetPlatform{
//    OF_TARGET_OSX,
//    OF_TARGET_MINGW,
//    OF_TARGET_WINGCC,
//    OF_TARGET_WINVS,
//    OF_TARGET_IOS,
//    OF_TARGET_ANDROID,
//    OF_TARGET_LINUX,
//    OF_TARGET_LINUX64,
//    OF_TARGET_LINUXARMV6L, // arm v6 little endian
//    OF_TARGET_LINUXARMV7L, // arm v7 little endian
//    OF_TARGET_EMSCRIPTEN
//};

#ifdef WINDOWWIDTH
	int w = WINDOWWIDTH;
#else
	int w = 1680;
#endif

#ifdef WINDOWHEIGHT
int h = WINDOWHEIGHT;
#else
int h = 1100;
#endif

	//bool final = false;
#if defined( TARGET_OF_OSX )

	CGDisplayCount displayCount;
	CGDirectDisplayID displays[32];
	CGGetActiveDisplayList(32, displays, &displayCount);
	int numDisplays = displayCount;
	
	// se sao dois, ele pega o segundo, 0 e 1
	int whichDisplay = numDisplays-1;
	int displayHeight = CGDisplayPixelsHigh (displays[whichDisplay]);
	int displayWidth = CGDisplayPixelsWide (displays[whichDisplay]);
	CGRect displayBounds = CGDisplayBounds (displays[whichDisplay]);
	
	ofGLFWWindowSettings settings;
	settings.setSize(w, h);
	float offset = 0;
	settings.setPosition(ofVec2f(offset,offset));
	settings.windowMode = OF_WINDOW;
	auto guiWindow = ofCreateWindow(settings);
	auto mainApp = make_shared<ofApp>();
	
	//cout << numDisplays << endl;
	if (numDisplays > 1) {
		ofGLFWWindowSettings settings;
		settings.setSize(displayWidth, displayHeight);
		// SEGUNDA TELA, não precisa repetir os settings width/height pq eh fullHD tb.
		settings.setPosition(ofVec2f(displayBounds.origin.x, displayBounds.origin.y));
		settings.windowMode = OF_FULLSCREEN;
		settings.shareContextWith = guiWindow;
		shared_ptr<ofAppBaseWindow> secondWindow = ofCreateWindow(settings);
		ofAddListener(secondWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow);
	}
	
	//cout << "saidas todas" << endl;
	ofRunApp(guiWindow, mainApp);
	ofRunMainLoop();

#elseif defined( TARGET_OF_LINUX )


#else
	ofSetupOpenGL(w,h,OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new ofApp());
#endif
