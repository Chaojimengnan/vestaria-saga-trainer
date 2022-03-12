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
        if (!game_title_str) return false;
        auto game_window = mw::user::find_window(_T(""), game_title_str);

        if (game_window == NULL) return false;

        DWORD process_id = 0;

        mw::user::get_thread_process_id_from_window(game_window, &process_id);

        auto process_handle = mw::open_process(process_id, false, PROCESS_ALL_ACCESS);

        if (process_handle == NULL) return false;

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
            std::array<int, 9> data{};
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

    LPVOID hack_helper::first_()
    {
        auto base_address = get_game_base_address_();

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

    const TCHAR* hack_helper::get_game_title_enum_str(game_title game_title_v)
    {
        switch (game_title_v)
        {
        case vst::game_title::vestaria_1:
            return _T("Vestaria SagaⅠ");
        case vst::game_title::vestaria_ep2:
            return _T("Vestaria EP2");
        default:
            return nullptr;
        }
        
    }

}
