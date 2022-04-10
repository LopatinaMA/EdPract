#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <locale.h>
#include "plant.h"

using namespace std;
const int NUM_CONTROL_CHANNELS_MAX = 100;

/*
    num_control_channels - k – число используемых каналов управления;
    num_inter_sensors - F – число датчиков, опрашиваемых на объекте;
    num_sens_measure - H - номер датчика (канала опроса), по которому будет измеряться отклик (реакция объекта)
на поданные управляющие воздействия;
    num_control_cycles - N - число тактов управления (продолжительность управления)
    control_chan_nums - L1, L2, …, Lk – номера каналов управления;
    sensors_nums - M1, M2, …., MF – номера датчиков (каналов опроса);
    coefs_resp_pred_eq - А0, А1, А2,…, Аk+F – коэффициенты уравнения прогноза отклика;
    values_control_actions - U1, U2, …, Uk - значения управляющих воздействий;
    X1, X2,…,XF - соответствующие измеренные значения после опроса каналов M1, M2,…., MF;
    Y1pr - прогнозируемое значение отклика;

*/

void input(int num_control_channels, int num_inter_sensors, int num_sens_measure, int num_control_cycles, int S,
           vector <int> control_chan_nums, vector <int> sensors_nums,
           vector <double> coefs_resp_pred_eq)
{
    cout << "Введите число используемых каналов управления: "; cin >> num_control_channels;
    cout << "Введите число датчиков, опрашиваемых на объекте: "; cin >> num_inter_sensors;
    S = num_control_channels + num_inter_sensors;
    cout << "Введите номер датчика, по которому будет измеряться отклик на" << '\n';
    cout << "поданные управляющие воздействия: "; cin >> num_sens_measure;
    cout << "Введите число тактов управления: "; cin >> num_control_cycles;
    for (int i = 0; i < num_control_channels; i++)
    {
        int t;
        cout << "Введите номера каналов управления: "; cin >> t;
        control_chan_nums.push_back(t);
    }
    for (int i = 0; i < num_inter_sensors; i++)
    {
        int t;
        cout << "Введите номера датчиков: "; cin >> t;
        sensors_nums.push_back(t);
    }
    for (int i = 0; i < S; i++)
    {
        double t;
        cout << "Коэффициенты уравнения прогноза отклика: "; cin >> t;
        coefs_resp_pred_eq.push_back(t);
    }

}

//Операция 5
void calc_resp_forecast(vector<double> coefs_resp_pred_eq, int num_control_channels, vector <double> values_control_actions,
                        vector<double> X, int n, int S, vector<double> Ypr)
{
    for (int t = 0; t < n; t++)
    {
        Ypr[t] = coefs_resp_pred_eq[0];
        for (int i = 1; i < S; i++)
        {
            if ( i <= num_control_channels)
            {
                Ypr[t] += coefs_resp_pred_eq[i] * values_control_actions[i-1];
            }
            else
            {
                Ypr[t] += coefs_resp_pred_eq[i] * X[i-num_control_channels-1];
            }
        }
    }
}

int main()
{
    setlocale(0,"Russian");

    Plant plant;
    plant_init(plant);

    int num_control_channels, num_inter_sensors, num_sens_measure, num_control_cycles;
    int S, n;
    vector <int> control_chan_nums, sensors_nums;
    vector <double> coefs_resp_pred_eq;
    input(num_control_channels, num_inter_sensors, num_sens_measure, num_control_cycles, S, control_chan_nums,
          sensors_nums, coefs_resp_pred_eq);

// Операция 8: повторить операции 2-7 n раз.
    cout << "Введите количество повторений операций 2-7: ";
    cin >> n;

//Операция 2: Ввести выбранные пользователем программы k значений управляющих воздействий: U1, U2, …, Uk.
    vector <double> values_control_actions(num_control_channels);
    for (int i = 0; i < n; i++)
    {
        for (int i = 0; i < num_control_channels; i++)
        {
            cout << "Введите значения управляющих воздействий: ";
            cin >> values_control_actions[i];
        }
    }

//Операция 3: Установить значение U1 в канале управления с номером L1, значение U2 – в канале L2, …., значение Uk – в канале Lk.
    for(int t = 0; t < n; t++)
    {
        for (int i = 0; i < num_control_channels; i++)
        {
            plant_control(control_chan_nums[i], values_control_actions[i], plant);
        }
    }


//Ошибки начинаются с 4 операции из-за plant и я понятия не имею как это фиксить. Единственное, что я поняла: какие-то беды с памятью.


//Операция 4: Последовательно произвести опрос каналов M1, M2,…., MF, запомнить соответствующие измеренные значения X1, X2,…,XF.
    vector <double> X(num_inter_sensors);
    for (int i = 0; i < n; i++)
    {
        for (int i = 0; i < num_inter_sensors; i++)
        {
            X[i] = plant_measure(sensors_nums[i], plant);
        }
    }

//Операция 5: Вычислить прогнозируемое значение отклика по формуле Y1пр = A0+ A1*U1+ A2*U2+…+ Ak*Uk+ Ak+1*X1+ Ak+2*X2+…+ Ak+F*XF
    vector<double> Ypr(n);
    calc_resp_forecast(coefs_resp_pred_eq, num_control_channels, values_control_actions, X, n, S, Ypr);

//Операция 6: Произвести опрос датчика с номером Н и запомнить измеренное значение отклика Y1
    vector<double> Y(n);
    for (int i = 0; i < n; i++)
    {
        Y[i] = plant_measure(num_sens_measure, plant);
    }

//Операция 7
    for (int j = 0; j < n; j++)
    {
        for ( int i = 0; i < num_control_channels; i++)
        {
            cout << "Значения управляющих воздействий: " << values_control_actions[i];
        }
    }
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < num_inter_sensors; i++)
        {
            cout << "Значения на каналах опроса: " << X[i];
        }
    }
    for (int i = 0; i < n; i++)
    {
        cout << "Значение отклика на поданные управляющие воздействия: " << Y[i];
    }
    for (int i = 0; i < n; i++)
    {
        cout << "Прогнозируемое значение: " << Ypr[i];
    }
    return 0;
}
