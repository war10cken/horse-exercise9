#include <io.h>
#include <fcntl.h>
#include "msoftcon.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <codecvt>

using namespace std;

const int CPF = 5;
const int maxHorses = 7;

class track;
class comtrack;

class horse
{
protected:
    const track* ptrTrack;
    const int horse_number;
    float finish_time;
    float distance_run;
public:
    horse(const int n, const track* ptrT) :
        horse_number(n),
        ptrTrack(ptrT),
        distance_run(0.0)
    { }
    ~horse()
    { }
    void display_horse(const float elapsed_time);
};

class comhorse : public horse
{
private:
    const comtrack* ptrTrack;

public:
    comhorse(const int n, const comtrack* ptrT) :
        horse(n, NULL),
        ptrTrack(ptrT)
    { }
    void display_horse(const float elapsed_time);
};

class track
{
protected:
    horse* hArray[maxHorses];
    int total_horses;
    int horse_count;
    const float track_length;
    float elapsed_time;
public:
    track(float lenT, int nH);
    ~track();
    void display_track();
    void run();
    float get_track_len() const;
};

class comtrack : public track
{
private:
    comhorse* hArray[maxHorses];
    int horse_count;
public:
    comtrack(float lenT, int nH);
    ~comtrack();
    void run();
    friend void comhorse::display_horse(float);
};

void horse::display_horse(float elapsed_time)
{
    set_cursor_pos(1 + int(distance_run * CPF), 2 + horse_number * 2);

    set_color(static_cast<color>(cBLUE + horse_number));

    char horse_char = '0' + static_cast<char>(horse_number);

    putch(' ');
    putch('«');
    putch(horse_char);
    putch('»');

    if (distance_run < ptrTrack->get_track_len() + 1.0 / CPF)
    {
        if (rand() % 3)
            distance_run += 0.2F;
        finish_time = elapsed_time;
    }
    else
    {
        int mins = int(finish_time) / 60;
        int secs = int(finish_time) - mins * 60;
        cout << " Time = " << mins << ":" << secs;
    }
}

void comhorse::display_horse(float elapsed_time)
{
    set_cursor_pos(1 + int(distance_run * CPF), 2 + horse_number * 2);

    set_color(static_cast<color>(cBLUE + horse_number));

    char horse_char = '0' + static_cast<char>(horse_number);


    putch(' ');
    putch('«');
    putch(horse_char);
    putch('»');

    if (distance_run < ptrTrack->get_track_len() + 1.0 / CPF)
    {
        float speed = 0.2F;

        if (distance_run >= ptrTrack->get_track_len() / 3)
        {
            if (ptrTrack->total_horses > 1)
            {
                int Ld, Sc;

                if (ptrTrack->hArray[0]->distance_run < ptrTrack->hArray[1]->distance_run)
                {
                    Ld = 1;
                    Sc = 0;
                }
                else
                {
                    Ld = 0;
                    Sc = 1;
                }

                for (int j = 2; j < ptrTrack->total_horses; j++)
                {
                    if (ptrTrack->hArray[Ld]->distance_run < ptrTrack->hArray[j]->distance_run)
                    {
                        Sc = Ld;
                        Ld = j;
                    }
                    else if (ptrTrack->hArray[Sc]->distance_run < ptrTrack->hArray[j]->distance_run)
                    {
                        Sc = j;
                    }
                }
                if (distance_run == ptrTrack->hArray[Ld]->distance_run ||
                        distance_run == ptrTrack->hArray[Sc]->distance_run)
                {
                    if (ptrTrack->hArray[Ld]->distance_run - ptrTrack->hArray[Sc]->distance_run <= 0.1F)
                        speed = 0.3F;
                }
            }
        }

        if (rand() % 3)
            distance_run += speed;
        finish_time = elapsed_time;
    }
    else
    {
        int mins = int(finish_time) / 60;
        int secs = int(finish_time) - mins * 60;
        cout << " Time = " << mins << ":" << secs;
    }
}

track::track(float lenT, int nH) :
    track_length(lenT),
    total_horses(nH),
    horse_count(0),
    elapsed_time(0.0)
{
    init_graphics();
    total_horses = (total_horses > maxHorses) ? maxHorses : total_horses;

    for (int j = 0; j < total_horses; j++)
        hArray[j] = new horse(horse_count++, this);

    time_t aTime;
    srand(static_cast<unsigned>(time(&aTime)));

    display_track();
}

comtrack::comtrack(float lenT, int nH) :
    track(lenT, nH), horse_count(0)
{
    for (int j = 0; j < total_horses; j++)
        hArray[j] = new comhorse(horse_count++, this);
}

track::~track()
{
    for (int j = 0; j < total_horses; j++)
        delete hArray[j];
}

comtrack::~comtrack()
{
    for (int j = 0; j < total_horses; j++)
        delete hArray[j];
}

void track::display_track()
{
    clear_screen();
    for (int f = 0; f <= track_length; f++)
        for (int r = 1; r <= total_horses * 2 + 1; r++)
        {
            set_cursor_pos(f * CPF + 5, r);
            if (f == 0 || f == track_length)
                cout << '¦';
            else
                cout << '·';
        }
}

void track::run()
{
    while (!_kbhit())
    {
        elapsed_time += 1.75;
        for (int j = 0; j < total_horses; j++)
            hArray[j]->display_horse(elapsed_time);
        wait(500);
    }
    _getche();
    cout << endl;
}

void comtrack::run()
{
    while (!_kbhit())
    {
        elapsed_time += 1.75;
        for (int j = 0; j < total_horses; j++)
            hArray[j]->display_horse(elapsed_time);
        wait(500);
    }
    _getche();
    cout << endl;
}

float track::get_track_len() const
{
    return track_length;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    float length;
    int total;

    cout << "Введите длину дистанции (от 1 до 12 фурлонгов): ";
    cin >> length;
    cout << "Введите количество лошадей (от 1 до 7): ";
    cin >> total;
    cout << endl;

    comtrack theTrack(length, total);
    theTrack.run();
    cout << endl;

    end_graphics();

    return 0;
}
