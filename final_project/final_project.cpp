// testproject.cpp : Defines the entry point for the application.
//

typedef unsigned int uint;
typedef unsigned long long uint64;
#include "final_project.h"
#include "lua.hpp"
#include "math.h"
#include "cmath"
#include <fstream>
using namespace std;

ofstream oFile;
ofstream seedfile;
ifstream seedinput;

uint world_seed = 0;
double random_seed = 0;

uint64 SetRandomSeedHelper(double r)
{
    uint64 e = *(uint64*)&r;
    if (((e >> 0x20 & 0x7fffffff) < 0x7FF00000) && (-9.223372036854776e+18 <= r) && (r < 9.223372036854776e+18))
    {
        // should be same as e &= ~(1<<63); which should also just clears the sign bit,
        // or maybe it does nothing,
        // but want to keep it as close to the assembly as possible for now
        e <<= 1;
        e >>= 1;
        double s = *(double*)&e;
        uint64 i = 0;
        if (s != 0.0)
        {
            uint64 f = (((uint64)e) & 0xfffffffffffff) | 0x0010000000000000;
            uint64 g = 0x433 - ((uint64)e >> 0x34);
            uint64 h = f >> g;

            int j = -(uint)(0x433 < ((e >> 0x20) & 0xFFFFFFFF) >> 0x14);
            i = (uint64)j << 0x20 | j;
            i = ~i & h | f << (((uint64)s >> 0x34) - 0x433) & i;
            i = ~- (uint64)(r == s) & -i | i & -(uint64)(r == s);
            // error handling, whatever
            // f = f ^
            // if((int) g > 0 && f )
        }
        return i & 0xFFFFFFFF;
    }

    // error!
    uint64 error_ret_val = 0x8000000000000000;
    return *(double*)&error_ret_val;
}

uint SetRandomSeedHelper2(uint a, uint b, uint ws)
{
    uint uVar1;
    uint uVar2;
    uint uVar3;

    uVar2 = (a - b) - ws ^ ws >> 0xd;
    uVar1 = (b - uVar2) - ws ^ uVar2 << 8;
    uVar3 = (ws - uVar2) - uVar1 ^ uVar1 >> 0xd;
    uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 0xc;
    uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 0x10;
    uVar3 = (uVar3 - uVar2) - uVar1 ^ uVar1 >> 5;
    uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 3;
    uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 10;
    return (uVar3 - uVar2) - uVar1 ^ uVar1 >> 0xf;
}

float Randomf()
{
    int b = (int)random_seed * 0x41a7 + ((int)random_seed / 0x1f31d) * -0x7fffffff;
    if (b < 1)
    {
        b += 0x7fffffff;
    }
    random_seed = (double)b;
    return (random_seed * 4.656612875e-10);
}

int Random(int a, int b){
    int iVar1;

    iVar1 = (int)random_seed * 0x41a7 + ((int)random_seed / 0x1f31d) * -0x7fffffff;
    if (iVar1 < 1) {
        iVar1 = iVar1 + 0x7fffffff;
    }
    random_seed = (double)iVar1;
    return a - (int)((double)((b - a) + 1) * (double)iVar1 * -4.656612875e-10);
}

double search_x = 14957;
double search_y = 18654;

int SetRandomSeed(lua_State* L) {
    uint a = world_seed ^ 0x93262e6f;
    uint b = a & 0xfff;
    uint c = (a >> 0xc) & 0xfff;

    double x_ = search_x + b;
    double y_ = search_y + c;

    double r = x_ * 134217727.0;
    uint64 e = SetRandomSeedHelper(r);

    uint64 _x = (*(uint64*)&x_ & 0x7FFFFFFFFFFFFFFF);
    uint64 _y = (*(uint64*)&y_ & 0x7FFFFFFFFFFFFFFF);
    if (102400.0 <= *((double*)&_y) || *((double*)&_x) <= 1.0)
    {
        r = y_ * 134217727.0;
    }
    else
    {
        double y__ = y_ * 3483.328;
        double t = e;
        y__ += t;
        y_ *= y__;
        r = y_;
    }

    uint64 f = SetRandomSeedHelper(r);

    uint g = SetRandomSeedHelper2(e, f, world_seed);
    double s = g;
    s /= 4294967295.0;
    s *= 2147483639.0;
    s += 1.0;

    if (2147483647.0 <= s)
    {
        s = s * 0.5;
    }
    random_seed = s;

    Random(0, 0);

    uint h = world_seed & 3;
    while (h)
    {
        Random(0, 0);
        h--;
    }
    return 0;
}

