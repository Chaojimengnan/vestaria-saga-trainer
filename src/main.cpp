#include "dialog.h"
#include "my_windows/my_windows.h"
#include "resource.h"

int WINAPI _tWinMain(
    HINSTANCE instance_handle,
    HINSTANCE,
    PTSTR cmd_line,
    int cmd_show)
{
    mw::user::create_modal_dialog(VST_MAIN_DIALOG, (DLGPROC)vst::dialog_procedure, nullptr);
    return 0;
}
