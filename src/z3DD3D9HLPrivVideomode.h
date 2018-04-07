/* This file is a part of Zavod3D engine project.
It's licensed unser the MIT license (see "License.txt" for details).*/

#ifndef Z3DD3D9HL_PRIVVIDEOMODE_H
#define Z3DD3D9HL_PRIVVIDEOMODE_H

#include <vector>
#include <string>
#include <algorithm>

namespace z3D_priv
{

/* Предикат сортировки видеорежимов.
Видеорежимы сортируются таким образом, что видеорежимы будут располагаться в массиве в следующем порядке:
в порядке возрастания ширины;
при равных значениях ширины - в порядке возрастания высоты;
при равных ширине и высоте - по принципу:
[частоты, большие или равные заданной в порядке возрастания][частоты, меньшие заданной в порядке убывания].
*/
template <typename V, typename R>
class VideoModeSortPred{
    R curRefreshRate_; // текущая частота развертки монитора
public:
    VideoModeSortPred(R refreshRate) : curRefreshRate_(refreshRate){}
    bool operator() (const V& v1, const V& v2){
        if (v1.Width() < v2.Width()) return true;
        else if (v1.Width() == v2.Width()){
            if (v1.Height() < v2.Height()) return true;
            else if (v1.Height() == v2.Height()){
                // Частоты, большие или равные заданной, располагаем перед частотами, меньшими заданной
                if (v1.RefreshRate() >= curRefreshRate_ && v2.RefreshRate() < curRefreshRate_){
                    return true;
                }
                // Частоты, большие или равные заданной, располагаем в порядке возрастания
                else if (v1.RefreshRate() >= curRefreshRate_ && v2.RefreshRate() >= curRefreshRate_){
                    if (v1.RefreshRate() < v2.RefreshRate()) return true;
                }
                // Частоты, меньшие заданной, располагаем в порядке убывания
                else if (v1.RefreshRate() < curRefreshRate_ && v2.RefreshRate() < curRefreshRate_){
                    if (v1.RefreshRate() > v2.RefreshRate()) return true;
                }

                // Частоты, меньшие заданной, не располагаем перед частотами, большими или равными заданной
            }
        }

        return false;
    }
};

/* Предикат для использования в процедуре, которая оставляет в массиве только видеорежимы с наиболее
близкой частотой обновления экрана и удаляет остальные.
*/
template <typename V, typename R>
class VideoModeUniqPred{
public:
    R curRefreshRate_; // текущая частота развертки монитора
    VideoModeUniqPred(R refreshRate) : curRefreshRate_(refreshRate) {}
    bool operator () (const V& v1, const V& v2){
        if (v1.Width() == v2.Width() && v1.Height() == v2.Height()){
            if (v1.RefreshRate() == v2.RefreshRate()) // 100% дубликат
                return true;
            else if (v1.RefreshRate() == curRefreshRate_) // второе сравниваемое значение уже не имеет смысла
                return true;
            else if (v1.RefreshRate() > curRefreshRate_ && v2.RefreshRate() > curRefreshRate_){
                if (v1.RefreshRate() < v2.RefreshRate()) return true; // удаляем более высокое большее значение
            }
            else if (v1.RefreshRate() < curRefreshRate_ && v2.RefreshRate() < curRefreshRate_){
                if (v1.RefreshRate() > v2.RefreshRate()) return true; // оставляем более высокое значение среди меньших
            }
            else if (v1.RefreshRate() > curRefreshRate_ && v2.RefreshRate() < curRefreshRate_){
                return true; // оставляем более высокое значение
            }
        }

        return false;
    }
};


/** Оставить в массиве подходящий список видеорежимов.

Оставить в массиве видеорежимов, только видеорежимы, различающиеся шириной или
высотой (при равных значениях ширины). Если ширина и высота видеорежимов в массиве
совпадают, то оставить только видеорежим с частотой обновления экрана, наиболее близкой
к заданной частоте.

V тип, описывающий видеорежим.
R тип, описывающий частоту развертки. Целое число.
Требования к типу V - реализация операций:
 -  возвращения значения ширины Width();
 -  возвращения значения высоты Height();
 -  возвращения значения частоты развертки R RefreshRate().

@param videoModes массив с видеорежимами, который нужно "проредить".
@param refreshRate текущая или желаемая частота обновления экрана.
*/
template <typename V, typename R>
void LeaveVideoModeWithClosestRefreshRates(std::vector<V>& videoModes, R refreshRate){

    std::sort(videoModes.begin(), videoModes.end(), VideoModeSortPred<V, R>(refreshRate));
    videoModes.erase(std::unique(videoModes.begin(), videoModes.end(), VideoModeUniqPred<V, R>(refreshRate)),
                     videoModes.end());
}
} // end of z3D_priv
#endif // Z3DD3D9HL_PRIVVIDEOMODE_H


