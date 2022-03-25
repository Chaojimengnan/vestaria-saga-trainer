#include "hack_helper.h"
#include <array>

namespace vst {

hack_helper& hack_helper::instance()
{
    static hack_helper hh;
    return hh;
}

bool hack_helper::find_game(game_title game_title_v)
{
    auto game_title_str = get_game_title_enum_str(game_title_v);
    if (!game_title_str)
        return false;
    auto game_window = mw::user::find_window(_T(""), game_title_str);

    if (game_window == NULL)
        return false;

    DWORD process_id = 0;

    mw::user::get_thread_process_id_from_window(game_window, &process_id);

    auto process_handle = mw::open_process(process_id, false, PROCESS_ALL_ACCESS);

    if (process_handle == NULL)
        return false;

    process_handle_ = process_handle;

    is_game_found_ = true;

    return true;
}

bool hack_helper::write_skill(int val)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    LPVOID item_ptr = first_();

    while (item_ptr)
    {
        std::array<int, 9> data {};
        data.fill(val);
        if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xC, (LPVOID)data.data(), sizeof(int) * 9))
        {
            return false;
        }
        item_ptr = next_(item_ptr);
    }
    return true;
}

bool hack_helper::write_exp(int val)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    LPVOID item_ptr = first_();

    while (item_ptr)
    {
        if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0x140, (LPVOID)&val, sizeof(int)))
        {
            return false;
        }
        item_ptr = next_(item_ptr);
    }
    return true;
}

bool hack_helper::write_money(int money)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    LPVOID money_ptr = (char*)get_game_base_address_();

    money_ptr = get_next_pointer_(money_ptr, 0xB6CDC);

    if (!mw::write_process_memory(process_handle_, (char*)money_ptr + 0x10, (LPVOID)&money, sizeof(int)))
    {
        return false;
    }

    return true;
}

bool hack_helper::write_item_durablity(int val)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    char* item_ptr = static_cast<char*>(first_());

    while (item_ptr)
    {

        for (auto* item_array = item_ptr + 0x1B8;; item_array += 0x4)
        {
            auto* item = static_cast<char*>(get_next_pointer_(item_array, 0x0));

            if (item == nullptr)
                break;

            if (!mw::write_process_memory(process_handle_, item + 0x28, (LPVOID)&val, sizeof(int)))
                return false;

            if (!mw::write_process_memory(process_handle_, item + 0x84, (LPVOID)&val, sizeof(int)))
                return false;
        }

        item_ptr = static_cast<char*>(next_(item_ptr));
    }

    return true;
}

LPVOID hack_helper::first_()
{
    auto* base_address = get_game_base_address_();

    base_address = get_next_pointer_(base_address, 0xB6CDC);
    base_address = get_next_pointer_(base_address, 0x1E0);

    base_address = get_next_pointer_(base_address, 0x4);
    base_address = get_next_pointer_(base_address, 0x4);

    return base_address;
}

LPVOID hack_helper::next_(LPVOID address)
{
    return get_next_pointer_(address, 0x4);
}

LPVOID hack_helper::get_next_pointer_(LPVOID address, int offset)
{
    address = (char*)address + offset;
    if (!mw::read_process_memory(process_handle_, address,
            &address, sizeof(address)))
        return nullptr;

    return address;
}

LPVOID hack_helper::get_game_base_address_()
{
    auto get_module_handle_raw = (decltype(GetModuleHandleA)*)
        mw::get_proc_address(mw::get_module_handle(_T("Kernel32.dll")), "GetModuleHandleA");

    auto thread_handle = mw::create_remote_thread(process_handle_,
        (LPTHREAD_START_ROUTINE)get_module_handle_raw);

    mw::sync::wait_for_single_object(thread_handle, INFINITE);

    DWORD address = 0;
    mw::get_thread_exit_code(thread_handle, address);

    return (LPVOID)address;
}