extern "C" {

    int OtherRand(lua_State* L) {
        lua_Number a = lua_tonumber(L, -2);
        lua_Number b = lua_tonumber(L, -1);
        lua_pushnumber(L, (int)Random(a, b));
        return 1;
    }

    int OtherRandomf(lua_State* L) {
        lua_pushnumber(L, Randomf());;
        return 1;
    }

    float GetDistribution(float mean, float sharpness, float baseline)
    {
        int i = 0;
        do
        {
            float r1 = Randomf();
            float r2 = Randomf();
            float div = fabs(r1 - mean);
            if (r2 < ((1.0 - div) * baseline))
            {
                return r1;
            }
            if (div < 0.5)
            {
                // double v11 = sin(((0.5f - mean) + r1) * M_PI);
                float v11 = sin(((0.5f - mean) + r1) * 3.1415f);
                float v12 = pow(v11, sharpness);
                if (v12 > r2)
                {
                    return r1;
                }
            }
            i++;
        } while (i < 100);
        return Randomf();
    }

    float RandomDistf(float min, float max, float mean, float sharpness)
    {
        if (sharpness == 0.0)
        {
            float r = Randomf();
            return r;
        }
        float adjMean = (mean - min) / (max - min);
        return GetDistribution(adjMean, sharpness, 0.005f); // Baseline is always this
    }

    int RandomDist(int min, int max, int mean, float sharpness)
    {
        if (sharpness == 0)
        {
            return Random(min, max);
        }

        float adjMean = (float)(mean - min) / (float)(max - min);
        float v7 = GetDistribution(adjMean, sharpness, 0.005f); // Baseline is always this
        int d = (int)round((float)(max - min) * v7);
        return min + d;
    }

    int RandomDistributionf(lua_State* L)
    {
        lua_Number min = lua_tonumber(L, -4);
        lua_Number max = lua_tonumber(L, -3);
        lua_Number mean = lua_tonumber(L, -2);
        lua_Number sharp = lua_tonumber(L, -1);
        lua_pushnumber(L, min + (max - min) * RandomDistf(min, max, mean, sharp));
        return 1;
    }

    int RandomDistribution(lua_State* L)
    {
        lua_Number min = lua_tonumber(L, -4);
        lua_Number max = lua_tonumber(L, -3);
        lua_Number mean = lua_tonumber(L, -2);
        lua_Number sharp = lua_tonumber(L, -1);
        lua_pushnumber(L, RandomDist(min, max, mean, sharp));
        return 1;
    }
}

int current_cap = 0;

int main() {
    cout << "77 Capacity Seed Search\n-----Made By Extol------\n Special Thanks To:\nkaliuresis\nTwoAbove\ndextercd\nNathanSkimScam\n";
    // I'm sorry 🍔🍔🍔 but you don't show up in console kekw
    system("pause");
    seedinput.open("lastseed.txt");
    if (!seedinput.is_open()) {
        cout << "Error openning last_seed.txt! Please make one!";
        system("pause");
        return 0;
    };
    int starting_seed;
    seedinput>>starting_seed;
    seedinput>>search_x;
    seedinput>>search_y;
    seedinput.close();
    cout << "Last Search: " << starting_seed << "\nSearch from : ";
    cin >> starting_seed;
    cout << "Previous X,Y Search: " << search_x << ", " << search_y << "\nDefault X,Y: 14957, 18654 \nSearch X: ";
    cin >> search_x;
    cout << "Search Y: ";
    cin >> search_y;
    int starting_cap;
    cout << "Starting Capacity: ";
    cin >> starting_cap;
    current_cap = starting_cap;
    world_seed = starting_seed;
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, SetRandomSeed);
    lua_setglobal(L, "SetRandomSeed");
    lua_pushcfunction(L, OtherRand);
    lua_setglobal(L, "Random");
    lua_pushcfunction(L, OtherRandomf);
    lua_setglobal(L, "Randomf");
    lua_pushcfunction(L, RandomDistributionf);
    lua_setglobal(L, "RandomDistributionf");
    lua_pushcfunction(L, RandomDistribution);
    lua_setglobal(L, "RandomDistribution");
    bool errors = luaL_dofile(L, "gun_procedural.lua");
    if (errors) {
        auto error_str = lua_tolstring(L, lua_gettop(L), nullptr);
        std::cerr << "dofile error: " << error_str << '\n';
        system("pause");
        return 0;
    }
    while (true) {
        if (world_seed >= UINT_MAX) {
            cout << "ALL SEEDS SEARCHED! CLOSE AND CHECK OUTPUT";
            system("pause");
            break;
        }
        lua_getglobal(L, "generate_gun");
        lua_pushnumber(L, current_cap);
        lua_pcall(L, 1, 1, 0);
        lua_Number new_found_cap = lua_tonumber(L, -1);
        cout << "Seed: " << world_seed << "  Capacity: " << new_found_cap << "\n";
        seedfile.open("lastseed.txt");
        if (!seedfile.is_open()) {
            cout << "Error openning lastseed.txt?";
            system("pause");
            return 0;
        };
        seedfile << world_seed;
        seedfile << search_x;
        seedfile << search_y;
        seedfile.close();
        lua_resetthread(L);
        if (new_found_cap > current_cap) {
            oFile.open("output.txt", ios::app);
            if (!oFile.is_open()) {
                cout << "Error openning output.txt?";
                system("pause");
                return 0;
            };
            oFile << "Seed: " << world_seed << "  Capacity: " << new_found_cap << "  X,Y: " << search_x << ", " << search_y << "\n";
            oFile.close();
            current_cap = new_found_cap;
            cout << "Paused...\n";
            system("pause");
            cout << "Resumed\n";
        };
        world_seed = world_seed + 1;
    }
    lua_close(L);
    return(0);
}