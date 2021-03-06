#include <Base/Log.h>
#include <Base/Debug.h>
#include <Base/SDLHelper.h>
#include <Render/GLHelper.h>
#include <Engine/Core.h>

Core::Core(): _running(false), _renderContext(0), _elapsedIndex(0) {
    setup();
}

Core::~Core() {
    teardown();
}

void Core::setup() {
    // Set up FPS sampling
    for(int i = 0; i < FPS_WINDOW_SIZE; i++) {
        _elapsedSamples[i] = 0;
    }
    
    _core = this;
    
    Log::Setup();
    Log::EnableAllChannels();

    _window = new Window("GhastlyWindow");

    _viewport = new Viewport();
    
	_renderContext = new RenderContext(_window->getSDLWindow());
	_renderContext->setViewport(_viewport);
    
    _eventHandler = new EventHandler(_window->getID());
    _eventHandler->addWindowListener(this);
    _eventHandler->addKeyboardListener(this);
    _eventHandler->addMouseListener(this);
	
	// TODO - Set this with an option
	_window->resize(640, 480);
	_window->swapBuffers();
}

void Core::teardown() {
    ParentState::teardown();

    if(_eventHandler) {
        delete _eventHandler;
        _eventHandler = 0;
    }
    if(_renderContext) {
        delete _renderContext;
        _renderContext = 0;
    }
    if(_viewport) {
        delete _viewport;
        _viewport = 0;
    }
    if(_window) {
        delete _window;
        _window = 0;
    }
    
    Log::Teardown();
}

void Core::start() {
    int lastTime, elapsedTime;

    lastTime = getTime();
    _running = true;

    Info("Main loop starting.");
    while(_running) {
        int currentTime = getTime();
        elapsedTime = currentTime - lastTime;

        //Info("FPS: " << trackFPS(elapsedTime) << "(" << elapsedTime << ")");
        
        _eventHandler->handleEvents();

        update(elapsedTime);
        _renderContext->clear();
        render(_renderContext);
        _window->swapBuffers();

        //CheckSDLErrors();
        CheckGLErrors();
        
        lastTime = currentTime;
    }

    // Cleanup
    Info("Main loop halted, exiting.");
}

void Core::stop() {
    _running = false;
}

void Core::resizeWindow(const int w, const int h) {
	// Resize the viewport
    _viewport->resize(0, 0, w, h);
    _renderContext->setViewport(_viewport);
}

void Core::closeWindow() {
    stop();
    flushStates();
}

int Core::getTime() {
    return SDL_GetTicks();
}

Viewport* Core::getViewport() const {
    return _viewport;
}

float Core::trackFPS(int elapsed) {
    int sum;
    int i;
    
    _elapsedSamples[_elapsedIndex] = elapsed;
    _elapsedIndex++;
    if(_elapsedIndex > FPS_WINDOW_SIZE) {
        _elapsedIndex = 0;
    }
    
    sum = 0;
    for(i = 0; i < FPS_WINDOW_SIZE; i++) {
        sum += _elapsedSamples[i];
    }

    return (1000 * FPS_WINDOW_SIZE) / (float)sum;
}
