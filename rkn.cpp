/***************************************************************************************************************\
*   Моделирование системы уравнений:                                                                            *
*                                                                                                               *
*   i\frac{\partial^2 A}{\partial x^2} + \frac{\partial A}{\partial\tau} + \frac{\partial A}{\partial z} = J    *
*   \frac{\partial^2\theta}{\partial z^2} = Re\left(A e^{i\theta}\right)                                        *
*   A(\tau)|_{z=0} = KA(\tau)|_{z=L}                                                                            *
*   \theta|_{z=0} = \theta_0\in [0, 2\pi]                                                                       *
*   \frac{\partial\theta}{\partial z} = \Delta                                                                  *
*                                                                                                               *
*   методом Рунге-Кутты-Нистрема                                                                                *
\***************************************************************************************************************/

#include <fstream>
#include <iostream>
#include <QDebug>
#include <QThread>
#include "dialog.h"
#include "rkn.h"

Rkn::Rkn(QObject *parent) : QObject(parent), ic(0.0, 1.0)
{
   Dialog *d = new Dialog(geth(), getL(), getNe(), getAr(), getAi(), getdelta());

   d->exec();

   ifstream in("input.dat");
   if (!in) {
      qDebug() << "Error opening file input.dat";
      exit(1);
   }

   //****************************************************************************************//
   //							 Читаем данные из файла input.dat
   //****************************************************************************************//
   in >> h >> L >> Ne >> Ar >> delta >> phase_space >> draw_trajectories;
   //========================================================================================//
   //						   / Читаем данные из файла input.dat
   //========================================================================================//

   //****************************************************************************************//
   //							 Печатаем в консоли данные из файла input.dat
   //****************************************************************************************//
   cout << "h = " << h << "\nL = " << L << "\nNe = " << Ne << "\ndelta = " << delta << endl << endl;
   //****************************************************************************************//
   //							 Печатаем в консоли данные из файла input.dat
   //****************************************************************************************//

   A = complex<double>(Ar, Ai);
   NZ = int(L / h) + 1;
   hth = 2 * M_PI / Ne;

   cout << "NZ = " << NZ << "\nh = " << hth << endl;

   //****************************************************************************************//
   //							 Массивы для A(z), th(z,th0), dthdz(z,th0)
   //****************************************************************************************//
   th0 = new double *[NZ];
   dthdz0 = new double *[NZ];
   th1 = new double *[NZ];
   dthdz1 = new double *[NZ];
   for (int i = 0; i < NZ; i++) {
      th0[i] = new double[Ne];
      dthdz0[i] = new double[Ne];
      th1[i] = new double[Ne];
      dthdz1[i] = new double[Ne];
   }
   F0 = new double[Ne];
   z = new double[NZ];

   //========================================================================================//
   //						   / Массивы для A(z), th(z,th0), dthdz(z,th0)
   //========================================================================================//

   for (int i = 0; i < NZ; i++) // формируем вектор пространственных координат
   {
      z[i] = i * h;
   }
}

bool Rkn::calculating() const
{
   return m_calculating;
}

void Rkn::setCalculating(bool calculating)
{
   if (m_calculating == calculating)
      return;

   m_calculating = calculating;
   emit calculatingChanged(calculating);

   //    qDebug() << "m_calculating = " << m_calculating;
}

bool Rkn::stop() const
{
   return m_calculating;
}

void Rkn::setStop(bool stop)
{
   if (m_stop == stop)
      return;

   m_stop = stop;
   emit stopChanged(stop);

   //    qDebug() << "m_calculating = " << m_calculating;
}

void Rkn::calculate()
{
   //****************************************************************************************//
   //						     Начальные условия
   //****************************************************************************************//

   for (int k = 0; k < Ne; k++) {
      for (int i = 0; i < NZ; i++) {
         th0[i][k] = hth * k;
         dthdz0[i][k] = 0.0;
      }

      th1[0][k] = hth * k;
      dthdz1[0][k] = delta;
   }

   //========================================================================================//
   //						   / Начальные условия
   //========================================================================================//

   for (int i = 0; i < NZ - 1; i++) {
      for (int k = 0; k < Ne; k++) {
         F0[k] = F(A, th1[i][k] /*, dthdz1[i][k]*/);

         //Предиктор th
         th1[i + 1][k] = th1[i][k] + dthdz1[i][k] * h + h / 2.0 * F0[k] * h;
         //Предиктор dthdz
         dthdz1[i + 1][k] = dthdz1[i][k] + h * F0[k];
      }

      for (int k = 0; k < Ne; k++) {
         //Корректор th
         th1[i + 1][k] = th1[i][k] + dthdz1[i][k] * h + h / 6.0 * F0[k] * h
                         + h / 3.0 * F(A, th1[i + 1][k] /*, dthdz1[i + 1][k]*/) * h;
         //Корректор dthdz
         dthdz1[i + 1][k] = dthdz1[i][k]
                            + h / 2.0 * (F0[k] + F(A, th1[i + 1][k] /*, dthdz1[i + 1][k]*/));

         if (th1[i + 1][k] < thmin)
            thmin = th1[i + 1][k];
         if (th1[i + 1][k] > thmax)
            thmax = th1[i + 1][k];
         if (dthdz1[i + 1][k] < dthmin)
            dthmin = dthdz1[i + 1][k];
         if (dthdz1[i + 1][k] > dthmax)
            dthmax = dthdz1[i + 1][k];
      }

      it = i + 1;
      emit paintSignal();
   }

   if (m_stop) {
      while (m_calculating) {
      }
      emit finished();
   }
   //    }

   cout << endl;

   //========================================================================================//
   //						   / Main part
   //========================================================================================//

   while (m_calculating) {
   }
   emit finished();
}

//****************************************************************************************//
//							 Functions
//****************************************************************************************//

inline double Rkn::F(complex<double> A, double th)
{
   return real(A * exp(ic * th));
}

inline complex<double> Rkn::J(double *th, int Ne)
{
   complex<double> sum = 0;

   for (int k = 0; k < Ne; k++)
      sum += exp(-ic * th[k]);

   return 2.0 / Ne * sum;
}

//========================================================================================//
//						   / Functions
//========================================================================================//
