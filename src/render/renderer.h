#pragma once


//Base renderer class for testing various features
class Renderer {

public:

	virtual ~Renderer() {}

	virtual bool init() = 0;
	virtual void render() = 0;
	virtual void destroy() = 0;

};