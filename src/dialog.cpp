#include "dialog.h"
#include "resource.h"
#include "windowsx.h"
#include "hack_helper.h"

namespace vst
{

	enum class mod_type {
		skill,
		exp,
		money
	};

	BOOL CALLBACK dialog_procedure(
		HWND dialog_handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam)
	{
		static HWND output_handle = nullptr;
		static HWND game_title_list_handle = nullptr;
		static HWND mod_type_list_handle = nullptr;
		static auto& hacker = hack_helper::instance();
		switch (message)
		{
		case WM_INITDIALOG:
		{
			output_handle = mw::user::get_child_winodw_from_id(dialog_handle, VST_OUTPUT);
			game_title_list_handle = mw::user::get_child_winodw_from_id(dialog_handle, VST_GAME_TITLE_LIST);
			mod_type_list_handle = mw::user::get_child_winodw_from_id(dialog_handle, VST_MOD_TYPE_LIST);

			Edit_LimitText(GetDlgItem(dialog_handle, VST_VALUE_INPUT), 9);

			ListBox_AddString(game_title_list_handle,
				hack_helper::get_game_title_enum_str(game_title::vestaria_1));
			ListBox_AddString(game_title_list_handle,
				hack_helper::get_game_title_enum_str(game_title::vestaria_ep2));


			ListBox_SetCurSel(game_title_list_handle, 0);

			ListBox_AddString(mod_type_list_handle,
				_T("人物属性"));
			ListBox_AddString(mod_type_list_handle,
				_T("人物经验"));
			ListBox_AddString(mod_type_list_handle,
				_T("金钱"));
			

			ListBox_SetCurSel(mod_type_list_handle, 0);

			/*ListBox_SetHorizontalExtent(game_title_list_handle, 110);
			ListBox_SetHorizontalExtent(mod_type_list_handle, 50);*/
			return true;
		}
		case WM_COMMAND:
		{
			auto title_index = ListBox_GetCurSel(game_title_list_handle);
			if (!hacker.find_game(static_cast<game_title>(title_index)))
			{
				mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：未找到游戏进程，请确认是否打开游戏！"));
				return true;
			}

			WORD control_message = HIWORD(wParam);
			WORD control_id = LOWORD(wParam);
			HWND control_handle = (HWND)lParam;
			switch (control_id)
			{
			case VST_MOD:
			{
				int val = 0;
				if (!mw::user::get_dialog_item_int(dialog_handle, VST_VALUE_INPUT, val))
				{
					mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：获取金钱数量失败！"));
					return true;
				}

				auto mod_type_index = static_cast<mod_type>(ListBox_GetCurSel(mod_type_list_handle));

				switch (mod_type_index)
				{
				case vst::mod_type::skill:
					if (!hacker.write_skill(val))
					{
						mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改人物属性失败！"));
						return true;
					}
					break;
				case vst::mod_type::exp:
					if (!hacker.write_exp(val))
					{
						mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改人物经验失败！"));
						return true;
					}
					break;
				case vst::mod_type::money:
					if (!hacker.write_money(val))
					{
						mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改金钱失败！"));
						return true;
					}
					break;
				default:
					mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：请选择一个正确的修改类型！"));
					return true;
				}
				mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("操作成功！"));
				break;
			}
			default:
				break;
			}

			break;
		}
		case WM_CLOSE:
		{
			mw::user::end_modal_dialog(dialog_handle, 0);
			return true;
		}
		}
		return false;
	}

};