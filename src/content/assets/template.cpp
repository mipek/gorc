#include "template.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "content/manager.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace Gorc {
namespace Content {
namespace Assets {

static const std::unordered_map<std::string, Flags::ThingType> TemplateTypeMap {
	{ "actor", Flags::ThingType::Actor },
	{ "weapon", Flags::ThingType::Weapon },
	{ "debris", Flags::ThingType::Debris },
	{ "item", Flags::ThingType::Item },
	{ "explosion", Flags::ThingType::Explosion },
	{ "cog", Flags::ThingType::Cog },
	{ "ghost", Flags::ThingType::Ghost },
	{ "corpse", Flags::ThingType::Corpse },
	{ "player", Flags::ThingType::Player },
	{ "particle", Flags::ThingType::Particle }
};

static const std::unordered_map<std::string, Flags::MoveType> MoveTypeMap {
	{ "none", Flags::MoveType::None },
	{ "physics", Flags::MoveType::Physics },
	{ "path", Flags::MoveType::Path }
};

using TemplateParameterParser = std::function<void(Template&, Text::Tokenizer&, Content::Manager&, const Colormap&, const Cog::Compiler&, Diagnostics::Report&)>;

template <typename T> void TemplateParameterValueMapper(const std::unordered_map<std::string, T>& map, T& value,
		const T& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	std::string key = tok.GetSpaceDelimitedString();
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	auto it = map.find(key);
	if(it == map.end()) {
		report.AddWarning("Template::ParseArgs::TemplateParameterValueMapper", boost::str(boost::format("unknown parameter value \'%s\'") % key),
				tok.GetInternalTokenLocation());
		value = defaultValue;
	}
	else {
		value = it->second;
	}
}

template <typename T> void TemplateParameterNumberMapper(T& value, const T& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	Text::Token t;
	tok.GetToken(t);

	if(t.Type == Text::TokenType::Float || t.Type == Text::TokenType::Integer || t.Type == Text::TokenType::HexInteger) {
		value = t.GetNumericValue<T>();
	}
	else {
		report.AddWarning("Template::ParseArgs::TemplateParameterNumberMapper", "expected numeric value", t.Location);
		value = defaultValue;
	}
}

template <typename T> void TemplateParameterEnumMapper(T& value, const T& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	Text::Token t;
	tok.GetToken(t);

	if(t.Type == Text::TokenType::Float || t.Type == Text::TokenType::Integer || t.Type == Text::TokenType::HexInteger) {
		value = static_cast<T>(t.GetNumericValue<typename std::underlying_type<T>::type>());
	}
	else {
		report.AddWarning("Template::ParseArgs::TemplateParameterEnumMapper", "expected numeric value", t.Location);
		value = defaultValue;
	}
}

template <typename T> void TemplateParameterFlagMapper(FlagSet<T>& value, const FlagSet<T>& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	Text::Token t;
	tok.GetToken(t);

	if(t.Type == Text::TokenType::Float || t.Type == Text::TokenType::Integer || t.Type == Text::TokenType::HexInteger) {
		value = FlagSet<T>(t.GetNumericValue<typename std::underlying_type<T>::type>());
	}
	else {
		report.AddWarning("Template::ParseArgs::TemplateParameterFlagMapper", "expected numeric value", t.Location);
		value = defaultValue;
	}
}

void TemplateParameterVectorMapper(Math::Vector<3>& value, Text::Tokenizer& tok) {
	tok.AssertPunctuator("(");
	Math::Get<0>(value) = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	Math::Get<1>(value) = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	Math::Get<2>(value) = tok.GetNumber<float>();
	tok.AssertPunctuator(")");
}

void TemplateModel3DParser(Template& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Colormap& cmp, const Cog::Compiler&, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.Model3d = nullptr;
	}
	else {
		try {
			tpl.Model3d = &manager.Load<Model>(fn, cmp);
		}
		catch(...) {
			tpl.Model3d = nullptr;
		}
	}
}

void TemplateSoundClassParser(Template& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Colormap&, const Cog::Compiler&, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.SoundClass = nullptr;
	}
	else {
		try {
			tpl.SoundClass = &manager.Load<SoundClass>(fn);
		}
		catch(...) {
			tpl.SoundClass = nullptr;
		}
	}
}

void TemplateCogParser(Template& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Colormap&, const Cog::Compiler& compiler, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.Cog = nullptr;
	}
	else {
		try {
			tpl.Cog = &manager.Load<Script>(fn, compiler);
		}
		catch(...) {
			tpl.Cog = nullptr;
		}
	}
}

