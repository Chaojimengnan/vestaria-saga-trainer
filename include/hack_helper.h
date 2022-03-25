#pragma once
#include "my_windows/my_windows.h"

namespace vst {

enum class game_title
{
    vestaria_1,
    vestaria_ep2,
    vestaria_ep1
};

class hack_helper
{
public:
    struct people_skill
    {
        int hit_point;    // 生命
        int strength;     // 力量
        int magic;        // 魔力
        int skill;        // 技巧
        int speed;        // 速度
        int lucky;        // 幸运
        int defend;       // 防御
        int magic_defend; // 魔防御
        int move_point;   // 移动力
    };

    struct item
    {
        int upper_durablity;      // 物品耐久上限
        int weight;               // 重量，用于武器
        int current_durablity;    // 当前物品耐久
        int attack;               // 物品攻击力，用于武器
        int lower_range;          // 射程下限，用于武器
        int upper_range;          // 射程上限，用于武器
        int accuracy;             // 命中，用于武器
        int criticality;          // 致命，用于武器
        int combo_num;            // 连击次数，用于武器
        people_skill bonus_skill; // 物品属性增益
    };

    static hack_helper& instance();

    hack_helper(const hack_helper&) = delete;
    hack_helper(hack_helper&&) = delete;
    hack_helper& operator=(const hack_helper&) = delete;
    hack_helper& operator=(hack_helper&&) = delete;
    ~hack_helper() = default;

    bool find_game(game_title);

    [[nodiscard]] bool is_game_found() const
    {
        return is_game_found_;
    }

    bool write_skill(int val);

    bool write_exp(int val);

    bool write_money(int money);

    bool write_item_durablity(int val);

    static const TCHAR* get_game_title_enum_str(game_title);

    bool write_select_people(const people_skill&);

    bool write_select_item(int index, const item&);

    bool get_current_select_people(people_skill&);

    bool get_current_select_item(int index, item&);

private:
    hack_helper() = default;

    LPVOID first_();

    LPVOID next_(LPVOID address);

    LPVOID get_next_pointer_(LPVOID address, int offset);

    LPVOID get_game_base_address_();

    LPVOID get_current_select_pointer_();

    bool is_game_found_ = false;

    HANDLE process_handle_ = nullptr;
};

}; // namespace vst
