#include "dialog.h"
#include "resource.h"
#include "windowsx.h"
#include "hack_helper.h"

namespace vst
{

	BOOL CALLBACK dialog_procedure(
		HWND dialog_handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam)
	{
		static HWND output_handle = nullptr;
		static auto& hacker = hack_helper::instance();
		switch (message)
		{
		case WM_INITDIALOG:
		{
			output_handle = mw::user::get_child_winodw_from_id(dialog_handle, VST_OUTPUT);
			Edit_LimitText(GetDlgItem(dialog_handle, VST_MONEY_INPUT), 9);
			return true;
		}
		case WM_COMMAND:
		{
			if (!hacker.find_game())
			{
				mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：未找到游戏进程，请确认是否打开游戏！"));
				return true;
			}

			WORD control_message = HIWORD(wParam);
			WORD control_id = LOWORD(wParam);
			HWND control_handle = (HWND)lParam;
			switch (control_id)
			{
			case VST_MAX_SKILL:

				if (!hacker.write_skill())
				{
					mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改游戏人物属性失败！"));
					return true;
				}

				break;
			case VST_MAX_EXP:

				if (!hacker.write_exp())
				{
					mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改游戏人物经验失败！"));
					return true;
				}

				break;
			case VST_MONEY:
			{
				int money = 0;
				if (!mw::user::get_dialog_item_int(dialog_handle, VST_MONEY_INPUT, money))
				{
					mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：获取金钱数量失败！"));
					return true;
				}

				if (!hacker.write_money(money))
				{
					mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改游戏金钱失败！"));
					return true;
				}

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