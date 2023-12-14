#pragma once

#include <vector>

#include "base/Base.h"

#include "gse/program/Program.h"
#include "gse/program/Scope.h"
#include "gse/program/Statement.h"

namespace gse {
namespace parser {

CLASS( Parser, base::Base )

	Parser( const std::string& source );

	const program::Program* Parse();

protected:

	const std::string CHARS_EOLN = "\r\n";
	const std::string CHARS_NUMBERS = "0123456789";
	const std::string CHARS_NUMBERS_C = CHARS_NUMBERS + ".";
	const std::string CHARS_LETTERS_LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
	const std::string CHARS_LETTERS_UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string CHARS_LETTERS = CHARS_LETTERS_LOWERCASE + CHARS_LETTERS_UPPERCASE;

	class SourceElement {
	public:
		enum element_type_t {
			ET_LINE_COMMENT,
			ET_MULTILINE_COMMENT,
			ET_IDENTIFIER,
			ET_OPERATOR,
			ET_CONTROL,
			ET_BLOCK,
		};
		SourceElement( const element_type_t type )
			: m_type( type ) {}
		const element_type_t m_type;
	};

	class Comment : public SourceElement {
	public:
		Comment( bool is_multiline, const std::string& text )
			: SourceElement(
			is_multiline
				? ET_MULTILINE_COMMENT
				: ET_LINE_COMMENT
		)
			, m_is_multiline( is_multiline )
			, m_text( text ) {}
		const bool m_is_multiline;
		const std::string m_text;
	};

	class Identifier : public SourceElement {
	public:
		Identifier( const std::string& name, const uint8_t identifier_type )
			: SourceElement( ET_IDENTIFIER )
			, m_name( name )
			, m_identifier_type( identifier_type ) {}
		const std::string m_name;
		const uint8_t m_identifier_type;
	};

	class Operator : public SourceElement {
	public:
		Operator( const std::string& op )
			: SourceElement( ET_OPERATOR )
			, m_op( op ) {}
		const std::string m_op;
	};

	class Control : public SourceElement {
	public:
		enum control_type_t {
			CT_FLOW_DELIMITER,
			CT_DATA_DELIMITER,
		};
		Control( const control_type_t type )
			: SourceElement( ET_CONTROL )
			, m_type( type ) {};
		const control_type_t m_type;
	};

	class Block : public SourceElement {
	public:
		enum block_side_t {
			BS_BEGIN,
			BS_END,
		};
		Block( const uint8_t block_type, const block_side_t block_side )
			: SourceElement( ET_BLOCK )
			, m_block_type( block_type )
			, m_block_side( block_side ) {}
		const block_side_t m_block_side;
		const uint8_t m_block_type;
	};

	virtual void GetElements( std::vector< SourceElement* >& elements ) = 0;
	virtual const program::Program* GetProgram( const std::vector< SourceElement* >& elements ) = 0;

	const char get() const; // get character at current position
	const bool eof() const; // returns true if source is parsed to the end
	
	// read and until end character encountered
	const std::string read_until_char( char chr, bool consume );
	// read until any of end characters encountered
	const std::string read_until_char_any( const char* chrs, bool consume );
	// read until end sequence encountered
	const std::string read_until_sequence( const char* sequence, bool consume );
	// read while any of characters match
	const std::string read_while_char_any( const char* chrs );
	// skip while any of characters match
	void skip_while_char_any( const char* chrs );

	// check if character occurs at current position
	const bool match_char( const char chr, bool consume );
	// check if any of characters occurs at current position
	const char match_char_any( const char* chrs, bool consume );
	// check if sequence occurs at current position
	const bool match_sequence( const char* sequence, bool consume );

private:
	const std::string m_source;

	const char* const m_begin;
	const char* const m_end;
	const char* m_ptr = nullptr;

};

}
}
