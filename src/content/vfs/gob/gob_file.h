#pragma once

#include "framework/io/read_only_file.h"
#include "framework/io/native_file.h"
#include <boost/filesystem/path.hpp>
#include <string>

namespace gorc {
namespace content {
namespace vfs {
namespace gob {

class gob_file : public io::read_only_file {
private:
	io::native_file file;
	const size_t chunkOffset;
	const size_t chunkLength;
	const size_t chunkEnd;

public:
	gob_file(const boost::filesystem::path& gobPath, const boost::filesystem::path& filename, size_t chunkOffset, size_t chunkLength);

	virtual void read(void* dest, size_t size) override;

	virtual void seek(long offset) override;
	virtual void set_position(size_t offset) override;
	virtual size_t get_position() override;

	virtual size_t get_size() override;
	virtual bool is_end_of_file() override;
};

}
}
}
}
