#pragma once

#include <functional>
#include <string>
#include <vector>

#include "base/Task.h"

#include "world/World.h"

namespace task {
namespace worldtests {

typedef std::function< std::string( world::World & world ) > world_test_t;
#define WT( ... ) [ __VA_ARGS__ ]( world::World & world ) -> std::string

#define WT_OK() {\
    return "";\
}

#define WT_FAIL( _text ) {\
    return (std::string)__FILE__ + ":" + std::to_string(__LINE__) + ": " + _text;\
}

#define WT_ASSERT( _condition, _text ) {\
    if ( !( _condition ) ) {\
        WT_FAIL("assertion failed [ " # _condition " ] " + _text);\
    }\
}

CLASS( WorldTests, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddTest( const std::string& name, const world_test_t test );

private:
	size_t current_test_index = 0;
	std::vector< std::pair< std::string, world_test_t >> m_tests = {};

	struct {
		size_t passed = 0;
		size_t failed = 0;
	} m_stats = {};
};

}
}