LPVOID hack_helper::get_current_select_pointer_()
{
    auto* current_people_id = get_game_base_address_();

    current_people_id = get_next_pointer_(current_people_id, 0xB70B8);
    current_people_id = get_next_pointer_(current_people_id, 0x04);

    LPVOID item_ptr = first_();

    LPVOID people_id = nullptr;

    while (true)
    {
        if (item_ptr == nullptr)
            return nullptr;

        if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0x208, &people_id, sizeof(int)))
        {
            return nullptr;
        }

        if (people_id == current_people_id)
            break;

        item_ptr = next_(item_ptr);
    }

    return item_ptr;
}

const TCHAR* hack_helper::get_game_title_enum_str(game_title game_title_v)
{
    switch (game_title_v)
    {
    case vst::game_title::vestaria_1:
        return _T("Vestaria SagaⅠ");
    case vst::game_title::vestaria_ep2:
        return _T("Vestaria EP2");
    case vst::game_title::vestaria_ep1:
        return _T("Vestaria Saga外伝　ルッカの英雄");
    default:
        return nullptr;
    }
}

bool hack_helper::write_select_people(const people_skill& current_people)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    LPVOID item_ptr = get_current_select_pointer_();
    if (item_ptr == nullptr)
        return false;

    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xC, (LPVOID)&current_people, sizeof(current_people)))
        return false;

    return true;
}

bool hack_helper::write_select_item(int index, const item& current_item)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    if (index < 0 || index >= 6)
        return false;

    LPVOID item_ptr = get_current_select_pointer_();
    if (item_ptr == nullptr)
        return false;

    item_ptr = get_next_pointer_(item_ptr, 0X1B8 + index * 0x4);

    if (item_ptr == nullptr)
        return false;


    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0x28, (LPVOID)&current_item.upper_durablity, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0x2C, (LPVOID)&current_item.weight, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0x30, (LPVOID)&current_item.bonus_skill, sizeof(current_item.bonus_skill)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0x84, (LPVOID)&current_item.current_durablity, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xc4, (LPVOID)&current_item.attack, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xc8, (LPVOID)&current_item.lower_range, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xcc, (LPVOID)&current_item.upper_range, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xd0, (LPVOID)&current_item.accuracy, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xd4, (LPVOID)&current_item.criticality, sizeof(int)))
        return false;
    if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xd8, (LPVOID)&current_item.combo_num, sizeof(int)))
        return false;

    return true;
}

bool hack_helper::get_current_select_people(people_skill& current_people)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    LPVOID item_ptr = get_current_select_pointer_();
    if (item_ptr == nullptr)
        return false;

    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xC, (LPVOID)&current_people, sizeof(current_people)))
    {
        return false;
    }

    return true;
}

bool hack_helper::get_current_select_item(int index, item& current_item)
{
    if (!is_game_found_ || process_handle_ == NULL)
        return false;

    if (index < 0 || index >= 6)
        return false;

    LPVOID item_ptr = get_current_select_pointer_();
    if (item_ptr == nullptr)
        return false;

    item_ptr = get_next_pointer_(item_ptr, 0X1B8 + index * 0x4);

    if (item_ptr == nullptr)
        return false;

    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0x28, (LPVOID)&current_item.upper_durablity, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0x2C, (LPVOID)&current_item.weight, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0x30, (LPVOID)&current_item.bonus_skill, sizeof(current_item.bonus_skill)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0x84, (LPVOID)&current_item.current_durablity, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xc4, (LPVOID)&current_item.attack, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xc8, (LPVOID)&current_item.lower_range, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xcc, (LPVOID)&current_item.upper_range, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xd0, (LPVOID)&current_item.accuracy, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xd4, (LPVOID)&current_item.criticality, sizeof(int)))
        return false;
    if (!mw::read_process_memory(process_handle_, (char*)item_ptr + 0xd8, (LPVOID)&current_item.combo_num, sizeof(int)))
        return false;

    return true;
}

} // namespace vst
