/**************************************************************************\
 * main.cpp - application's entry point                                   *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "core/utils/log.hpp"

#include "game/game_engine.hpp"


#include <iostream>
#include <exception>
#include <SDL2/SDL.h>

using namespace mo; // import game namespace

std::unique_ptr<mo::Game_engine> engine;

void init(int argc, char** argv, char** env);
void onFrame();
void shutdown();


#ifdef main
int main(int argc, char** argv) {
	char* noEnv = nullptr;
	char** env = &noEnv;
#else
int main(int argc, char** argv, char** env) {
#endif

	#ifdef __EMSCRIPTEN__
		init(argc, argv, env);

		EM_ASM(
			// Work-around chromium autoplay policy
			// https://github.com/emscripten-core/emscripten/issues/6511
			function resumeAudio(e) {
			if (typeof Module === 'undefined'
				|| typeof Module.SDL2 == 'undefined'
				|| typeof Module.SDL2.audioContext == 'undefined')
				return;
			if (Module.SDL2.audioContext.state == 'suspended') {
				Module.SDL2.audioContext.resume();
			}
			if (Module.SDL2.audioContext.state == 'running') {
				document.getElementById('canvas').removeEventListener('click', resumeAudio);
				document.removeEventListener('keydown', resumeAudio);
			}
			}
			document.getElementById('canvas').addEventListener('click', resumeAudio);
			document.addEventListener('keydown', resumeAudio);
		);

		emscripten_set_main_loop(onFrame, 0, 1);

		shutdown();
		emscripten_exit_with_live_runtime();
	#else
		init(argc, argv, env);

		while(engine->running())
			onFrame();

		shutdown();
	#endif

    return 0;
}

void init(int argc, char** argv, char** env) {
	INFO("Nothing to see here!");
	try {
		util::init_stacktrace(argv[0]);
		engine.reset(new mo::Game_engine("MagnumOpus", argc, argv, env));

	} catch (const util::Error& ex) {
		CRASH_REPORT("Exception in init: "<<ex.what());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Sorry :-(", "Error in init", nullptr);
		shutdown();
		exit(1);
	}
}

void onFrame() {
	try {
		engine->on_frame();

	} catch (const util::Error& ex) {
		CRASH_REPORT("Exception in onFrame: "<<ex.what());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Sorry :-(", "Error in onFrame", nullptr);
		shutdown();
		exit(2);
	}
}

void shutdown() {
	try {
		engine.reset();

	} catch (const util::Error& ex) {
		CRASH_REPORT("Exception in shutdown: "<<ex.what());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Sorry :-(", "Error in shutdown", nullptr);
		exit(3);
	}
}
