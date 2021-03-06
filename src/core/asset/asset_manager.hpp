/**************************************************************************\
 * interface for loading & writing asset & files                          *
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

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "../utils/maybe.hpp"
#include "../utils/template_utils.hpp"
#include "../utils/stacktrace.hpp"
#include "../utils/string_utils.hpp"

#include "aid.hpp"
#include "stream.hpp"

/**
 * void example(asset::Manager& assetMgr) {
 *		asset::Ptr<Texture> itemTex = assetMgr.load<Texture>("tex:items/health/small"_aid);
 * }
 */
namespace mo {
namespace asset {
	class Asset_manager;

	template<class R>
	class Ptr {
		public:
			Ptr();
			Ptr(Asset_manager& mgr, const AID& id, std::shared_ptr<const R> res=std::shared_ptr<const R>());

			bool operator==(const Ptr& o)const noexcept;
			bool operator<(const Ptr& o)const noexcept;

			auto operator*() -> const R&;
			auto operator*()const -> const R&;

			auto operator->() -> const R*;
			auto operator->()const -> const R*;

			operator bool()const noexcept {return !!_ptr;}
			operator std::shared_ptr<const R>();

			auto aid()const noexcept -> const AID& {return _aid;}
			auto mgr()const noexcept -> Asset_manager& {return *_mgr;}
			void load();
			void unload();

			void reset(){_ptr.reset();}

		private:
			Asset_manager* _mgr;
			std::shared_ptr<const R> _ptr;
			AID _aid;
	};


	class Asset_manager : util::no_copy_move {
		public:
			Asset_manager(const std::string& exe_name, const std::string& app_name);
			~Asset_manager();

			void shrink_to_fit()noexcept;

			template<typename T>
			auto load(const AID& id) -> Ptr<T>;

			template<typename T>
			auto load_maybe(const AID& id) -> util::maybe<Ptr<T>>;

			auto list(Asset_type type) -> std::vector<AID>;

			template<typename T>
			void save(const AID& id, const T& asset);

			bool exists(const AID& id)const noexcept;

			auto physical_location(const AID& id)const noexcept -> util::maybe<std::string>;

			void reload();

		private:
			using Reloader = void (*)(void*, istream);

			template<class T>
			static void _asset_reloader_impl(void* asset, istream in);

			struct Asset {
				std::shared_ptr<void> data;
				Reloader reloader;
				int64_t last_modified;

				Asset(std::shared_ptr<void> data, Reloader reloader, int64_t last_modified);
			};

			std::unordered_map<AID, Asset> _assets;
			std::unordered_map<AID, std::string> _dispatcher;

			void _add_asset(const AID& id, const std::string& path, Reloader reloader, std::shared_ptr<void> asset);

			auto _base_dir(Asset_type type)const -> util::maybe<std::string>;
			auto _open(const std::string& path) -> util::maybe<istream>;
			auto _open(const std::string& path, const AID& aid) -> util::maybe<istream>;
			auto _locate(const AID& id)const -> util::maybe<std::string>;

			auto _create(const AID& id) -> ostream;
			void _post_write();
	};

	template<class T>
	util::maybe<const T&> unpack(util::maybe<Ptr<T>> m) {
		return m.process(util::maybe<const T&>{}, [](Ptr<T>& p){return util::maybe<const T&>{*p};});
	}

} /* namespace asset */
}

#define ASSETMANAGER_INCLUDED
#include "asset_manager.hxx"
