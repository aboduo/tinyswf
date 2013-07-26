/******************************************************************************
Copyright (c) 2013 jbyu. All rights reserved.
******************************************************************************/
#ifndef __DEFINE_TEXT_H__
#define __DEFINE_TEXT_H__

#include "tsTag.h"

namespace tinyswf {

struct TextRecord {
	struct Glyph {
		int		_index;
		float	_advance;
	};

	struct Style {
		uint16_t _font_id;
		float _font_height;
		float _XOffset;
		float _YOffset;
		COLOR4f	_color;
		bool read(Reader& reader, int tag_type, int flag);
	};

	typedef std::vector<Glyph> GlyphArray;

	Style _style;
	GlyphArray _glyphs;

	bool read(Reader& reader, int tag_type, int flag, int glyph_bits, int advance_bits, Style& prev);
};

//-----------------------------------------------------------------------------

class DefineTextTag : public ITag {
typedef std::vector<TextRecord> TextArray;

	uint16_t	_character_id;
	RECT		_bound;
	MATRIX		_transform;
	TextArray	_texts;

public:
	DefineTextTag( TagHeader& h ) 
		:ITag( h )
	{}
		
	virtual ~DefineTextTag()
    {}
		
	virtual bool read( Reader& reader, SWF&, MovieFrames& data );
		
	virtual void print();
		
	static ITag* create( TagHeader& header ) {
		return new DefineTextTag( header );
	}				
};

//-----------------------------------------------------------------------------

class DefineEditTextTag : public ITag {
	friend class Text;
	uint16_t	_character_id;
	uint16_t	_font_id;
	uint16_t	_max_length;
	bool		_html;
	RECT		_bound;
	TextStyle	_style;
	std::string _font_class;
	std::string _variable_name;
	std::string _initial_text;

public:
	DefineEditTextTag( TagHeader& h ) 
		:ITag( h )
		,_character_id(0)
		,_font_id(0)
		,_max_length(0)
		,_html(false)
	{}
		
	virtual ~DefineEditTextTag()
    {}
		
	virtual bool read( Reader& reader, SWF&, MovieFrames& data );
		
	virtual void print();

	static ITag* create( TagHeader& header ) {
		return new DefineEditTextTag( header );
	}				
};

//-----------------------------------------------------------------------------

class Text : public ICharacter {
public:
	Text(const DefineEditTextTag&);
	virtual ~Text() {}

	// override ICharacter function
    virtual const RECT& getRectangle(void) const { return _bound; }
	virtual void draw(void);
	virtual void update(void) {}
	virtual ICharacter* getTopMost(float localX, float localY, bool polygonTest) { 
		SWF_UNUSED_PARAM(localX);
		SWF_UNUSED_PARAM(localY);
		SWF_UNUSED_PARAM(polygonTest);
		return NULL;
	}
	virtual void onEvent(Event::Code) {}

	bool setString(const char* str);
	
protected:
	// no copy constructor and assignment operator
	Text& operator=(const Text&);
	Text(const Text&);

	const DefineEditTextTag& _reference;
	uint32_t	_glyphs;
	RECT		_bound;
	TextStyle	_style;
	VertexArray _vertices;
	std::wstring _text;
};
	

}//namespace
#endif//__DEFINE_TEXT_H__