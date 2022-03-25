#pragma once

#include "my_windows/my_windows.h"

namespace vst {

BOOL CALLBACK dialog_procedure(HWND dialog_handle, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK mod_people_dialog_procedure(HWND dialog_handle, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK mod_item_dialog_procedure(HWND dialog_handle, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK input_item_index_dialog_procedure(HWND dialog_handle, UINT message, WPARAM wParam, LPARAM lParam);

}
