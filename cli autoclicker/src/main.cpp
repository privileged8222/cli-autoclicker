#include <iostream>
#include <windows.h>
#include <random>
#include <chrono>
#include <thread>

int min_cps = 8, max_cps = 13;
bool toggled = false, pressed = false, first_click = false;
int keybind = VK_F4;

std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(time(nullptr)) };

int random_int(int min, int max)
{
    std::uniform_int_distribution gen(min, max);
    return gen(mersenne);
}

void run_keybind()
{
    while (!(GetAsyncKeyState(VK_DELETE) & 0x8000))
    {
        if (GetKeyState(keybind) & 0x8000 && !pressed)
        {
            pressed = true;
        }
        else if (!(GetKeyState(keybind) & 0x8000) && pressed)
        {
            toggled = !toggled;
            pressed = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::exit(0);
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "<minimum cps> <maximum cps> <keybind>" << std::endl;

        system("pause");

        return -1;
    }

    min_cps = std::atoi(argv[1]);
    max_cps = std::atoi(argv[2]);

    keybind = std::atoi(argv[3]);

    SetConsoleTitleA(" ");
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)run_keybind, 0, 0, 0);

    while (!(GetAsyncKeyState(VK_DELETE) & 0x8000))
    {
        while (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && toggled)
        {
            if (GetForegroundWindow() == FindWindowA("LWJGL", NULL))
            {
                if (first_click)
                {
                    PostMessageA(FindWindowA("LWJGL", NULL), WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                    std::this_thread::sleep_for(std::chrono::milliseconds(random_int(15, 35)));
                    first_click = false;
                }
                else
                {
                    PostMessageA(FindWindowA("LWJGL", NULL), WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
                    int down_delay = random_int(15, 35);
                    std::this_thread::sleep_for(std::chrono::milliseconds(down_delay));

                    PostMessageA(FindWindowA("LWJGL", NULL), WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                    int up_delay = random_int(850, 1150) / (random_int(min_cps, max_cps) + random_int(-1, 3)) - down_delay;
                    std::this_thread::sleep_for(std::chrono::milliseconds(up_delay));
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        first_click = true;
    }
    std::exit(0);
}