void TemplatePuppetParser(Template& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Colormap&, const Cog::Compiler& compiler, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.Puppet = nullptr;
	}
	else {
		try {
			tpl.Puppet = &manager.Load<Puppet>(fn);
		}
		catch(...) {
			tpl.Puppet = nullptr;
		}
	}
}

void TemplateAddFrame(Template& tpl, Text::Tokenizer& tok) {
	tok.AssertPunctuator("(");
	float x = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	float y = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	float z = tok.GetNumber<float>();

	Text::Token t;
	tok.GetToken(t);

	if(t.Value == ":") {
		float p = tok.GetNumber<float>();
		tok.AssertPunctuator("/");
		float a = tok.GetNumber<float>();
		tok.AssertPunctuator("/");
		float r = tok.GetNumber<float>();
		tok.AssertPunctuator(")");

		tpl.Frames.emplace_back(Math::Vec(x, y, z), Math::Vec(p, a, r));
	}
	else {
		tpl.Frames.emplace_back(Math::Vec(x, y, z), Math::Zero<3>());
	}
}

#define TPP_ARGS Template& tpl, Text::Tokenizer& tok, Content::Manager& content, const Colormap& colormap, const Cog::Compiler& compiler, Diagnostics::Report& report

static const std::unordered_map<std::string, TemplateParameterParser> TemplateParameterParserMap {
	{ "actorflags", [](TPP_ARGS) { TemplateParameterFlagMapper(tpl.ActorFlags, FlagSet<Flags::ActorFlag>(), tok, report); }},
	{ "cog", &TemplateCogParser },
	{ "collide", [](TPP_ARGS) { TemplateParameterEnumMapper(tpl.Collide, Flags::CollideType::None, tok, report); }},
	{ "eyeoffset", [](TPP_ARGS) { TemplateParameterVectorMapper(tpl.EyeOffset, tok); }},
	{ "frame", [](TPP_ARGS) { TemplateAddFrame(tpl, tok); }},
	{ "health", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.Health, 100.0f, tok, report); }},
	{ "height", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.Height, 0.18f, tok, report); }},
	{ "light", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.Light, 0.0f, tok, report); }},
	{ "lightintensity", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.LightIntensity, 0.0f, tok, report); }},
	{ "lightoffset", [](TPP_ARGS) { TemplateParameterVectorMapper(tpl.LightOffset, tok); }},
	{ "mass", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.Mass, 2.0f, tok, report); }},
	{ "maxhealth", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.MaxHealth, 100.0f, tok, report); }},
	{ "maxlight", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.Light, 1.0f, tok, report); }},
	{ "model3d", &TemplateModel3DParser },
	{ "move", [](TPP_ARGS) { TemplateParameterValueMapper(MoveTypeMap, tpl.Move, Flags::MoveType::None, tok, report); }},
	{ "movesize", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.MoveSize, 0.05f, tok, report); }},
	{ "numframes", [](TPP_ARGS) { /* Silently consume numframes */ tok.GetNumber<int>(); }},
	{ "physflags", [](TPP_ARGS) { TemplateParameterFlagMapper(tpl.PhysicsFlags, FlagSet<Flags::PhysicsFlag>(), tok, report); }},
	{ "puppet", &TemplatePuppetParser },
	{ "size", [](TPP_ARGS) { TemplateParameterNumberMapper(tpl.Size, 0.05f, tok, report); }},
	{ "soundclass", &TemplateSoundClassParser },
	{ "thingflags", [](TPP_ARGS) { TemplateParameterFlagMapper(tpl.Flags, FlagSet<Flags::ThingFlag>(), tok, report); }},
	{ "type", [](TPP_ARGS) { TemplateParameterValueMapper(TemplateTypeMap, tpl.Type, Flags::ThingType::Free, tok, report); }}
};

#undef TPP_ARGS

}
}
}

void Gorc::Content::Assets::Template::ParseArgs(Text::Tokenizer& tok, Content::Manager& manager, const Colormap& cmp,
		const Cog::Compiler& compiler, Diagnostics::Report& report) {
	bool oldReportEOL = tok.GetReportEOL();
	tok.SetReportEOL(true);

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile || t.Type == Text::TokenType::EndOfLine) {
			break;
		}
		else {
			tok.AssertPunctuator("=");

			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = TemplateParameterParserMap.find(t.Value);
			if(it == TemplateParameterParserMap.end()) {
				report.AddWarning("Template::ParseArgs", boost::str(boost::format("unrecognized template param \'%s\'") % t.Value), t.Location);
				tok.GetSpaceDelimitedString();
			}
			else {
				it->second(*this, tok, manager, cmp, compiler, report);
			}
		}
	}

	tok.SetReportEOL(oldReportEOL);
}
