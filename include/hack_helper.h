#pragma once
#include "my_windows/my_windows.h"

namespace vst {

	enum class game_title
	{
		vestaria_1,
		vestaria_ep2
	};

	class hack_helper
	{
	public:
		static hack_helper& instance();

		hack_helper(const hack_helper&) = delete;
		hack_helper(hack_helper&&) = delete;
		hack_helper& operator=(const hack_helper&) = delete;
		hack_helper& operator=(hack_helper&&) = delete;

	public:
		bool find_game(game_title);

		bool is_game_found()
		{
			return is_game_found_;
		}

		bool write_skill(int val);

		bool write_exp(int val);

		bool write_money(int money);

		static const TCHAR* get_game_title_enum_str(game_title);

	private:
		hack_helper(){}

		LPVOID first_();

		LPVOID next_(LPVOID address);

		LPVOID get_next_pointer_(LPVOID address, int offset);

		LPVOID get_game_base_address_();



	private:
		bool is_game_found_ = false;

		HANDLE process_handle_ = nullptr;


	};

};



