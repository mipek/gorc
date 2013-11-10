#pragma once

#include "content/text_loader.h"
#include "content/assets/colormap.h"

namespace gorc {
namespace content {
namespace loaders {

class model_loader : public text_loader {
private:
	const assets::colormap& colormap;

public:
	static const std::vector<boost::filesystem::path> asset_root_path;

	model_loader(const assets::colormap& colormap);

	virtual std::unique_ptr<asset> parse(text::tokenizer& t, manager& manager, diagnostics::report& report) override;
};

}
}
}
