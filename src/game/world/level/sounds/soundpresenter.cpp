#include "soundpresenter.h"
#include "game/world/level/levelmodel.h"
#include "game/components.h"
#include "content/manager.h"
#include "soundmodel.h"

Gorc::Game::World::Level::Sounds::SoundPresenter::SoundPresenter(Content::Manager& contentManager)
	: contentManager(contentManager), levelModel(nullptr), model(nullptr) {
	return;
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::Start(LevelModel& levelModel, SoundModel& soundModel) {
	this->levelModel = &levelModel;
	this->model = &soundModel;
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::Update(double dt) {
	// Update listener
	auto camera_position = levelModel->CameraPosition;
	auto listener_target = camera_position + levelModel->CameraLook;
	sf::Listener::SetTarget(Math::Get<0>(listener_target), Math::Get<1>(listener_target), Math::Get<2>(listener_target));
	sf::Listener::SetPosition(Math::Get<0>(camera_position), Math::Get<1>(camera_position), Math::Get<2>(camera_position));
	// TODO: Handle camera orientation (not currently properly implemented in SFML).
	const auto& player_sec = levelModel->Sectors[levelModel->CameraSector];
	SetAmbientSound(player_sec.AmbientSound, player_sec.AmbientSoundVolume);

	// Update sounds
	for(auto& sound : model->Sounds) {
		sound.Update(dt, *levelModel);
	}

	for(auto it = model->Sounds.begin(); it != model->Sounds.end(); ++it) {
		if(it->GetExpired()) {
			model->Sounds.Destroy(it.GetIndex());
		}
	}

	model->AmbientMusic.Update(dt);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::SetAmbientSound(Content::Assets::Sound const* sound, float volume) {
	if(&sound->Buffer == model->AmbientSound.GetBuffer() && model->AmbientSound.GetStatus() != sf::Sound::Stopped) {
		model->AmbientSound.SetVolume(volume * 100.0f);
	}
	else if(sound != nullptr) {
		model->AmbientSound.SetBuffer(sound->Buffer);
		model->AmbientSound.SetLoop(true);
		model->AmbientSound.SetVolume(volume * 100.0f);
		model->AmbientSound.SetPosition(0,0,0);
		model->AmbientSound.SetRelativeToListener(true);
		model->AmbientSound.SetAttenuation(0.0f);
		model->AmbientSound.Play();
	}
	else {
		model->AmbientSound.Stop();
	}
}

// Verbs:

void Gorc::Game::World::Level::Sounds::SoundPresenter::ChangeSoundPitch(int channel, float pitch, float delay) {
	Sound& sound = model->Sounds[channel];
	sound.SetPitch(pitch, delay);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::ChangeSoundVol(int channel, float volume, float delay) {
	Sound& sound = model->Sounds[channel];
	sound.SetVolume(volume, delay);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySong(int start, int end, int loopto) {
	model->AmbientMusic.PlaySong(start, end, loopto);
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundClass(int thing, Flags::SoundSubclassType subclass_type) {
	Thing& referenced_thing = levelModel->Things[thing];
	if(referenced_thing.SoundClass) {
		const Content::Assets::SoundSubclass& subclass = referenced_thing.SoundClass->Get(subclass_type);
		if(subclass.sound >= 0) {
			return PlaySoundThing(subclass.sound, thing, subclass.max_volume, subclass.min_radius, subclass.max_radius,
					subclass.flags + Flags::SoundFlag::ThingOriginMovesWithThing);
		}
	}

	return -1;
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::PlayFoleyLoopClass(int thing, Flags::SoundSubclassType subclass_type) {
	Thing& referenced_thing = levelModel->Things[thing];

	if(referenced_thing.CurrentFoleyLoopChannel >= 0) {
		model->Sounds[referenced_thing.CurrentFoleyLoopChannel].Stop();
	}

	int channel = PlaySoundClass(thing, subclass_type);
	if(channel >= 0) {
		referenced_thing.CurrentFoleyLoopChannel = channel;
	}
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::StopFoleyLoop(int thing) {
	Thing& referenced_thing = levelModel->Things[thing];

	if(referenced_thing.CurrentFoleyLoopChannel >= 0) {
		model->Sounds[referenced_thing.CurrentFoleyLoopChannel].Stop();
	}
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundLocal(int wav, float volume, float panning, FlagSet<Flags::SoundFlag> flags) {
	auto snd_tuple = model->Sounds.Create();

	Sound& snd = *std::get<0>(snd_tuple);
	snd.PlaySoundLocal(contentManager.GetAsset<Content::Assets::Sound>(wav), volume, panning, flags);

	return std::get<1>(snd_tuple);
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundPos(int wav, Math::Vector<3> pos, float volume, float minrad, float maxrad,
		FlagSet<Flags::SoundFlag> flags) {
	auto snd_tuple = model->Sounds.Create();

	Sound& snd = *std::get<0>(snd_tuple);
	snd.PlaySoundPos(contentManager.GetAsset<Content::Assets::Sound>(wav), pos, volume, minrad, maxrad, flags);

	return std::get<1>(snd_tuple);
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundThing(int wav, int thing, float volume, float minrad, float maxrad,
		FlagSet<Flags::SoundFlag> flags) {
	auto snd_tuple = model->Sounds.Create();

	Sound& snd = *std::get<0>(snd_tuple);
	snd.PlaySoundThing(*levelModel, contentManager.GetAsset<Content::Assets::Sound>(wav), thing, volume, minrad, maxrad, flags);

	return std::get<1>(snd_tuple);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::SetMusicVol(float volume) {
	model->AmbientMusic.SetVolume(volume);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::StopSound(int channel, float delay) {
	Sound& sound = model->Sounds[channel];
	sound.Stop(delay);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<void, 3>("changesoundpitch", [&components](int channel, float pitch, float delay) {
		components.CurrentLevelPresenter->SoundPresenter.ChangeSoundPitch(channel, pitch, delay);
	});

	verbTable.AddVerb<void, 3>("changesoundvol", [&components](int channel, float volume, float delay) {
		components.CurrentLevelPresenter->SoundPresenter.ChangeSoundVol(channel, volume, delay);
	});

	verbTable.AddVerb<void, 3>("playsong", [&components](int start, int end, int loopto) {
		components.CurrentLevelPresenter->SoundPresenter.PlaySong(start, end, loopto);
	});

	verbTable.AddVerb<int, 4>("playsoundlocal", [&components](int wav, float volume, float panning, int flags) {
		return components.CurrentLevelPresenter->SoundPresenter.PlaySoundLocal(wav, volume, panning, FlagSet<Flags::SoundFlag>(flags));
	});

	verbTable.AddVerb<int, 6>("playsoundpos", [&components](int wav, Math::Vector<3> pos, float volume, float min_rad, float max_rad, int flags) {
		return components.CurrentLevelPresenter->SoundPresenter.PlaySoundPos(wav, pos, volume, min_rad, max_rad, FlagSet<Flags::SoundFlag>(flags));
	});

	verbTable.AddVerb<int, 6>("playsoundthing", [&components](int wav, int thing, float volume, float min_rad, float max_rad, int flags) {
		return components.CurrentLevelPresenter->SoundPresenter.PlaySoundThing(wav, thing, volume, min_rad, max_rad, FlagSet<Flags::SoundFlag>(flags));
	});

	verbTable.AddVerb<void, 1>("setmusicvol", [&components](float vol) {
		components.CurrentLevelPresenter->SoundPresenter.SetMusicVol(vol);
	});

	verbTable.AddVerb<void, 2>("stopsound", [&components](int channel, float delay) {
		components.CurrentLevelPresenter->SoundPresenter.StopSound(channel, delay);
	});
}
