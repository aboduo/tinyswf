/******************************************************************************
Copyright (c) 2013 jbyu. All rights reserved.
******************************************************************************/
#ifndef __CCFLASH_H__
#define __CCFLASH_H__

#include "tinyswf.h"
#include "cocos2d.h"

tinyswf::Asset CCFlashLoadAsset( const char *name, const char *url );

class CCFlash : public cocos2d::LayerRGBA {
    typedef std::map<std::string, cocos2d::Texture2D*> FlashTextureCache;
    FlashTextureCache _textureCache;
	tinyswf::SWF *_swf;

public:
	virtual ~CCFlash();

    virtual void draw();

    virtual void update(float delta);

	virtual bool initWithFile(const char* filename, tinyswf::SWF::GetURLCallback fscommand);

	tinyswf::ICharacter* getCharacter(const char* name);

    virtual void registerWithTouchDispatcher();

    virtual bool ccTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void ccTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual void ccTouchCancelled(cocos2d::Touch *touch, cocos2d::Event* event) override;
    virtual void ccTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) override;

    virtual void setColor(const cocos2d::Color3B &color) override;
    virtual void setOpacity(GLubyte opacity) override;

    static CCFlash* create(const char* filename, tinyswf::SWF::GetURLCallback fscommand = NULL);

    cocos2d::Texture2D* getTexture(const char *filename , int &width, int&height, int&x, int&y);
};

class CCFlashRenderer : public tinyswf::Renderer {
    typedef std::map<std::string, tinyswf::SWF*> ImoprtSWFCache;
	ImoprtSWFCache _swfCache;

	cocos2d::GLProgram *_defaultShader;
	cocos2d::GLProgram *_textureShader;
	int _defaultColorLocation;
	int _addColorLocation;
	int _multColorLocation;
	int _textureMatrixLocation;
	int _maskLevel;
	kmMat4 _matrixMV;

public:
    CCFlashRenderer();
    virtual ~CCFlashRenderer();

	void maskBegin(void);
	void maskEnd(void);
	void maskOffBegin(void);
	void maskOffEnd(void);

    void applyTransform(const tinyswf::MATRIX3f& mtx );
    
	void drawLineStrip(const tinyswf::VertexArray& vertices, const tinyswf::CXFORM& cxform, const tinyswf::LineStyle& style);

	void drawTriangles(const tinyswf::VertexArray& vertices, const tinyswf::CXFORM& cxform, const tinyswf::FillStyle& style, const tinyswf::Asset& asset );

    void drawImportAsset( const tinyswf::RECT& rect, const tinyswf::CXFORM& cxform, const tinyswf::Asset& asset );
    
    void drawBegin(void);
    void drawEnd(void);

	tinyswf::SWF* importSWF(const char* filename);
};


#endif//__CCFLASH_H__