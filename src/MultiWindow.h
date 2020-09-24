
shared_ptr<ofAppBaseWindow> newWindow;
vector <shared_ptr<ofAppBaseWindow> > allWindows;

#include "GLFW/glfw3.h"

//========================================================================
int main( ){
#ifndef WINDOWWIDTH
#define WINDOWWIDTH 1420
#endif

#ifndef WINDOWHEIGHT
#define WINDOWHEIGHT 840
#endif

	
//	ofTargetPlatform {
//	  OF_TARGET_OSX, OF_TARGET_WINGCC, OF_TARGET_WINVS, OF_TARGET_IOS,
//	  OF_TARGET_ANDROID, OF_TARGET_LINUX, OF_TARGET_LINUX64, OF_TARGET_LINUXARMV6L,
//	  OF_TARGET_LINUXARMV7L
//	}
	
#ifdef TARGET_OSX
	cout << "TARGET_OSX" << endl;
#endif
	
#ifdef TARGET_LINUX
	cout << "TARGET_LINUX" << endl;
#endif

#ifdef TARGET_LINUX64
	cout << "TARGET_LINUX64" << endl;
#endif


	// WINDOW GUI
	ofGLFWWindowSettings settings;
	settings.resizable = true;
	settings.setSize(WINDOWWIDTH, WINDOWHEIGHT);
	settings.windowMode = OF_WINDOW;

//	ofGLFWWindowSettings settings;
	// settings.numSamples = 8;
	//settings.setPosition(ofVec2f(80,80));
	//settings.setGLVersion(3, 2);

	auto guiWindow = ofCreateWindow(settings);
	auto mainApp = make_shared<ofApp>();

//#ifdef TARGET_LINUX
	// mainApp->gui = guiApp;
	// mainApp->window = mainWindow;
	// mainApp->gl = dynamic_pointer_cast<ofBaseGLRenderer>(mainWindow->renderer());
	
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    cout << "Screens Found: " << monitorCount << endl;
	for (int a=1; a<monitorCount; a++) {
        int xM; int yM;
        glfwGetMonitorPos(monitors[1], &xM, &yM); // We take the second monitor
        const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[1]);
		
		glm::vec2 size = glm::vec2(desktopMode->width, desktopMode->height);
		glm::vec2 pos = glm::vec2(xM, yM);
		cout << "display #" << a << endl;
		cout << "position: " << pos << endl;
		cout << "size:" << size << endl;
		cout << "-------" << endl;

		settings.setSize (size.x, size.y);
        settings.setPosition(pos);
		settings.windowMode = OF_FULLSCREEN;
		settings.shareContextWith = guiWindow;
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
		allWindows.emplace_back(newWindow);
//        return true;
    }
//#endif

//#ifdef TARGET_OSX
//	CGDisplayCount displayCount;
//	CGDirectDisplayID displays[32];
//	CGGetActiveDisplayList(32, displays, &displayCount);
//	int numDisplays = displayCount;
//	for (int a=1; a<numDisplays; a++) {
//		int displayHeight = CGDisplayPixelsHigh (displays[a]);
//		int displayWidth = CGDisplayPixelsWide (displays[a]);
//		CGRect displayBounds = CGDisplayBounds (displays[a]);
//		glm::vec2 pos = glm::vec2(displayBounds.origin.x, displayBounds.origin.y);
//		settings.setSize(displayWidth, displayHeight);
//		settings.setPosition(pos);
//		settings.windowMode = OF_FULLSCREEN;
//		settings.shareContextWith = guiWindow;
//
////		ofApp->wins.emplace_back(glm::vec2(displayWidth, displayHeight), pos);
//		newWindow = ofCreateWindow(settings);
////		ofAddListener(newWindow->events().draw, mainApp.get(), eventos[a]);
//
//		if (a==1) {
//			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow1);
//		}
//		else if (a==2) {
//			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow2);
//		}
//		else if (a==3) {
//			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow3);
//		}
//		else if (a==4) {
//			ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow4);
//		}
//		allWindows.emplace_back(newWindow);
//	}
//#endif
	
	ofRunApp(guiWindow, mainApp);
	ofRunMainLoop();
}