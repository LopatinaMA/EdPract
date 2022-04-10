#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <locale.h>
#include "plant.h"

using namespace std;
const int NUM_CONTROL_CHANNELS_MAX = 100;

/*
    num_control_channels - k � ����� ������������ ������� ����������;
    num_inter_sensors - F � ����� ��������, ������������ �� �������;
    num_sens_measure - H - ����� ������� (������ ������), �� �������� ����� ���������� ������ (������� �������)
�� �������� ����������� �����������;
    num_control_cycles - N - ����� ������ ���������� (����������������� ����������)
    control_chan_nums - L1, L2, �, Lk � ������ ������� ����������;
    sensors_nums - M1, M2, �., MF � ������ �������� (������� ������);
    coefs_resp_pred_eq - �0, �1, �2,�, �k+F � ������������ ��������� �������� �������;
    values_control_actions - U1, U2, �, Uk - �������� ����������� �����������;
    X1, X2,�,XF - ��������������� ���������� �������� ����� ������ ������� M1, M2,�., MF;
    Y1pr - �������������� �������� �������;

*/

void input(int num_control_channels, int num_inter_sensors, int num_sens_measure, int num_control_cycles, int S,
           vector <int> control_chan_nums, vector <int> sensors_nums,
           vector <double> coefs_resp_pred_eq)
{
    cout << "������� ����� ������������ ������� ����������: "; cin >> num_control_channels;
    cout << "������� ����� ��������, ������������ �� �������: "; cin >> num_inter_sensors;
    S = num_control_channels + num_inter_sensors;
    cout << "������� ����� �������, �� �������� ����� ���������� ������ ��" << '\n';
    cout << "�������� ����������� �����������: "; cin >> num_sens_measure;
    cout << "������� ����� ������ ����������: "; cin >> num_control_cycles;
    for (int i = 0; i < num_control_channels; i++)
    {
        int t;
        cout << "������� ������ ������� ����������: "; cin >> t;
        control_chan_nums.push_back(t);
    }
    for (int i = 0; i < num_inter_sensors; i++)
    {
        int t;
        cout << "������� ������ ��������: "; cin >> t;
        sensors_nums.push_back(t);
    }
    for (int i = 0; i < S; i++)
    {
        double t;
        cout << "������������ ��������� �������� �������: "; cin >> t;
        coefs_resp_pred_eq.push_back(t);
    }

}

//�������� 5
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

// �������� 8: ��������� �������� 2-7 n ���.
    cout << "������� ���������� ���������� �������� 2-7: ";
    cin >> n;

//�������� 2: ������ ��������� ������������� ��������� k �������� ����������� �����������: U1, U2, �, Uk.
    vector <double> values_control_actions(num_control_channels);
    for (int i = 0; i < n; i++)
    {
        for (int i = 0; i < num_control_channels; i++)
        {
            cout << "������� �������� ����������� �����������: ";
            cin >> values_control_actions[i];
        }
    }

//�������� 3: ���������� �������� U1 � ������ ���������� � ������� L1, �������� U2 � � ������ L2, �., �������� Uk � � ������ Lk.
    for(int t = 0; t < n; t++)
    {
        for (int i = 0; i < num_control_channels; i++)
        {
            plant_control(control_chan_nums[i], values_control_actions[i], plant);
        }
    }


//������ ���������� � 4 �������� ��-�� plant � � ������� �� ���� ��� ��� �������. ������������, ��� � ������: �����-�� ���� � �������.


//�������� 4: ��������������� ���������� ����� ������� M1, M2,�., MF, ��������� ��������������� ���������� �������� X1, X2,�,XF.
    vector <double> X(num_inter_sensors);
    for (int i = 0; i < n; i++)
    {
        for (int i = 0; i < num_inter_sensors; i++)
        {
            X[i] = plant_measure(sensors_nums[i], plant);
        }
    }

//�������� 5: ��������� �������������� �������� ������� �� ������� Y1�� = A0+ A1*U1+ A2*U2+�+ Ak*Uk+ Ak+1*X1+ Ak+2*X2+�+ Ak+F*XF
    vector<double> Ypr(n);
    calc_resp_forecast(coefs_resp_pred_eq, num_control_channels, values_control_actions, X, n, S, Ypr);

//�������� 6: ���������� ����� ������� � ������� � � ��������� ���������� �������� ������� Y1
    vector<double> Y(n);
    for (int i = 0; i < n; i++)
    {
        Y[i] = plant_measure(num_sens_measure, plant);
    }

//�������� 7
    for (int j = 0; j < n; j++)
    {
        for ( int i = 0; i < num_control_channels; i++)
        {
            cout << "�������� ����������� �����������: " << values_control_actions[i];
        }
    }
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < num_inter_sensors; i++)
        {
            cout << "�������� �� ������� ������: " << X[i];
        }
    }
    for (int i = 0; i < n; i++)
    {
        cout << "�������� ������� �� �������� ����������� �����������: " << Y[i];
    }
    for (int i = 0; i < n; i++)
    {
        cout << "�������������� ��������: " << Ypr[i];
    }
    return 0;
}
