#pragma once
#include "my_windows/my_windows.h"

namespace vst {

	class hack_helper
	{
	public:
		static hack_helper& instance();

		hack_helper(const hack_helper&) = delete;
		hack_helper(hack_helper&&) = delete;
		hack_helper& operator=(const hack_helper&) = delete;
		hack_helper& operator=(hack_helper&&) = delete;

	public:
		bool find_game();

		bool is_game_found()
		{
			return is_game_found_;
		}

		bool write_skill();

		bool write_exp();

		bool write_money(int money);

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



