#include "hack_helper.h"


namespace vst {

    hack_helper& hack_helper::instance()
    {
        static hack_helper hh;
        return hh;
    }

    bool hack_helper::find_game()
    {
        auto game_window = mw::user::find_window(_T(""), _T("Vestaria SagaⅠ"));

        if (game_window == NULL) return false;

        DWORD process_id = 0;

        mw::user::get_thread_process_id_from_window(game_window, &process_id);

        auto process_handle = mw::open_process(process_id, false, PROCESS_ALL_ACCESS);

        if (process_handle == NULL) return false;

        process_handle_ = process_handle;

        is_game_found_ = true;



        return true;
    }

    bool hack_helper::write_skill()
    {
        if (!is_game_found_ || process_handle_ == NULL)
            return false;

        LPVOID item_ptr = first_();

        while (item_ptr)
        {
            int val[9] = { 99 , 99, 99, 99, 99, 99, 99, 99, 99};
            if (!mw::write_process_memory(process_handle_, (char*)item_ptr + 0xC, (LPVOID)val, sizeof(int) * 9))
            {
                return false;
            }
            item_ptr = next_(item_ptr);
        }
        return true;
    }

    bool hack_helper::write_exp()
    {
        if (!is_game_found_ || process_handle_ == NULL)
            return false;

        LPVOID item_ptr = first_();

        while (item_ptr)
        {
            int val = 999999;
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

}
