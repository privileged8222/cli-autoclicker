#include <iostream>
#include <windows.h>
#include <random>
#include <chrono>
#include <thread>
#include <string>

#define KEY_DOWN(k) GetAsyncKeyState(k) & 0x8000

using namespace std::chrono_literals;

struct settings_t
{
    int m_min_cps, m_max_cps, m_keybind;
    bool m_pressed, m_toggled, m_first_click;
};

auto random_int(int min, int max)
{
    static auto device = std::random_device();
    static auto engine = std::mt19937(device());
    const auto gen = std::uniform_int_distribution<int>(min, max);
    return gen(engine);
}

auto run_keybind(settings_t *settings)
{
    while (!(KEY_DOWN(VK_DELETE)))
    {
        if (KEY_DOWN(settings->m_keybind) && !settings->m_pressed)
        {
            settings->m_pressed = true;
        }
        else if (!(KEY_DOWN(settings->m_keybind)) && settings->m_pressed)
        {
            settings->m_toggled = !settings->m_toggled;
            settings->m_pressed = false;
        }
        std::this_thread::sleep_for(10ms);
    }

    delete settings;
    std::exit(0);
}

auto main(int argc, char* argv[]) -> int
{
    if (argc < 4)
    {
        std::cerr << "<minimum cps> <maximum cps> <keybind>" << std::endl;

        system("pause");

        return -1;
    }

    auto* settings = new settings_t;

    settings->m_toggled = false;
    settings->m_pressed = false;
    settings->m_first_click = false;

    settings->m_min_cps = std::stoi(argv[1]);
    settings->m_max_cps = std::stoi(argv[2]);
    settings->m_keybind = std::stoi(argv[3]);

    SetConsoleTitleA(" ");
	
	// be careful as you have a data race here
	auto* handle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(run_keybind), settings, 0, nullptr);
    auto* window_handle = FindWindowA("LWJGL", nullptr);
	
    while (!(KEY_DOWN(VK_DELETE)))
    {
        while (KEY_DOWN(VK_LBUTTON) && settings->m_toggled)
        {
            if (GetForegroundWindow() == window_handle)
            {
                if (settings->m_first_click)
                {
                    PostMessageA(window_handle, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                    std::this_thread::sleep_for(std::chrono::milliseconds(random_int(15, 35)));
                    settings->m_first_click = false;
                }
                else
                {
                    PostMessageA(window_handle, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
                    const auto down_delay = random_int(15, 35);
                    std::this_thread::sleep_for(std::chrono::milliseconds(down_delay));

                    PostMessageA(window_handle, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                    const auto up_delay = random_int(850, 1150) / (random_int(settings->m_min_cps, settings->m_max_cps) + random_int(-1, 3)) - down_delay;
                    std::this_thread::sleep_for(std::chrono::milliseconds(up_delay));
                }
            }
        }
        std::this_thread::sleep_for(10ms);
        settings->m_first_click = true;
    }

	if (handle) CloseHandle(handle);
	
    delete settings;
    std::exit(0);
}