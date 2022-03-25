#include "dialog.h"
#include "hack_helper.h"
#include "resource.h"
#include "windowsx.h"

namespace vst {

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
        /*mod_type_list_handle = mw::user::get_child_winodw_from_id(dialog_handle, VST_MOD_TYPE_LIST);

			Edit_LimitText(GetDlgItem(dialog_handle, VST_VALUE_INPUT), 9);*/

        ListBox_AddString(game_title_list_handle,
            hack_helper::get_game_title_enum_str(game_title::vestaria_1));
        ListBox_AddString(game_title_list_handle,
            hack_helper::get_game_title_enum_str(game_title::vestaria_ep2));
        ListBox_AddString(game_title_list_handle,
            hack_helper::get_game_title_enum_str(game_title::vestaria_ep1));

        ListBox_SetCurSel(game_title_list_handle, 0);

        /*ListBox_SetHorizontalExtent(game_title_list_handle, 120);
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
        case VST_EXP_MAX:
            if (!hacker.write_exp(999999))
            {
                mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改人物经验失败！"));
                return true;
            }
            break;
        case VST_MONEY_MAX:
            if (!hacker.write_money(999999))
            {
                mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改金钱失败！"));
                return true;
            }
            break;
        case VST_SKILL_MAX:
            if (!hacker.write_skill(99))
            {
                mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改人物属性失败！"));
                return true;
            }
            break;
        case VST_ITEM_DURABILITY_MAX:
            if (!hacker.write_item_durablity(99))
            {
                mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：修改物品耐久失败！"));
                return true;
            }
            break;
        case VST_MOD_CHOOSE:
        {

            auto return_val = mw::user::create_modal_dialog(VST_MOD_PEOPLE_DIALOG, mod_people_dialog_procedure, dialog_handle);

            if (return_val > 1)
            {
                if (return_val == 2)
                {
                    mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：获取或设置当前选中人物失败，请确认是否选中正确人物！"));
                } else if (return_val == 3)
                {
                    mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：未找到游戏进程，请确认是否打开游戏！"));
                }
            } else if (return_val == -1)
            {
                mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：出现未知错误！"));
            }
        } break;
        case VST_MOD_CHOOSE_ITEM:
        {
            // TODO: 获取物品索引

            auto return_index = mw::user::create_modal_dialog(VST_INPUT_ITEM_INDEX_DIALOG,
                input_item_index_dialog_procedure, dialog_handle);

            if (return_index != INT_MAX)
            {
                if (return_index < 1 || return_index > 6)
                {
                    mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：物品索引必须在1到6之间！"));
                    return false;
                }
            } else {
                return false;
            }

            auto return_val = mw::user::create_modal_dialog(VST_MOD_ITEM_DIALOG, mod_item_dialog_procedure, dialog_handle, return_index - 1);

            if (return_val > 1)
            {
                if (return_val == 2)
                {
                    mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：获取或设置当前选中人物物品失败，请确认是否选中正确人物或设置正确的物品索引！"));
                } else if (return_val == 3)
                {
                    mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：未找到游戏进程，请确认是否打开游戏！"));
                }
            } else if (return_val == -1)
            {
                mw::user::set_dialog_item_text(dialog_handle, VST_OUTPUT, _T("错误：出现未知错误！"));
            }
        } break;

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

BOOL CALLBACK mod_people_dialog_procedure(
    HWND dialog_handle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    static auto& hacker = hack_helper::instance();
    static hack_helper::people_skill current_people {};
    switch (message)
    {
    case WM_INITDIALOG:
    {
        if (!hacker.is_game_found())
        {
            mw::user::end_modal_dialog(dialog_handle, 3);
            return true;
        }

        if (!hacker.get_current_select_people(current_people))
        {
            mw::user::end_modal_dialog(dialog_handle, 2);
            return true;
        }

        //mw::user::show_window(dialog_handle);

        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_HIT_POINT), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_STRENGTH), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_MAGIC), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_SKILL), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_SPEED), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_LUCKY), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_DEFEND), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_MAGIC_DEFEND), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_PEOPLE_MOVE_POINT), 9);

        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_HIT_POINT, current_people.hit_point);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_STRENGTH, current_people.strength);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_MAGIC, current_people.magic);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_SKILL, current_people.skill);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_SPEED, current_people.speed);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_LUCKY, current_people.lucky);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_DEFEND, current_people.defend);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_MAGIC_DEFEND, current_people.magic_defend);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_MOVE_POINT, current_people.move_point);

        return true;
    }
    case WM_COMMAND:
    {
        if (!hacker.is_game_found())
        {
            mw::user::end_modal_dialog(dialog_handle, 3);
            return true;
        }
        WORD control_message = HIWORD(wParam);
        WORD control_id = LOWORD(wParam);
        HWND control_handle = (HWND)lParam;
        switch (control_id)
        {
        case VST_MOD_PEOPLE_YES:

            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_HIT_POINT, current_people.hit_point);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_STRENGTH, current_people.strength);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_MAGIC, current_people.magic);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_SKILL, current_people.skill);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_SPEED, current_people.speed);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_LUCKY, current_people.lucky);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_DEFEND, current_people.defend);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_MAGIC_DEFEND, current_people.magic_defend);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_PEOPLE_MOVE_POINT, current_people.move_point);

            if (!hacker.write_select_people(current_people))
            {
                mw::user::end_modal_dialog(dialog_handle, 2);
                return true;
            }

            mw::user::end_modal_dialog(dialog_handle, 1);
            break;
        case VST_MOD_PEOPLE_NO:
            mw::user::end_modal_dialog(dialog_handle, 1);
            break;
        default:
            break;
        }
    } break;
    case WM_CLOSE:
    {
        mw::user::end_modal_dialog(dialog_handle, 1); // 如果是模态对话框
        return true;
    }
    default:
        break;
    }
    return false;
}

BOOL CALLBACK mod_item_dialog_procedure(
    HWND dialog_handle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    static auto& hacker = hack_helper::instance();
    static hack_helper::item current_item {};
    static int current_item_index = -1;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        if (!hacker.is_game_found())
        {
            mw::user::end_modal_dialog(dialog_handle, 3);
            return true;
        }

        current_item_index = lParam;

        if (!hacker.get_current_select_item(current_item_index, current_item))
        {
            mw::user::end_modal_dialog(dialog_handle, 2);
            return true;
        }

        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_HIT_POINT), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_STRENGTH), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_MAGIC), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_SKILL), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_SPEED), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_LUCKY), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_DEFEND), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_MAGIC_DEFEND), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_MOVE_POINT), 9);

        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_UPPER_DURABLITY), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_WEIGHT), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_CURRENT_DURABLITY), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_ATTACK), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_LOWER_RANGE), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_UPPER_RANGE), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_ACCURARY), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_CRITICALITY), 9);
        Edit_LimitText(GetDlgItem(dialog_handle, VST_MOD_ITEM_COMBO_NUM), 9);

        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_HIT_POINT, current_item.bonus_skill.hit_point);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_STRENGTH, current_item.bonus_skill.strength);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_MAGIC, current_item.bonus_skill.magic);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_SKILL, current_item.bonus_skill.skill);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_SPEED, current_item.bonus_skill.speed);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_LUCKY, current_item.bonus_skill.lucky);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_DEFEND, current_item.bonus_skill.defend);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_MAGIC_DEFEND, current_item.bonus_skill.magic_defend);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_MOVE_POINT, current_item.bonus_skill.move_point);

        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_UPPER_DURABLITY, current_item.upper_durablity);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_WEIGHT, current_item.weight);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_CURRENT_DURABLITY, current_item.current_durablity);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_ATTACK, current_item.attack);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_LOWER_RANGE, current_item.lower_range);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_UPPER_RANGE, current_item.upper_range);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_ACCURARY, current_item.accuracy);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_CRITICALITY, current_item.criticality);
        mw::user::set_dialog_item_int(dialog_handle, VST_MOD_ITEM_COMBO_NUM, current_item.combo_num);

        return true;
    }
    case WM_COMMAND:
    {
        if (!hacker.is_game_found())
        {
            mw::user::end_modal_dialog(dialog_handle, 3);
            return true;
        }
        WORD control_message = HIWORD(wParam);
        WORD control_id = LOWORD(wParam);
        HWND control_handle = (HWND)lParam;
        switch (control_id)
        {
        case VST_MOD_ITEM_YES:

            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_HIT_POINT, current_item.bonus_skill.hit_point);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_STRENGTH, current_item.bonus_skill.strength);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_MAGIC, current_item.bonus_skill.magic);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_SKILL, current_item.bonus_skill.skill);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_SPEED, current_item.bonus_skill.speed);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_LUCKY, current_item.bonus_skill.lucky);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_DEFEND, current_item.bonus_skill.defend);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_MAGIC_DEFEND, current_item.bonus_skill.magic_defend);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_MOVE_POINT, current_item.bonus_skill.move_point);

            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_UPPER_DURABLITY, current_item.upper_durablity);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_WEIGHT, current_item.weight);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_CURRENT_DURABLITY, current_item.current_durablity);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_ATTACK, current_item.attack);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_LOWER_RANGE, current_item.lower_range);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_UPPER_RANGE, current_item.upper_range);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_ACCURARY, current_item.accuracy);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_CRITICALITY, current_item.criticality);
            mw::user::get_dialog_item_int(dialog_handle, VST_MOD_ITEM_COMBO_NUM, current_item.combo_num);

            if (!hacker.write_select_item(current_item_index, current_item))
            {
                mw::user::end_modal_dialog(dialog_handle, 2);
                return true;
            }

            mw::user::end_modal_dialog(dialog_handle, 1);
            break;
        case VST_MOD_ITEM_NO:
            mw::user::end_modal_dialog(dialog_handle, 1);
            break;
        default:
            break;
        }
    } break;
    case WM_CLOSE:
    {
        mw::user::end_modal_dialog(dialog_handle, 1); // 如果是模态对话框
        return true;
    }
    default:
        break;
    }
    return false;
}

BOOL CALLBACK input_item_index_dialog_procedure(
    HWND dialog_handle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        WORD control_message = HIWORD(wParam);
        WORD control_id = LOWORD(wParam);
        HWND control_handle = (HWND)lParam;
        switch (control_id)
        {
        case VST_INPUT_ITEM_INDEX_YES:
        {
            int return_val = INT_MAX;
            mw::user::get_dialog_item_int(dialog_handle, VST_INPUT_ITEM_INDEX_INPUT, return_val);
            mw::user::end_modal_dialog(dialog_handle, return_val);
        } break;
        case VST_INPUT_ITEM_INDEX_NO:
            mw::user::end_modal_dialog(dialog_handle, INT_MAX);
            break;
        default:
            break;
        }
    } break;
    case WM_CLOSE:
    {
        mw::user::end_modal_dialog(dialog_handle, INT_MAX); // 如果是模态对话框
        return true;
    }
    }
    return false;
}

} // namespace vst